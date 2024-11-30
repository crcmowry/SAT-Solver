#ifndef SAT_LOGGER_H
#define SAT_LOGGER_H

#include <iostream>
#include <sstream>
#include <stdexcept>

class Logger {
public:
    static void setLevel(int level) { verbosityLevel = level; }
    static int getLevel() { return verbosityLevel; }

    static void log(int level, const std::string& message) {
        if (level <= verbosityLevel) {
            std::cout << message << std::endl;
        }
    }

    template <typename... Args>
    static void logf(int level, const char* format, Args... args) {
        if (level <= verbosityLevel) {
            std::ostringstream oss;
            logFormatted(oss, format, args...);
            std::cout << oss.str() << std::endl;
        }
    }

private:
    // Define the static member variable as inline
    inline static int verbosityLevel = 0;

    template <typename T, typename... Args>
    static void logFormatted(std::ostringstream& oss, const char* format, T value, Args... args) {
        while (*format) {
            if (*format == '%' && *(++format) != '%') {
                oss << value;
                logFormatted(oss, format, args...);
                return;
            }
            oss << *format++;
        }
    }

    static void logFormatted(std::ostringstream& oss, const char* format) {
        while (*format) {
            if (*format == '%' && *(++format) != '%') {
                throw std::runtime_error("format string mismatch");
            }
            oss << *format++;
        }
    }
};

#endif // SAT_LOGGER_H