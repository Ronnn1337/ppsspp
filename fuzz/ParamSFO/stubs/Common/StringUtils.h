#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>

inline std::string StringFromFormat(const char *format, ...) {
    if (!format)
        return {};

    char stackBuf[256];
    va_list args;
    va_start(args, format);
    int needed = vsnprintf(stackBuf, sizeof(stackBuf), format, args);
    va_end(args);

    if (needed < 0)
        return {};

    if (static_cast<size_t>(needed) < sizeof(stackBuf))
        return std::string(stackBuf, stackBuf + needed);

    std::string out;
    out.resize(static_cast<size_t>(needed));
    va_start(args, format);
    vsnprintf(out.data(), out.size() + 1, format, args);
    va_end(args);
    return out;
}
