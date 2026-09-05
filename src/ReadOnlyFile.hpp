#include "Result.hpp"
#include "FileHandle.hpp"

class ReadOnlyFile {
public:
    static Result<ReadOnlyFile, FileError> Open(const char* path);

    Result<std::size_t, FileError> Read(std::uint64_t offset, std::byte* buffer, size_t length); 
private:
    explicit ReadOnlyFile(FileHandle&& file);
    FileHandle mReadFile;
};