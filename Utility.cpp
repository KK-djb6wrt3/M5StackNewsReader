#include "Utility.h"

const char* duplicateString(const String& str) {
	int bufSz = str.length() + 1;
	char* pStr = new char[bufSz];
	memcpy(pStr, str.c_str(), bufSz);
	return pStr;
}
