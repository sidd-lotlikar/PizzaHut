#pragma once

#include "Result.hpp"

#include <cstddef>
#include <cstdint>

enum class FileError {
    OpenError,
    ReadError,
    WriteError,
    InvalidBuffer
};

class FileHandle {
public:
    ~FileHandle() noexcept;

    FileHandle (const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle (FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    static Result<FileHandle, FileError> Open(const char* path);

    Result<std::size_t, FileError> Read(std::uint64_t offset, std::byte* buffer, std::size_t length);
    Result<std::size_t, FileError> Write(std::uint64_t offset, const std::byte* buffer, std::size_t length);
private:
    explicit FileHandle(int fd);
    bool Close() noexcept;
    int mDescriptor = -1;
};