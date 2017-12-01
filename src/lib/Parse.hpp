#ifndef AUTOJSON_PARSE_HPP
#define AUTOJSON_PARSE_HPP

#include <string>

namespace autojson {

void ParseError(const std::string &message, const char *content_pos);

bool CanSkipWhitespace(const char *content, const std::string &custom_pass="");

void SkipWhitespace(const char *&content, const std::string &custom_pass);

std::string EscapeKeys(const std::string &content);

std::string ParseWord(const char *&content);

std::string ParseString(const char *&content);

} // namespace autojson

#endif // AUTOJSON_PARSE_HPP
