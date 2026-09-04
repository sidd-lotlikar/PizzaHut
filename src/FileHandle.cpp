#include "FileHandle.hpp"

#include <fcntl.h>
#include <unistd.h>

Result<FileHandle, FileError> FileHandle::Open(const char* path) {
    // Open the file
    // O_CREAT - Permission to open a new file
    // O_RDWR - Permission to read and write to the file
    // 0644 - Open the file with 644 permission so the owner can read and write to the file
    int fd = ::open(path, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        return Result<FileHandle, FileError>::Err(FileError::OpenError);
    }

    return Result<FileHandle, FileError>::Ok(FileHandle(fd));
}

Result<std::size_t, FileError> FileHandle::Read(std::uint64_t offset, std::byte* buffer, std::size_t length) {
    if (buffer == nullptr && length > 0) {
        return Result<std::size_t, FileError>::Err(FileError::InvalidBuffer);
    }
    if (buffer == nullptr && length == 0) {
        return Result<std::size_t, FileError>::Ok(0);
    }
    // ssize_t is a Posix specifc type. 
    ssize_t bytesRead = pread(mDescriptor, buffer, length, offset);
    
    if (bytesRead == -1) {
        return Result<size_t, FileError>::Err(FileError::ReadError);
    }
    return Result<size_t, FileError>::Ok(bytesRead);
}

Result<std::size_t, FileError> FileHandle::Write(std::uint64_t offset, const std::byte* buffer, std::size_t length) {
    if (buffer == nullptr && length > 0) {
        return Result<std::size_t, FileError>::Err(FileError::InvalidBuffer);
    }
    if (buffer == nullptr && length == 0) {
        return Result<std::size_t, FileError>::Ok(0);
    }

    ssize_t bytesWrite = pwrite(mDescriptor, buffer, length, offset);
    if (bytesWrite == -1) {
        return Result<std::size_t, FileError>::Err(FileError::WriteError);
    }
    
    return Result<std::size_t, FileError>::Ok(bytesWrite);
}

bool FileHandle::Close() noexcept {
    if(mDescriptor != -1 && ::close(mDescriptor) == -1) {
        mDescriptor = -1;
        return false;
    }
    mDescriptor = -1;
    return true;
}

FileHandle::FileHandle(int fd) : mDescriptor(fd) {}

FileHandle::~FileHandle() {
    Close();
}

FileHandle::FileHandle(FileHandle&& other) noexcept : mDescriptor(other.mDescriptor) {
    other.mDescriptor = -1;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    this->Close();
    this->mDescriptor = other.mDescriptor;
    other.mDescriptor = -1;
    return *this;
}
