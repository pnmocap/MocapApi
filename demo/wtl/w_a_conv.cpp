#include "stdafx.h"
#include "w_a_conv.h"
#include <system_error>
std::wstring AToW(const char* str)
{
    std::wstring dstr;
    int l = ::MultiByteToWideChar(
        CP_ACP, 0, str, -1, NULL, 0);
    if (l > 0) {
        dstr.resize(l);
        if ((l = ::MultiByteToWideChar(
            CP_ACP, 0, str, -1, dstr.data(), dstr.size()))>0) {
            dstr.resize(l);
            while (!dstr.empty() && dstr.back() == 0) {
                dstr.pop_back();
            }
        }
    }
    return std::move(dstr);
}

std::wstring Utf8ToW(const char* str)
{
    std::wstring dstr;
    int l = ::MultiByteToWideChar(
        CP_UTF8, 0, str, -1, NULL, 0);
    if (l > 0) {
        dstr.resize(l);
        if ((l = ::MultiByteToWideChar(
            CP_UTF8, 0, str, -1, dstr.data(), dstr.size())) > 0) {
            dstr.resize(l);
            while (!dstr.empty() 
                && dstr.back() == 0) {
                dstr.pop_back();
            }
        }
    }
    return std::move(dstr);
}

std::string WToUtf8(LPCWSTR wcs)
{
    int l = ::WideCharToMultiByte(CP_UTF8,
        0,
        wcs,
        -1,
        NULL,
        0,
        NULL,
        NULL);
    if (l > 0) {
        std::string r;
        r.resize(l);
        if (l = ::WideCharToMultiByte(CP_UTF8,
            0,
            wcs, -1,
            &r[0], static_cast<int>(r.size()),
            NULL,
            NULL)) {
            r.resize(l);
            return std::move(r);
        }
        else {
            throw std::system_error(std::error_code(::GetLastError(), std::system_category()));
        }
    }
    else {
        throw std::system_error(std::error_code(::GetLastError(), std::system_category()));
    }
}