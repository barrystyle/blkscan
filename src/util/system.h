#ifndef BITCOIN_LOGGING_H
#define BITCOIN_LOGGING_H

#include <tinyformat.h>

#define LogPrintf(...) LogPrintf_(__func__, __FILE__, __LINE__, __VA_ARGS__)

// Use a macro instead of a function for conditional logging to prevent
// evaluating arguments when logging for the category is not enabled.
#define LogPrint(category, ...)              \
    do {                                     \
        if (LogAcceptCategory((category))) { \
            LogPrintf(__VA_ARGS__);          \
        }                                    \
    } while (0)

template <typename... Args>
static inline void LogPrintf_(const std::string& logging_function, const std::string& source_file, const int source_line, const char* fmt, const Args&... args)
{
}

#endif // BITCOIN_LOGGING_H
