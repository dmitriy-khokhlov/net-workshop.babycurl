#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include "log.h"

static int exit_on_error = 1;

static enum log_level min_log_level = LL_DEBUG;

static char *log_level_names[] =
{
    "debug",    /* LL_DEBUG */
    "info",     /* LL_INFO */
    "warning",  /* LL_WARNING */
    "error"     /* LL_ERROR */
};

/*
 * Log message of specified level.
 * This function uses the same formatting style as vprintf family.
 * args must be initialized by va_start() before calling this function.
 * This function does not call va_end() for args.
 * The value of args is undefined after this function returns.
 */
static void log_message(enum log_level level, const char *func,
        const char *origin_msg, const char *format, va_list args)
{
    if (level < min_log_level) {
        return;
    }

    fprintf(stderr, "%s: ", program_invocation_name);

    if (func != NULL) {
        fprintf(stderr, "%s(): ", func);
    }

    /* Boundary check for log_level_names[] is intentionally omitted as this
     * function is internal and there is no need to clutter the code. */
    fprintf(stderr, "%s: ", log_level_names[level]);

    vfprintf(stderr, format, args);

    if (origin_msg != NULL) {
        fprintf(stderr, ": %s", origin_msg);
    }

    fprintf(stderr, "\n");
}

/*
 * Log error message and handle exit_on_error flag.
 * This function calls log_message(). See its documentation about
 * format and args.
 */
static void log_error_message(const char *func,
        const char *origin_msg, const char *format, va_list args)
{
    log_message(LL_ERROR, func, origin_msg, format, args);
    if (exit_on_error) {
        exit(EXIT_FAILURE);
    }
}

void log_set_exit_on_error(int value)
{
    exit_on_error = value;
}

void log_set_min_log_level(enum log_level value)
{
    min_log_level = value;
}

void log_debug(const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LL_DEBUG, func, NULL, format, args);
    va_end(args);
}

void log_info(const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LL_INFO, func, NULL, format, args);
    va_end(args);
}

void log_sys_info(const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LL_INFO, func, strerror(errno), format, args);
    va_end(args);
}

void log_warning(const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LL_WARNING, func, NULL, format, args);
    va_end(args);
}

void log_sys_warning(const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LL_WARNING, func, strerror(errno), format, args);
    va_end(args);
}

void log_error(const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_error_message(func, NULL, format, args);
    va_end(args);
}

void log_gai_error(const char *func, int errcode, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_error_message(func,
            errcode == EAI_SYSTEM ? strerror(errno) : gai_strerror(errcode),
            format, args);
    va_end(args);
}

void log_sys_error(const char *func, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_error_message(func, strerror(errno), format, args);
    va_end(args);
}
