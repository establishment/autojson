#include "error.hpp"

#include <iostream>

namespace AutoJson {

/// Basic Error handling
/// TODO make this better
void JsonError(const std::string& message)
{
    std::cerr << message << '\n';
}

}  // namespace AutoJson
