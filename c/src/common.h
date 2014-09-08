#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <vector>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

typedef std::vector<std::string> EntitySet;

#endif  // COMMON_H_
