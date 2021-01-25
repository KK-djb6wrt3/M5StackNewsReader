#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <wstring.h>

#define SAFE_DELETE(ptr) { if((ptr) != NULL) { delete (ptr); (ptr) = NULL; } }
#define SAFE_DELARY(ptr) { if((ptr) != NULL) { delete[] (ptr); (ptr) = NULL; } }

#define COLOR565(r, g, b) (((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)

const char* duplicateString(const String& str);
const char* duplicateString(const char* pStr);

#endif //_UTILITY_H_
