#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

enum class LogLevel { NONE = 0, ERROR, WARN, INFO, DEBUG };

class LogClass {
  private:
    template <typename T> void print(const T str, const char level[]);

    UartClass *uart;
    LogLevel log_level;

  public:
    LogClass(UartClass *uart);

    void setOutputUart(UartClass *uart);

    void setLogLevel(const LogLevel log_level);
    bool setLogLevelStr(const char *log_level);

    void begin(const unsigned long baud_rate);
    void end();

    void error(const char str[]);
    void error(const String str);
    void errorf(const char *format, ...);

    void warn(const char str[]);
    void warn(const String str);
    void warnf(const char *format, ...);

    void info(const char str[]);
    void info(const String str);
    void infof(const char *format, ...);

    void debug(const char str[]);
    void debug(const String str);
    void debugf(const char *format, ...);

    void raw(const char str[]);
    void raw(const String str);
    void rawf(const char *format, ...);
};

extern LogClass Log;

#endif
