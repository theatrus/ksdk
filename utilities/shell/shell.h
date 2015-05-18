/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#if !defined(__FSL_SHELL_H__)
#define __FSL_SHELL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "fsl_os_abstraction.h"
#include "shell_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
 
#ifndef SHELL_USE_HISTORY
#define SHELL_USE_HISTORY       0
#endif

#ifndef SHELL_USE_FILE_STREAM
#define SHELL_USE_FILE_STREAM   0
#endif

#ifndef SHELL_AUTO_COMPLETE
#define SHELL_AUTO_COMPLETE     0
#endif

/*! @brief Macro to set console buffer size. */
#ifndef SHELL_BUFFER_SIZE
#define SHELL_BUFFER_SIZE       (128)
#endif

/*! @brief Macro to set max args in command. */
#ifndef SHELL_MAX_ARGS
#define SHELL_MAX_ARGS          (8)
#endif

#ifndef SHELL_HIST_MAX
#define SHELL_HIST_MAX          (10)
#endif

/*! @brief data structure for command table. */
typedef struct shell_cmd_tbl_struct* shell_cmd_tbl_t;
/*! @brief data structure for shell environment. */
typedef struct shell_context_struct* shell_context_t;
/*! @brief user command function prototype. */
typedef int (*cmd_function_t)(struct shell_context_struct *context, int argc, char** argv);
/*! @brief shell IO operation. */
typedef struct shell_ops_struct* shell_ops_t;


/*! @brief A type for the handle special key. */
enum fun_key_status
{
    SHELL_NORMAL_KEY,
    SHELL_SPEC_KEY,
    SHELL_FUNC_KEY,
};

/*! @brief shell IO user callback type. */
struct shell_ops_struct
{
    void        (*send_data)(const uint8_t *buf, uint32_t len);
    uint32_t    (*rev_data)(uint8_t* buf, uint32_t len, uint32_t timeout);
};

#if 0
struct shell_user_config_struct
{
    char *prompt;
#if defined(SHELL_USE_FILE_STREAM)
    FILE *STDOUT, *STDIN, *STDERR;
#else
    shell_ops_t ops;                /* IO interface operation */
#endif
    shell_cmd_tbl_t cmd_tab;        /* command tables */
    uint8_t cmd_num;                /* number of user commands */
};
#endif

/*!
 * @brief Runtime state structure for shell.
 *
 * This structure holds data that is used by the shell to manage operation.
 * The user must pass the memory for this run-time state structure and the shell
 * fills out the members.
 */
struct shell_context_struct
{
    char *prompt;                   /* prompt string */
    enum fun_key_status stat;       /* special key status */
    char line[SHELL_BUFFER_SIZE];   /* consult buffer */
    uint8_t cmd_num;                /* number of user commands */
    uint8_t l_pos;                  /* total line postion */
    uint8_t c_pos;                  /* current line postion */
#if SHELL_USE_FILE_STREAM
    FILE *STDOUT, *STDIN, *STDERR;
#else
    shell_ops_t ops;                /* IO interface operation */
#endif
    shell_cmd_tbl_t cmd_tab;        /* command tables */
#if SHELL_USE_HISTORY
    uint16_t hist_current;
    uint16_t hist_count;
    char hist_buf[SHELL_HIST_MAX][SHELL_BUFFER_SIZE];
#endif
    bool exit;
};

/*!
 * @brief shell command table struct.
 *
 * This structure holds command function information that is used by the shell to find and compare input command.
 */
struct shell_cmd_tbl_struct
{
    char            *name;		
    cmd_function_t  cmd;
    char            *help;
};

/*!
 * @brief getopt global varibles.
 *
 */
typedef struct shell_getopt_context
{
    int   opterr;
    int   optind;
    int   optopt;
    int   sp;
    char  *optarg;
}SHELL_GETOPT_CONTEXT;

/*!
 * @brief Main loop for shell.
 *
 * Main loop for shell, After this function is called, shell will begin to initialize basic variable and begin work.
 * this function will not return until shell->exit is true;
 *
 * @param shell_context_t The pointer to the software runtime states.
 */
int shell_main(shell_context_t context);

/*!
 * @brief shell printf function.
 *
 * Implement printf function via shell IO.
 *
 * @param shell_context_t The pointer to the software runtime states.
 * @param format format strings.
 */
int shell_printf(shell_context_t context, const char * format, ...);

/*!
 * @brief shell getopt init function.
 *
 * Initalize use of shell_getopt function, use must call it before use shell_getopt
 *
 * @param ctx The pointer to SHELL_GETOPT_CONTEXT.
 */
void shell_getopt_init(SHELL_GETOPT_CONTEXT* ctx);

/*!
 * @brief shell getopt function.
 *
 * Implement getopt function via shell IO.
 *
 * @param argc argument count.
 * @param argv argument vector.
 * @param opts option string.
 * @param ctx pointer of SHELL_GETOPT_CONTEXT to store runtime status.
 */
int32_t shell_getopt(int argc, char** argv, char* opts, SHELL_GETOPT_CONTEXT* ctx);

#endif /* __FSL_SHELL_H__ */

/*******************************************************************************
 * EOF
 ******************************************************************************/

