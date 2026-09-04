#include "Result.hpp"
#include "FileHandle.hpp"

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
    
    // Create the file
    auto opened = FileHandle::Open(filePath.c_str());
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
}