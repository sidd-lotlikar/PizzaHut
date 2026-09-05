#include "ReadOnlyFile.hpp"

Result<ReadOnlyFile, FileError> ReadOnlyFile::Open(const char* path) {
    // Setup options
    FileOpenOptions options;
    options.access = FileAccess::ReadOnly;
    options.creation = FileCreation::OpenExisting;
    // Create a file handle
    auto openFile = FileHandle::Open(path, options);
    if (!openFile.IsOk()) {
        return Result<ReadOnlyFile, FileError>::Err(openFile.error());
    }

    return Result<ReadOnlyFile, FileError>::Ok(ReadOnlyFile(std::move(openFile.value())));
}

Result<std::size_t, FileError> ReadOnlyFile::Read(std::uint64_t offset, std::byte* buffer, size_t length) {
    return mFile.Read(offset, buffer, length);
}

ReadOnlyFile::ReadOnlyFile(FileHandle&& file) : mFile(std::move(file)) {
    
}