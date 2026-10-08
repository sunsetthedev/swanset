#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H

#include <stddef.h>
#include <string.h>
typedef struct {
  const char *color_code;
  const char *color_name;
} TerminalColor;

static const TerminalColor terminal_colors[] = {
    {"\x1b[30m", "Black"},
    {"\x1b[31m", "Red"},
    {"\x1b[32m", "Green"},
    {"\x1b[33m", "Yellow"},
    {"\x1b[34m", "Blue"},
    {"\x1b[35m", "Magenta"},
    {"\x1b[36m", "Cyan"},
    {"\x1b[37m", "White"},
    {"\x1b[90m", "Bright Black"},
    {"\x1b[91m", "Bright Red"},
    {"\x1b[92m", "Bright Green"},
    {"\x1b[93m", "Bright Yellow"},
    {"\x1b[94m", "Bright Blue"},
    {"\x1b[95m", "Bright Magenta"},
    {"\x1b[96m", "Bright Cyan"},
    {"\x1b[97m", "Bright White"}};

extern TerminalColor get_terminal_color(const char *color_name);

/* Reset */
#define CLR_RESET "\x1b[0m"

/* Regular Colors */
/*#define CLR_BLACK "\x1b[30m"
#define CLR_RED "\x1b[31m"
#define CLR_GREEN "\x1b[32m"
#define CLR_YELLOW "\x1b[33m"
#define CLR_BLUE "\x1b[34m"
#define CLR_MAGENTA "\x1b[35m"
#define CLR_CYAN "\x1b[36m"
#define CLR_WHITE "\x1b[37m"
#define CLR_BRIGHT_BLACK "\x1b[90m"
#define CLR_BRIGHT_RED "\x1b[91m"
#define CLR_BRIGHT_GREEN "\x1b[92m"
#define CLR_BRIGHT_YELLOW "\x1b[93m"
#define CLR_BRIGHT_BLUE "\x1b[94m"
#define CLR_BRIGHT_MAGENTA "\x1b[95m"
#define CLR_BRIGHT_CYAN "\x1b[96m"
#define CLR_BRIGHT_WHITE "\x1b[97m"
Background Color
#define BG_BLACK "\x1b[40m"
#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"
#define BG_BRIGHT_BLACK "\x1b[100m"
#define BG_BRIGHT_RED "\x1b[101m"
#define BG_BRIGHT_GREEN "\x1b[102m"
#define BG_BRIGHT_YELLOW "\x1b[103m"
#define BG_BRIGHT_BLUE "\x1b[104m"
#define BG_BRIGHT_MAGENTA "\x1b[105m"
#define BG_BRIGHT_CYAN "\x1b[106m"
#define BG_BRIGHT_WHITE "\x1b[107m"
*/
/* Text Styles */
#define TXT_BOLD "\x1b[1m"
#define TXT_DIM "\x1b[2m"
#define TXT_ITALIC "\x1b[3m"
#define TXT_UNDERLINE "\x1b[4m"
#define TXT_BLINK "\x1b[5m"
#define TXT_REVERSE "\x1b[7m"
#define TXT_HIDDEN "\x1b[8m"
#define TXT_STRIKE "\x1b[9m"

/* Style Reset */
#define TXT_NORMAL "\x1b[22m"
#define TXT_NO_ITALIC "\x1b[23m"
#define TXT_NO_UNDERLINE "\x1b[24m"
#define TXT_NO_BLINK "\x1b[25m"
#define TXT_NO_REVERSE "\x1b[27m"
#define TXT_NO_HIDDEN "\x1b[28m"
#define TXT_NO_STRIKE "\x1b[29m"

#endif
