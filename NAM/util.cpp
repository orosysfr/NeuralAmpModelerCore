#include <algorithm>
#include <cctype>

#include "util.h"

#ifndef _WIN32

#else
  #pragma warning(push)
  #pragma warning(disable : 4244)
  #pragma warning(disable : 4267)


#endif


std::string util::lowercase(const std::string& s)
{
  std::string out(s);
  std::transform(s.begin(), s.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
  return out;
}


#ifndef _WIN32


#else

#pragma warning(pop)
#endif