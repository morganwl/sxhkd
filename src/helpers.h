/* Copyright (c) 2013, Bastien Dejean
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SXHKD_HELPERS_H
#define SXHKD_HELPERS_H

#include <stdbool.h>

#define LENGTH(x)         (sizeof(x) / sizeof(*x))
#define MAXLEN            256

#ifdef DEBUG
#  define PUTS(x)         puts(x)
#  define PRINTF(x,...)   printf(x, __VA_ARGS__)
#else
#  define PUTS(x)         ((void)0)
#  define PRINTF(x,...)   ((void)0)
#endif

/**
 * @brief Print formatted args to stderr.
 * @param fmt Printf format string.
 * @param ... Parameters to be printed.
 */
void warn(char *fmt, ...);
/**
 * @brief Print formatted args to stderr and exit with failure.
 * @param fmt Printf format string.
 * @param ... Parameters to be printed.
 */
__attribute__((noreturn))
void err(char *fmt, ...);
/**
 * @brief Executes command and redirects output.
 * @param cmd Command to be executed.
 *
 * Executes a command in a new session. This will keep the process from
 * being terminated if sxhkd exits. If a redirect file has been opened,
 * stdout and stderr are redirected to it.
 */
void execute(char *cmd[]);
/**
 * @brief Spawns command as a new process.
 * @param cmd Command to be executed.
 * @param sync Flag to wait for child process to exit.
 */
void spawn(char *cmd[], bool sync);
/**
 * @brief Run command in shell.
 * @param command Command to pass to shell.
 * @param sync Flag to run synchronously.
 *
 * Passes the string command using `shell -c <command>`. Defaults to
 * executing asynchronously, but will run synchronously if the sync flag
 * is present.
 */
void run(char *command, bool sync);
/**
 * @brief Strips non-graph characters from left of string.
 * @param s String to be stripped.
 * @return Pointer to first printable, non-space character, or NULL if no
 * such character exists.
 */
char *lgraph(char *s);
/**
 * @brief Strips non-graph characters from right of string.
 * @param s String to be stripped.
 * @return Pointer to last printable, non-space character, or NULL if no
 * such character exists.
 */
char *rgraph(char *s);

#endif
