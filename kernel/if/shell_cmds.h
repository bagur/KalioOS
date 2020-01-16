/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __SHELL_CMDS_H
#define __SHELL_CMDS_H

#include "../../common/if/types.h"
#include "../../common/if/common.h"
#include "../../common/if/list.h"
#include "../../mm/if/heap.h"
#include "../../fs/if/fs.h"
#include "shell.h"

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* handler for command "clear" */
void shell_cmd_clear(shell_cmd_t *cmd); 

/* handler for command "whoami" */
void shell_cmd_whoami(shell_cmd_t *cmd); 

/* handler for command "pwd" */
void shell_cmd_pwd(shell_cmd_t *cmd);

/* handler for command "mkdir" */
void shell_cmd_mkdir(shell_cmd_t *cmd);

/* handler for command "rmdir" */
void shell_cmd_rmdir(shell_cmd_t *cmd);

/* handler for command "touch" */
void shell_cmd_touch(shell_cmd_t *cmd);

/* handler for command "rm" */
void shell_cmd_rm(shell_cmd_t *cmd);

/* handler for command "cd" */
void shell_cmd_cd(shell_cmd_t *cmd);

/* handler for command "exit" */
void shell_cmd_exit(shell_cmd_t *cmd);

/* handler for command "echo" */
void shell_cmd_echo(shell_cmd_t *cmd);

/* handler for command "write" */
void shell_cmd_write(shell_cmd_t *cmd);

/* handler for command "cat" */
void shell_cmd_cat(shell_cmd_t *cmd);

/* handler for command "ls" */
void shell_cmd_ls(shell_cmd_t *cmd);

#endif
