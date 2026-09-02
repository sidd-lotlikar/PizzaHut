#include "src/Result.hpp"

#include <iostream>
#include <memory>

void assertTrue(bool op) {
    if (op) {
        std::cout << "Operation succeeded!" << std::endl;
    }
    else {
        std::cout << "Operation Failed!" << std::endl;
    }
}

void assertEqInt(int a, int b) {
    if (a == b) {
        std::cout << "Operation succeeded!" << std::endl;
    }
    else {
        std::cout << "Operation Failed!" << std::endl;
    }
}

int main() {
    auto success = Result<int, int>::Ok(1);
    auto failure = Result<int, int>::Err(-1);

    assertTrue(success.IsOk());
    assertTrue(!success.IsErr());

    assertTrue(failure.IsErr());
    assertTrue(!failure.IsOk());

    assertEqInt(success.value(), 1);
    assertEqInt(failure.error(), -1);

    std::cout << "Finished testing value and error" << std::endl;

    auto testPointer = std::make_unique<int>(0);
    auto successPointer = Result<std::unique_ptr<int>, int>::Ok(std::move(testPointer));
    if (success.IsOk()) {
        std::cout << "success pointer is okay!" << std::endl;
    }
    assertEqInt(*successPointer.value(), 0);
    testPointer = std::make_unique<int>(0);
    auto failurePointer = Result<int, std::unique_ptr<int>>::Err(std::move(testPointer));
    if (failurePointer.IsErr()) {
        std::cout << "failure pointer is an error!" << std::endl;
    }
    assertEqInt(*failurePointer.error(), 0);

    return 0;
}