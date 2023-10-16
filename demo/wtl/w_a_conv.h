#pragma once
#include <string>

std::wstring AToW(const char* str);

std::wstring Utf8ToW(const char* str);

std::string WToUtf8(LPCWSTR str);