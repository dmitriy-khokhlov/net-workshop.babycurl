#ifndef LOG_H
#define LOG_H 1

enum log_level
{
    LL_DEBUG = 0,
    LL_INFO = 1,
    LL_WARNING = 2,
    LL_ERROR = 3
};

/*
 * Set internal exit_on_error flag.
 */
void log_set_exit_on_error(int value);

/*
 * Set internal min_log_level variable value.
 */
void log_set_min_log_level(enum log_level value);

/*
 * Log a debug message. Uses the same formatting style as printf family.
 */
void log_debug(const char *func, const char *format, ...);

/*
 * Log an info message. Uses the same formatting style as printf family.
 */
void log_info(const char *func, const char *format, ...);

/*
 * Log an info message based on the global variable errno.
 * Uses the same formatting style as printf family.
 */
void log_sys_info(const char *func, const char *format, ...);

/*
 * Log a warning message. Uses the same formatting style as printf family.
 */
void log_warning(const char *func, const char *format, ...);

/*
 * Log a warning message based on the global variable errno.
 * Uses the same formatting style as printf family.
 */
void log_sys_warning(const char *func, const char *format, ...);

/*
 * Log an error message. Uses the same formatting style as printf family.
 */
void log_error(const char *func, const char *format, ...);

/*
 * Log a getaddrinfo (GAI) family error message based on errcode argument.
 * Uses the same formatting style as printf family.
 */
void log_gai_error(const char *func, int errcode, const char *format, ...);

/*
 * Log a system error message based on the global variable errno.
 * Uses the same formatting style as printf family.
 */
void log_sys_error(const char *func, const char *format, ...);

#endif /* log.h */
