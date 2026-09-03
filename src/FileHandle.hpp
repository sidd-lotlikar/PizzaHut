#pragma once

#include "Result.hpp"

#include <cstddef>

enum class FileError {
    OpenError,
    ReadError,
    WriteError,
};

class FileHandle {
public:
    ~FileHandle() noexcept;

    FileHandle (const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle (FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    static Result<FileHandle, FileError> Open(const char* path);

    Result<std::size_t, FileError> Read(std::size_t offset, std::byte* buffer, std::size_t length);
    Result<std::size_t, FileError> Write(std::size_t offset, std::byte* buffer, std::size_t length);
private:
    explicit FileHandle(int fd);
    bool Close() noexcept;
    int mDescriptor = -1;
};