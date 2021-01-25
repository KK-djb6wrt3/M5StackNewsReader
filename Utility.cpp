#include "Utility.h"

const char* duplicateString(const String& str) {
	int bufSz = str.length() + 1;
	char* pStr = new char[bufSz];
	memcpy(pStr, str.c_str(), bufSz);
	return pStr;
}

const char* duplicateString(const char* pStr) {
	int bufSz = (strlen(pStr) + 1);
	char* pTmp = new char[bufSz];
	memcpy(pTmp, pStr, bufSz);
	return pTmp;
}
