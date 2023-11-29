#include <algorithm>
#include <cctype>

#include "util.h"

std::string util::lowercase(const std::string& s)
{
  std::string out(s);
  std::transform(s.begin(), s.end(), out.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return out;
}
