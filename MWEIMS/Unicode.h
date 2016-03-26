#pragma once
#include <iostream>
#include <string>
#include "Limonp\Logger.hpp"
#include "Limonp\StringUtil.hpp"
#include <Windows.h>

typedef std::u16string Unicode;

BOOL MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize) {
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, NULL, 0);
	if (dwSize < dwMinSize) {
		return FALSE;
	}
	MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);
	return TRUE;
}
Unicode gbk2Unicode(std::string s) {
	wchar_t *ws = new wchar_t[s.length() + 1];
	if (!MByteToWChar(s.c_str(), ws, s.length() + 1)) {
		LogFatal("converting %s to unicode failed.", s.c_str());
	}
	Unicode ret((Unicode::value_type*)ws);
	delete[] ws;
	return ret;
}

BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize) {
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (dwSize < dwMinSize) {
		return FALSE;
	}
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
	return TRUE;
}
std::string Unicode2gbk(Unicode unicode) {
	char *s = new char[2*(unicode.length()) + 1];
	if (!WCharToMByte((wchar_t*)unicode.c_str(), s, 2 * (unicode.length()) + 1)) {
		LogFatal("converting to unicode error.");
	}
	std::string ret(s);
	delete[] s;
	return ret;
}

std::ostream& operator << (std::ostream& os, const Unicode& unicode) {
	os << "u\"" + Unicode2gbk(unicode.c_str()) + "\"";
	return os;
}

std::ofstream& operator << (std::ofstream& ofs, const Unicode& unicode) {
	ofs << Unicode2gbk(unicode.c_str());
	return ofs;
}