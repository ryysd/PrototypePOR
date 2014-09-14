#ifndef UTIL_MACRO_H_
#define UTIL_MACRO_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&); \
void operator=(const TypeName&)

#endif  // UTIL_MACRO_H_
