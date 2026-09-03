#include "FileHandle.hpp"

#include <gtest/gtest.h>

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
