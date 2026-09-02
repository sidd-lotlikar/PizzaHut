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

    /**
     * Returns the stored reference to the successful value
     */
    T& value() {
        return std::get<0>(mData);
    }
    
    E& error() {
        return std::get<1>(mData);
    }

private:
    /**
     * Tags distinguish Ok construction from Err construction,
     * even when T and E are the same type.
     */
    struct OkTag {};
    struct ErrTag {};

    Result(OkTag, T val) : mData(std::in_place_index<0>, val) {}
    Result(ErrTag, E err) : mData(std::in_place_index<1>, err) {}

    /**
     * This will contain EITHER the ok object, or the error object.
     * But never both, never none.
     */
    std::variant<T, E> mData;
};