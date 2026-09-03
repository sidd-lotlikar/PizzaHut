#include "Result.hpp"
#include "FileHandle.hpp"

#include <iostream>
#include <filesystem>

int main() {
    const std::filesystem::path directoryPath = std::filesystem::temp_directory_path() / "PizzaHut";
    // Create the directory
    if (!std::filesystem::exists(directoryPath) && !std::filesystem::is_directory(directoryPath)) {
        std::filesystem::create_directory(directoryPath);
    }
    // Create file path and test open.
    const std::filesystem::path filePath = directoryPath / "a.txt";
    auto fileHandle = FileHandle::Open(filePath.c_str());
    if (fileHandle.IsOk()) {
        std::cout << "successfully created a file!" << std::endl;
    } else {
        std::cout << "Open Error" << std::endl;
    }
}