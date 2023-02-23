#pragma once

#include <string>
#include <locale>
#include <codecvt>


//--------------------------------------------------------------------------------------------------
inline std::string wstring_to_string(const std::wstring &toConvert)
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    static std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes(toConvert);
}

//--------------------------------------------------------------------------------------------------
inline std::wstring string_to_wstring(const std::string &toConvert)
{
    using convert_type = std::codecvt_utf8<wchar_t>;
    static std::wstring_convert<convert_type, wchar_t> converter;
    return converter.from_bytes(toConvert);
}
