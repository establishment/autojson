#include "Error.hpp"

#include <iostream>

namespace autjson {

// Basic Error handling
// TODO make this better
void JSONError(const std::string& message)
{
    std::cerr << message << '\n';
}

}  // namespace AutoJson