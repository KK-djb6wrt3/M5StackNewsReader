#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <wstring.h>

#define SAFE_DELETE(ptr) { if((ptr) != NULL) { delete (ptr); (ptr) = NULL; } }
#define SAFE_DELARY(ptr) { if((ptr) != NULL) { delete[] (ptr); (ptr) = NULL; } }

const char* duplicateString(const String& str);

#endif //_UTILITY_H_
