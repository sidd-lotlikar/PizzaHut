#pragma once

#include <variant>

template<typename T, typename E>
class Result {
public:
    /**
     * Constructs a happy result.
     */
    static Result Ok(T val) {
        return Result(OkTag{}, val);
    }

    /**
     * Constructs an error result.
     */
    static Result Err(E err) {
        return Result(ErrTag{}, err);
    }

    /**
     * Checks to see if the result is holding the desired type.
     */
    bool isOk() const {
        return mData.index() == 0;
    }

    /**
     * Checks to see if the result is holding an error. 
     */
    bool isErr() const {
        return mData.index() == 1;
    }
private:
    struct OkTag {};
    struct ErrTag {};

    Result(OkTag, T val) : mData(std::in_place_index<0>, val) {}

    Result(ErrTag, E err) : mData(std::in_place_index<1>, err) {}

    /**
     * This will contain EITHER the desired object, or an error.
     * But never both, never none.
     */
    std::variant<T, E> mData;
};