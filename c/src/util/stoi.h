#ifndef UTIL_STOI_H_
#define UTIL_STOI_H_

#include <string>
#include <vector>

int stoi(const char* string) {
  int result = 0;
  for (int i = 0; string[i] != '\0'; i++) {
    if (isdigit(string[i])) result = result * 10 + (string[i] - '0');
  }

  return result;
}

void split(const std::string& str, char delim, std::vector<std::string>* res) {
  size_t current = 0, found;
  while ((found = str.find_first_of(delim, current)) != std::string::npos) {
    res->push_back(std::string(str, current, found - current));
    current = found + 1;
  }
  res->push_back(std::string(str, current, str.size() - current));
}

#endif  // UTIL_STOI_H_
