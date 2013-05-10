#ifndef BASE64_H
#define BASE64_H

#include <string>

namespace elog{

std::string base64_encode(const std::string& s);
std::string base64_decode(const std::string& s);

}

#endif
