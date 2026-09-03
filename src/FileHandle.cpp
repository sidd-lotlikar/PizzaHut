#include "FileHandle.hpp"

#include <fcntl.h>
#include <unistd.h>

Result<FileHandle, FileError> FileHandle::Open(const char* path) {
    // Open the file
    // O_CREAT - Permission to open a new file
    // O_RDWR - Permission to read and write to the file
    // 0644 - A 644 permission so the owner can read and write to the file
    int fd = ::open(path, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        return Result<FileHandle, FileError>::Err(FileError::OpenError);
    }

    return Result<FileHandle, FileError>::Ok(FileHandle(fd));
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
