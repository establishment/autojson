#include "Error.hpp"
#include "JSON.hpp"

#include <iostream>
#include <exception>

namespace autojson {

namespace internal {

// Basic Error handling
// TODO make this better
inline void JSONErrorDefault(const std::string& message) {
    std::cerr << message << '\n';
    std::cerr.flush();
#ifndef NDEBUG
    std::terminate();
#endif
}

}

FatalErrorCallback& GetFatalErrorCallback() {
    static FatalErrorCallback errorCallback(internal::JSONErrorDefault);
    return errorCallback;
}

void SetFatalErrorCallback(FatalErrorCallback callback) {
    GetFatalErrorCallback() = callback;
}

void JSONError(const std::string& message) {
    GetFatalErrorCallback()(message);
}

}  // namespace AutoJson
