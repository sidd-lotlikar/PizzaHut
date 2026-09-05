#include "Result.hpp"
#include "FileHandle.hpp"
#include "ReadOnlyFile.hpp"

#include <iostream>
#include <filesystem>
#include <vector>

int main() {
    const std::filesystem::path directoryPath = std::filesystem::temp_directory_path() / "PizzaHut";
    // Create the directory
    if (!std::filesystem::exists(directoryPath) && !std::filesystem::is_directory(directoryPath)) {
        std::filesystem::create_directory(directoryPath);
    }

    // Create file path and test open.
    const std::filesystem::path filePath = directoryPath / "a.txt";
    FileOpenOptions options;
    options.access = FileAccess::ReadWrite;
    options.creation = FileCreation::OpenNew;
    
    // Create the file
    auto opened = FileHandle::Open(filePath.c_str(), options);
    if (!opened.IsOk()) {
        std::cerr << "opening the file failed\n";
    }

    auto& file = opened.value();
    const std::string message = "Veggie Lover";

    auto written = file.Write(
        0, 
        reinterpret_cast<const std::byte*>(message.data()), 
        message.size()
    );

    if (!written.IsOk()) {
        std::cerr << "Write failed\n";
    }

    std::vector<std::byte> buffer(message.size());
    auto read = file.Read(0, buffer.data(), buffer.size());
    if (!read.IsOk()) {
        std::cerr << "Read failed\n";
    }

    std::string result(reinterpret_cast<const char*>(buffer.data()), read.value());
    std::cout << "Read: " << result << "\n";


    std::vector<std::byte> buffer2(message.size());
    // Create ReadOnlyFile handle
    auto readOnlyOpenedFile = ReadOnlyFile::Open(filePath.c_str());
    if (!readOnlyOpenedFile.IsOk()) {
        std::cerr << "Open Failed #2\n";
    }

    auto& readOnlyFile = readOnlyOpenedFile.value();
    auto readOnlyResult = readOnlyFile.Read(0, buffer2.data(), buffer2.size());
    if (!readOnlyResult.IsOk()) {
        std::cerr << "ReadOnly Read Failed\n";
    }

    std::string readOnlyString(reinterpret_cast<const char*>(buffer2.data()), readOnlyResult.value());
    std::cout << "ReadOnly: " << readOnlyString << "\n";

}