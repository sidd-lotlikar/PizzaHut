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

    assertTrue(success.isOk());
    assertTrue(!success.isErr());

    assertTrue(failure.isErr());
    assertTrue(!failure.isOk());

    assertEqInt(success.value(), 1);
    assertEqInt(failure.error(), -1);

    std::cout << "Finished testing value and error" << std::endl;
    return 0;
}