#ifndef UTIL_STOI_H_
#define UTIL_STOI_H_

#include <string>

int stoi(const char* string) {
  int result = 0;
  for (int i = 0; string[i] != '\0'; i++) {
    if (isdigit(string[i])) result = result * 10 + (string[i] - '0');
  }

  return result;
}

#endif  // UTIL_STOI_H_
