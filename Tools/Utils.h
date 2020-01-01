#pragma once

#include <string>

inline LPCSTR CStringToChar(CString const& cstr)
{
    return CT2CA(cstr, CP_UTF8);
}

inline CString CharToCString(char const* str)
{
    return CString(CA2CT(str));
}

inline std::string CStringToString(CString const& cstr)
{
    return std::string(CStringToChar(cstr));
}

inline CString StringToCString(std::string const& str)
{
    return CString(std::wstring(str.begin(), str.end()).c_str());
}

inline void StrToLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

inline void StrToUpper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}