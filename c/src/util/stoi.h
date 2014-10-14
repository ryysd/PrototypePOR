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


std::string trim(const std::string& string, const char* trimCharacterList = " \t\v\r\n") {
  std::string result;
  std::string::size_type left = string.find_first_not_of(trimCharacterList);

  if (left != std::string::npos) {
    std::string::size_type right = string.find_last_not_of(trimCharacterList);
    result = string.substr(left, right - left + 1);
  }

  return result;
}

#endif  // UTIL_STOI_H_
