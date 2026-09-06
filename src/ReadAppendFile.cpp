#pragma once

#include "ReadAppendFile.hpp"

Result<ReadAppendFile, FileError> ReadAppendFile::Open(const char* path) {
    FileOpenOptions options;
    options.access = FileAccess::ReadWrite;
    options.creation = FileCreation::OpenExisting;

    auto fileResult = FileHandle::Open(path, options);
    if (!fileResult.IsOk()) {
        return Result<ReadAppendFile, FileError>::Err(fileResult.error());
    }

    FileHandle& file = fileResult.value();

    auto sizeResult = file.Size();
    if (!sizeResult.IsOk()) {
        return Result<ReadAppendFile, FileError>::Err(sizeResult.error());
    }

    std::uint64_t initialOffset = static_cast<std::uint64_t>(sizeResult.value()); 

    return Result<ReadAppendFile, FileError>::Ok(ReadAppendFile(std::move(file), initialOffset));
}

Result<std::size_t, FileError> ReadAppendFile::Read(std::uint64_t offset, std::byte* buffer, size_t length) {
    return mFile.Read(offset, buffer, length);
}

Result<std::size_t, FileError> ReadAppendFile::Append(const std::byte* buffer, size_t length) {
    auto write = mFile.Write(mOffset, buffer, length);
    if (!write.IsOk()) {
        return Result<std::size_t, FileError>::Err(FileError::WriteError);
    }

    mOffset += length;

    return Result<std::size_t, FileError>::Ok(write.value());
}

ReadAppendFile::ReadAppendFile(FileHandle&& file, std::uint64_t offset) : mFile(std::move(file)), mOffset(offset) {

}