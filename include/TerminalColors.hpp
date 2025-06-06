#ifndef __TERMINAL_COLORS_HPP__
#define __TERMINAL_COLORS_HPP__

#include <string>

namespace TerminalColor {

    // Foreground (text) colors
    inline constexpr const char* black_fg   = "\033[30m";
    inline constexpr const char* red_fg     = "\033[31m";
    inline constexpr const char* green_fg   = "\033[32m";
    inline constexpr const char* yellow_fg  = "\033[33m";
    inline constexpr const char* blue_fg    = "\033[34m";

    // Background colors
    // Standart background renkleri
    inline constexpr const char* black_bg        = "\033[40m";
    inline constexpr const char* red_bg          = "\033[41m";
    inline constexpr const char* green_bg        = "\033[42m";
    inline constexpr const char* yellow_bg       = "\033[43m";
    inline constexpr const char* blue_bg         = "\033[44m";
    inline constexpr const char* magenta_bg      = "\033[45m";
    inline constexpr const char* cyan_bg         = "\033[46m";
    inline constexpr const char* white_bg        = "\033[47m";

    // Parlak (bright) background renkleri (256-color destekli terminallerde)
    inline constexpr const char* bright_black_bg   = "\033[100m"; // griye yakın
    inline constexpr const char* bright_red_bg     = "\033[101m";
    inline constexpr const char* bright_green_bg   = "\033[102m";
    inline constexpr const char* bright_yellow_bg  = "\033[103m";
    inline constexpr const char* bright_blue_bg    = "\033[104m";
    inline constexpr const char* bright_magenta_bg = "\033[105m";
    inline constexpr const char* bright_cyan_bg    = "\033[106m";
    inline constexpr const char* bright_white_bg   = "\033[107m";


    // Styles
    inline constexpr const char* bold       = "\033[1m";
    inline constexpr const char* underline  = "\033[4m";
    inline constexpr const char* reset      = "\033[0m";

    // Utility: Combine foreground + background + style
    inline std::string colorize(const char* fg = nullptr,
                                const char* bg = nullptr,
                                const char* style = nullptr)
    {
        std::string result;
        if (style) result += style;
        if (fg)    result += fg;
        if (bg)    result += bg;
        return result;
    }
}

#endif // __TERMINAL_COLORS_HPP__
