#include "FileHandle.hpp"

#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <string>
#include <type_traits>

#include <dirent.h>
#include <unistd.h>

namespace {
// Copy constructor and copy assignment should be deleted.
// Move constructor and move assignment should exist.
static_assert(!std::is_copy_constructible_v<FileHandle>);
static_assert(!std::is_copy_assignable_v<FileHandle>);
static_assert(std::is_move_constructible_v<FileHandle>);
static_assert(std::is_move_assignable_v<FileHandle>);

std::size_t OpenDescriptorCount() {
    DIR* directory = ::opendir("/proc/self/fd");
    if (directory == nullptr) {
        return 0;
    }

    std::size_t count = 0;
    while (::readdir(directory) != nullptr) {
        ++count;
    }
    ::closedir(directory);

    // Exclude the "." and ".." directory entries.
    return count - 2;
}

class FileHandleTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto uniqueName = "pizzahut-filehandle-test-" +
            std::to_string(::getpid()) + "-" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());

        testDirectory = std::filesystem::temp_directory_path() / uniqueName;
        ASSERT_TRUE(std::filesystem::create_directory(testDirectory));
    }

    void TearDown() override {
        std::error_code error;
        std::filesystem::remove_all(testDirectory, error);
    }

    std::filesystem::path FilePath(std::string_view name) const {
        return testDirectory / name;
    }

private:
    std::filesystem::path testDirectory;
};

TEST_F(FileHandleTest, OpenCreatesANewFileAndOpensAnExistingFile) {
    const auto path = FilePath("created.txt");

    auto first = FileHandle::Open(path.c_str());
    ASSERT_TRUE(first.IsOk());
    EXPECT_TRUE(std::filesystem::exists(path));

    auto second = FileHandle::Open(path.c_str());
    EXPECT_TRUE(second.IsOk());
}

TEST_F(FileHandleTest, WriteThenReadReturnsTheSameBytes) {
    const auto path = FilePath("read-write.txt");
    auto opened = FileHandle::Open(path.c_str());
    ASSERT_TRUE(opened.IsOk());

    const std::array<std::byte, 5> writtenBytes{
        static_cast<std::byte>('p'),
        static_cast<std::byte>('i'),
        static_cast<std::byte>('z'),
        static_cast<std::byte>('z'),
        static_cast<std::byte>('a'),
    };

    auto written = opened.value().Write(0, writtenBytes.data(), writtenBytes.size());
    ASSERT_TRUE(written.IsOk());
    EXPECT_EQ(written.value(), writtenBytes.size());

    std::array<std::byte, writtenBytes.size()> readBytes{};
    auto read = opened.value().Read(0, readBytes.data(), readBytes.size());
    ASSERT_TRUE(read.IsOk());
    EXPECT_EQ(read.value(), writtenBytes.size());
    EXPECT_EQ(readBytes, writtenBytes);
}

TEST_F(FileHandleTest, ReadReturnsOnlyTheBytesAvailableBeforeEndOfFile) {
    const auto path = FilePath("short-read.txt");
    auto opened = FileHandle::Open(path.c_str());
    ASSERT_TRUE(opened.IsOk());

    const std::array<std::byte, 3> writtenBytes{
        static_cast<std::byte>('p'),
        static_cast<std::byte>('i'),
        static_cast<std::byte>('e'),
    };
    ASSERT_TRUE(opened.value().Write(0, writtenBytes.data(), writtenBytes.size()).IsOk());

    std::array<std::byte, 10> readBytes{};
    auto read = opened.value().Read(0, readBytes.data(), readBytes.size());
    ASSERT_TRUE(read.IsOk());
    EXPECT_EQ(read.value(), writtenBytes.size());
    EXPECT_EQ(readBytes[0], writtenBytes[0]);
    EXPECT_EQ(readBytes[1], writtenBytes[1]);
    EXPECT_EQ(readBytes[2], writtenBytes[2]);
}

TEST_F(FileHandleTest, ReadAndWriteRejectNullBuffersWithNonZeroLengths) {
    const auto path = FilePath("invalid-buffer.txt");
    auto opened = FileHandle::Open(path.c_str());
    ASSERT_TRUE(opened.IsOk());

    auto read = opened.value().Read(0, nullptr, 1);
    EXPECT_TRUE(!read.IsOk());
    EXPECT_EQ(read.error(), FileError::InvalidBuffer);

    auto written = opened.value().Write(0, nullptr, 1);
    EXPECT_TRUE(!written.IsOk());
    EXPECT_EQ(written.error(), FileError::InvalidBuffer);
}

TEST_F(FileHandleTest, DestructorClosesItsDescriptor) {
    const auto path = FilePath("close-on-destruction.txt");
    const auto descriptorsBefore = OpenDescriptorCount();

    // Create local scope so the destructor is called at the end automatically
    {
        auto result = FileHandle::Open(path.c_str());
        ASSERT_TRUE(result.IsOk());
        EXPECT_EQ(OpenDescriptorCount(), descriptorsBefore + 1);
    }

    EXPECT_EQ(OpenDescriptorCount(), descriptorsBefore);
}

TEST_F(FileHandleTest, MoveConstructorTransfersOwnership) {
    const auto path = FilePath("move-constructor.txt");
    const auto descriptorsBefore = OpenDescriptorCount();

    {
        auto result = FileHandle::Open(path.c_str());
        ASSERT_TRUE(result.IsOk());

        FileHandle destination(std::move(result.value()));
        EXPECT_EQ(OpenDescriptorCount(), descriptorsBefore + 1);
    }

    EXPECT_EQ(OpenDescriptorCount(), descriptorsBefore);
}

TEST_F(FileHandleTest, MoveAssignmentClosesTheOldDescriptorAndTransfersOwnership) {
    const auto destinationPath = FilePath("move-assignment-destination.txt");
    const auto sourcePath = FilePath("move-assignment-source.txt");
    const auto descriptorsBefore = OpenDescriptorCount();

    {
        auto destinationResult = FileHandle::Open(destinationPath.c_str());
        auto sourceResult = FileHandle::Open(sourcePath.c_str());
        ASSERT_TRUE(destinationResult.IsOk());
        ASSERT_TRUE(sourceResult.IsOk());
        ASSERT_EQ(OpenDescriptorCount(), descriptorsBefore + 2);

        FileHandle destination(std::move(destinationResult.value()));
        destination = std::move(sourceResult.value());

        // The destination's old FD was closed; it now owns the source FD.
        EXPECT_EQ(OpenDescriptorCount(), descriptorsBefore + 1);
    }

    EXPECT_EQ(OpenDescriptorCount(), descriptorsBefore);
}

}  // namespace
