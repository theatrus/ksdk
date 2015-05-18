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

/*
POSIX getopt for Windows
Code given out at the 1985 UNIFORUM conference in Dallas.  
*/

// From std-unix@ut-sally.UUCP (Moderator, John Quarterman) Sun Nov  3 14:34:15 1985
// Relay-Version: version B 2.10.3 4.3bsd-beta 6/6/85; site gatech.CSNET
// Posting-Version: version B 2.10.2 9/18/84; site ut-sally.UUCP
// Path: gatech!akgua!mhuxv!mhuxt!mhuxr!ulysses!allegra!mit-eddie!genrad!panda!talcott!harvard!seismo!ut-sally!std-unix
// From: std-unix@ut-sally.UUCP (Moderator, John Quarterman)
// Newsgroups: mod.std.unix
// Subject: public domain AT&T getopt source
// Message-ID: <3352@ut-sally.UUCP>
// Date: 3 Nov 85 19:34:15 GMT
// Date-Received: 4 Nov 85 12:25:09 GMT
// Organization: IEEE/P1003 Portable Operating System Environment Committee
// Lines: 91
// Approved: jsq@ut-sally.UUCP

// Here's something you've all been waiting for:  the AT&T public domain
// source for getopt(3).  It is the code which was given out at the 1985
// UNIFORUM conference in Dallas.  I obtained it by electronic mail
// directly from AT&T.  The people there assure me that it is indeed
// in the public domain.

// There is no manual page.  That is because the one they gave out at
// UNIFORUM was slightly different from the current System V Release 2
// manual page.  The difference apparently involved a note about the
// famous rules 5 and 6, recommending using white space between an option
// and its first argument, and not grouping options that have arguments.
// Getopt itself is currently lenient about both of these things White
// space is allowed, but not mandatory, and the last option in a group can
// have an argument.  That particular version of the man page evidently
// has no official existence, and my source at AT&T did not send a copy.
// The current SVR2 man page reflects the actual behavor of this getopt.
// However, I am not about to post a copy of anything licensed by AT&T.


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

/*******************************************************************************
 * PROTOTYPES
 ******************************************************************************/
static int shell_exec_fun(shell_context_t context, char* cmd, uint32_t len);
static void shell_auto_complete(shell_context_t context);
static uint8_t shell_getc(shell_context_t context);
static cmd_function_t shell_find_cmd(shell_context_t context, const char *cmd);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define KEY_ESC   (0x1B)
#define KET_DEL   (0x7F)
/*******************************************************************************
 * Code
 ******************************************************************************/
 
 /*FUNCTION**********************************************************************
 *
 * Function Name : shell_printf
 * Description   : Implement printf function using shell_context IO interface.
 *
 *END**************************************************************************/
int shell_printf(shell_context_t context, const char * format, ...)
{
    int chars,i;
    char buf[SHELL_BUFFER_SIZE + 16];
    va_list ap;
    va_start(ap, format);
    chars = vsnprintf(buf, SHELL_BUFFER_SIZE, format, ap);
#if SHELL_USE_FILE_STREAM
    fputs(buf, context->STDOUT);
    fflush(context->STDOUT);
#else
    context->ops->send_data((const uint8_t *)buf, chars);
#endif
    va_end(ap);
    return chars;
}

static uint8_t shell_getc(shell_context_t context)
{
    uint8_t ch;
#if SHELL_USE_FILE_STREAM
    ch = fgetc(context->STDIN);
#else
    context->ops->rev_data(&ch, 1, 1);
#endif
    return ch;
}

#if 0
int shell_init(shell_context_t context, const shell_user_config_t config)
{

    context->prompt = config->prompt;
    context->STDERR = config->STDERR;
    context->STDOUT = config->STDOUT;
    context->STDIN = config->STDIN;
    context->cmd_tab = config->cmd_tab;
    context->cmd_num = config->cmd_num;
    
    return 0;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : shell_main
 * Description   : shell main loop function.
 *
 *END**************************************************************************/
int shell_main(shell_context_t context)
{
    uint8_t ch;

    /* parms check */
    if((!context) || (!context->cmd_tab) || (!context->prompt))
    {
        return -1;
    }

    context->exit = false;
    
    shell_printf(context, "\r\nSHELL (build: %s)\r\n", __DATE__);
    shell_printf(context, "Copyright (c) 2014 Freescale Semiconductor\r\n");
    shell_printf(context, context->prompt);
    
    while(1)
    {
        if(context->exit)
        {
            break;  
        }
        
        /* get char */
        ch = shell_getc(context);
        
        /* special key */
        if (ch == KEY_ESC)
        {
            context->stat = SHELL_SPEC_KEY;
            continue;
        }
        else if (context->stat == SHELL_SPEC_KEY)
        {
            /* func key */
            if (ch == '[')
            {
                context->stat = SHELL_FUNC_KEY;
                continue;
            }

            context->stat = SHELL_NORMAL_KEY;
        }
        else if (context->stat == SHELL_FUNC_KEY)
        {
            context->stat = SHELL_NORMAL_KEY;
            switch(ch)
            {
                /* history operation here */
                case 'A': /* up key */
                    break;
                case 'B': /* down key */
                    break;
                case 'D': /* left key */
                    if (context->c_pos)
                    {
                        shell_printf(context, "\b");
                        context->c_pos --;
                    }
                    break;
                case 'C': /* right key */
                    if (context->c_pos < context->l_pos)
                    {
                        shell_printf(context, "%c", context->line[context->c_pos]);
                        context->c_pos ++;
                    }
                    break;
            }
            continue;
        }
        
        /* handle tab key */
        else if (ch == '\t')
        {
#if SHELL_AUTO_COMPLETE
            int i;
            /* move the cursor to the beginning of line */
            for (i = 0; i < context->c_pos; i++)
            {
                shell_printf(context, "\b");
            } 
            /* do auto complete */
            shell_auto_complete(context);
            /* move position to end */
            context->c_pos = context->l_pos = strlen(context->line);
#endif
            continue;
        }
        
        /* handle backspace key */
        else if (ch == KET_DEL || ch == '\b')
        {
            /* there must be at lastest one char */
            if (context->c_pos == 0) continue;
                    
            context->l_pos--;
            context->c_pos--;

            if (context->l_pos > context->c_pos)
            {
                int i;
                memmove(&context->line[context->c_pos],&context->line[context->c_pos + 1],context->l_pos - context->c_pos);
                context->line[context->l_pos] = 0;
                printf("\b%s  \b", &context->line[context->c_pos]);

                /* reset postion */
                for (i = context->c_pos; i <= context->l_pos; i++)
                {
                    shell_printf(context, "\b");
                }              
            }
            else /* normal backspace operation */
            {
                shell_printf(context, "\b \b");
                context->line[context->l_pos] = 0;
            }
            continue;
        }
            
        /* input too long */
        if (context->l_pos >= SHELL_BUFFER_SIZE)
        {
            context->l_pos = 0;
        }
            
        /* handle end of line, break */
        if (ch == '\r' || ch == '\n')
        {
            shell_printf(context, "\r\n");
            shell_exec_fun(context, context->line, context->l_pos);
                
            /* reset all params */
            context->c_pos = context->l_pos = 0;
            shell_printf(context, context->prompt);
            memset(context->line, 0, sizeof(context->line));
            continue;
        }
            
        /* normal character */
        if (context->c_pos < context->l_pos)
        {
            int i;

            memmove(&context->line[context->c_pos + 1], &context->line[context->c_pos], context->l_pos - context->c_pos); 
            context->line[context->c_pos] = ch;
            shell_printf(context, "%s", &context->line[context->c_pos]);
            /* move the cursor to new position */
            for (i = context->c_pos; i < context->l_pos; i++)
            {
                printf("\b");
            }
        }
        else
        {
            context->line[context->l_pos] = ch;
            shell_printf(context, "%c", ch);
        }

        ch = 0;
        context->l_pos++;
        context->c_pos++;
    }
    return 0;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : shell_parse_line
 * Description   : Split command strings.
 *
 *END**************************************************************************/
static int shell_parse_line(char* cmd, uint32_t len, char* argv[SHELL_MAX_ARGS])
{
    uint32_t argc;
    char *p;
    uint32_t position;

    /* init params */
    p = cmd;
    position = 0;
    argc = 0;

    while (position < len)
    {
        /* skip all banks */
        while ((isblank(*p)) && (position < len))
        {
            *p = '\0';
            p ++; position ++;
        }
        /* process begin of a string */
        if (*p == '"')
        {
            p ++;
            position ++;
            argv[argc] = p; argc ++;
            /* skip this string */
            while (*p != '"' && position < len)
            {
                p ++; position ++;
            }
            /* skip '"' */
            *p = '\0'; p ++; position ++;
        }
        else /* normal char */
        {
            argv[argc] = p;
            argc ++;
            while ((*p != ' ' && *p != '\t') && position < len)
            {
                p ++; position ++;
            }
        }
    }
    return argc;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : shell_find_cmd
 * Description   : Split command strings.
 *
 *END**************************************************************************/
static cmd_function_t shell_find_cmd(shell_context_t context, const char *cmd)
{
    int i;
    int len = strlen(cmd);
    /* param check */
    if((!cmd) || (!len) || (!context))
    {
        return NULL;
    }
    /* find cmd */
    for(i = 0; i < context->cmd_num; i++)
    {
        if(!context->cmd_tab[i].name)
        {
            break;
        }
        if(!strncmp(cmd, context->cmd_tab[i].name, len))
        {
            return context->cmd_tab[i].cmd;
        }
    }
    return NULL;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : shell_exec_fun
 * Description   : Find and execute commands.
 *
 *END**************************************************************************/
static int shell_exec_fun(shell_context_t context, char* cmd, uint32_t len)
{
    int argc;
    char *argv[SHELL_BUFFER_SIZE];

    argc = shell_parse_line(cmd, len, argv);
    if (argc == 0)
    {
        return -1;
    }
    /* find cmd */
    cmd_function_t fun = shell_find_cmd(context, argv[0]);
    if (!fun) 
    {
        shell_printf(context, "%s:", argv[0]);
        shell_printf(context, "command not found.\r\n");
        return -1;
    }
    /* exec this command */
    return fun(context, argc, argv);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : str_compare
 * Description   : return char pointer of last matched char.
 *
 *END**************************************************************************/
static int str_compare(const char *str1, const char *str2)
{
    const char *str = str1;
    while ((*str != 0) && (*str2 != 0) && (*str == *str2))
    {
        str ++;
        str2 ++;
    }
    return (str - str1);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : shell_auto_complete
 * Description   : Implement auto competition
 *
 *END**************************************************************************/
static void shell_auto_complete(shell_context_t context)
{
    int len, minLen, i;
    const char *namePtr, *cmdName;
    cmd_function_t help;
    
    minLen = 0;
    namePtr = NULL;
    
    if(!context->line)
    {
        return;
    }
    shell_printf(context, "\r\n");
    /* empty tab, list all commandss */
    if (context->line == '\0') 
    {
        help = shell_find_cmd(context, "help");
        if(help)
        {
            help(context, 0, NULL);
        }
        return;
    }
    /* do auto complete */
    for(i = 0; i < context->cmd_num; i++)
    {
        cmdName = context->cmd_tab[i].name;
        if(!cmdName)
        {
            break;
        }
        if (strncmp(context->line, cmdName, strlen(context->line)) == 0)
        {
            if (minLen == 0)
            {
                namePtr = cmdName;
                minLen = strlen(namePtr);
            }

            len = str_compare(namePtr, cmdName);
            if (len < minLen)
            {
                minLen = len;
            }
            /* show posible matchs */
            shell_printf(context, "%s\r\n", cmdName);
        }
    }
    /* auto complete string */
    if (namePtr)
    {
        strncpy(context->line, namePtr, minLen);
    }
    shell_printf(context, "%s%s", context->prompt, context->line);
    return ;
}

void shell_getopt_init(SHELL_GETOPT_CONTEXT* ctx)
{
    ctx->opterr = 1;
    ctx->optind = 1;
    ctx->optopt = 0;
    ctx->sp = 1;
    ctx->optarg = NULL;
}

int32_t shell_getopt(int argc, char** argv, char* opts, SHELL_GETOPT_CONTEXT* ctx)
{
    register int c;
    register char *cp;

    if(ctx->sp == 1)
    {
        if(ctx->optind >= argc || argv[ctx->optind][0] != '-' || argv[ctx->optind][1] == '\0')
        {
            ctx->optind = 1;
            return(-1);
        }
        else if(strcmp(argv[ctx->optind], "--") == 0)
        {
            ctx->optind++;
            return(-1);
        }
    }
    ctx->optopt = c = argv[ctx->optind][ctx->sp];
    if(c == ':' || (cp=strchr(opts, c)) == NULL)
    {
        // printf(": illegal option -- ", c, ctx->opterr);
        if(argv[ctx->optind][++ctx->sp] == '\0')
        {
            ctx->optind++;
            ctx->sp = 1;
        }
        return('?');
    }
    if(*++cp == ':')
    {
        if(argv[ctx->optind][ctx->sp+1] != '\0')
        {
            ctx->optarg = &argv[ctx->optind++][ctx->sp+1];
        }
        else if(++ctx->optind >= argc)
        {
            //printf(": option requires an argument -- ", c, ctx->opterr);
            ctx->sp = 1;
         return('?');
        }
        else
        {
            ctx->optarg = argv[ctx->optind++];
        }
        ctx->sp = 1;
    }
    else
    {
        if(argv[ctx->optind][++ctx->sp] == '\0')
        {
            ctx->sp = 1;
            ctx->optind++;
        }
        ctx->optarg = NULL;
    }
    return(c);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
