#include "Error.hpp"

#include <iostream>
#include <exception>

namespace autojson {

// Basic Error handling
// TODO make this better
void JSONError(const std::string& message) {
    std::cerr << message << '\n';
    std::cerr.flush();
#ifndef NDEBUG
    std::terminate();
#endif
}

}  // namespace AutoJson
