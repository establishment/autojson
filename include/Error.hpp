#ifndef AUTOJSON_ERROR_HPP
#define AUTOJSON_ERROR_HPP

#include <string>

namespace autojson {

void JSONError(const std::string& message);

}  // namespace AutoJson

#ifndef autojsonuselib
#include "autojson_src/Error.cpp"
#endif

#endif // AUTOJSON_ERROR_HPP
