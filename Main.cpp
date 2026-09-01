#include "src/Result.hpp"

#include <iostream>


void assert(bool op) {
    if (op)
    {
        std::cout << "Operation succeeded!" << std::endl;
    }
    else
    {
        std::cout << "Operation Failed!" << std::endl;
    }
}

int main() {
    auto success = Result<std::string, std::string>::Ok("success");

    auto failure = Result<std::string, std::string>::Err("failed");

    assert(success.isOk());
    assert(!success.isErr());

    assert(failure.isErr());
    assert(!failure.isOk());
    
    return 0;
}