#pragma once

#include "Result.hpp"

#include <cstddef>
#include <cstdint>
#include <fcntl.h>

enum class FileError {
    OpenError,
    ReadError,
    WriteError,
    InvalidBuffer
};

enum class FileAccess {
    ReadOnly,
    ReadWrite
};

enum class FileCreation {
    OpenExisting,
    OpenNew
};

struct FileOpenOptions {
    FileAccess access = FileAccess::ReadOnly;
    FileCreation creation = FileCreation::OpenExisting;
    mode_t permissions = 0644; // Used only when creating a file.
};

class FileHandle {
public:
    // Rule of 5
    ~FileHandle() noexcept;

    FileHandle (const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle (FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    // FileHandle Factory Method
    static Result<FileHandle, FileError> Open(const char* path, const FileOpenOptions& options = {});

    Result<std::size_t, FileError> Read(std::uint64_t offset, std::byte* buffer, std::size_t length);
    Result<std::size_t, FileError> Write(std::uint64_t offset, const std::byte* buffer, std::size_t length);

    bool Close() noexcept;
private:
    explicit FileHandle(int fd);
    int mDescriptor = -1;
};