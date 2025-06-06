#ifndef ASSERT_UTILS_HPP
#define ASSERT_UTILS_HPP

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <execinfo.h>  // backtrace, backtrace_symbols
#include <unistd.h>

inline void printBacktrace(int max_frames = 25) {
    std::vector<void*> addrlist(max_frames + 1);

    int addrlen = backtrace(addrlist.data(), addrlist.size());
    if (addrlen == 0) {
        std::cerr << "  <empty stack trace>\n";
        return;
    }

    char** symbol_list = backtrace_symbols(addrlist.data(), addrlen);
    for (int i = 1; i < addrlen; i++) {
        std::cerr << "  #" << i << ": " << symbol_list[i] << "\n";
    }
    free(symbol_list);
}

#ifndef NDEBUG

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "[ASSERTION FAILED]\n" \
                      << "  Condition : " << #cond << "\n" \
                      << "  File      : " << __FILE__ << "\n" \
                      << "  Line      : " << __LINE__ << "\n" \
                      << "  Function  : " << __func__ << "\n"; \
            std::cerr << "  Call Stack:\n"; \
            printBacktrace(); \
            std::abort(); \
        } \
    } while (0)

#define ASSERT_MSG(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "[ASSERTION FAILED]\n" \
                      << "  Condition : " << #cond << "\n" \
                      << "  Message   : " << msg << "\n" \
                      << "  File      : " << __FILE__ << "\n" \
                      << "  Line      : " << __LINE__ << "\n" \
                      << "  Function  : " << __func__ << "\n"; \
            std::cerr << "  Call Stack:\n"; \
            printBacktrace(); \
            std::abort(); \
        } \
    } while (0)

#else
#define ASSERT(cond) ((void)0)
#define ASSERT_MSG(cond, msg) ((void)0)
#endif // NDEBUG

#endif // ASSERT_UTILS_HPP
