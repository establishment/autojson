#ifndef AUTOJSON_ERROR_HPP
#define AUTOJSON_ERROR_HPP
#include <string>

namespace AutoJson {

/// Basic Error handling
/// TODO make this better
void JsonError(const std::string& message);

}  // namespace AutoJson

#endif
