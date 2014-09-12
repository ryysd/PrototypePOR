#ifndef POR_MODULE_COMMON_H_
#define POR_MODULE_COMMON_H_

#include <string>
#include <vector>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

typedef std::vector<std::string> EntitySet;

#endif  // POR_MODULE_COMMON_H_
