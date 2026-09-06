#include "Result.hpp"
#include "FileHandle.hpp"

#include <cstdint>

class ReadAppendFile {
public:
    static Result<ReadAppendFile, FileError> Open(const char* path);

    Result<std::size_t, FileError> Read(std::uint64_t offset, std::byte* buffer, size_t length);
    Result<std::size_t, FileError> Append(const std::byte* buffer, size_t length);
private:
    explicit ReadAppendFile(FileHandle&& file, std::uint64_t offset);
    FileHandle mFile;
    std::uint64_t mOffset;
};