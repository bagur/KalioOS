/* KalioOS (C) 2020 Pranav Bagur */
/*
* +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
* | c  | a  | t  | \0 |    |    | f  | i  | l  | e  | 1  | \0 |    |    |    | f  | i  | l  | e  | 2  | \0 |    |
* +----+----+----+-------------------+----+---------------------------------------+----+----+----+----+----+----+
* ^                             ^                                            ^
* |                             |                                            |
* +----- +                      +-+                    +---------------------+
*        |                        |                    |
*        |                        |                    |
* +------------+            +-----------+        +----------+
* |      |     |            |     |     |        |     |    |
* |      |     +----------->+  arg_sa   +------->+  arg_sa  |
* |      |     |            +-----------+        +----------+
* |   cmd_sc   |
* |            |
* |            |
* |            |
* |            |
* +------------+
*
*/ 

#ifndef __SHELL_H
#define __SHELL_H

#include "../../common/if/types.h"
#include "../../common/if/common.h"
#include "../../common/if/list.h"
#include "../../mm/if/heap.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
/* 
 * STRUCT shell_args_t - Describes a command argument
 * the tub) 
 */
typedef struct _sa
{
  list      link_sa;

  ub1      *arg_sa;
  ub4       arg_len_sa;
} shell_args_t;

/* STRUCT shell_cmd_t - Describes a shell command */
typedef struct _sc
{
  list      arg_list_sc;
  ub4       args_count_sc;

  ub1      *cmd_sc;
  ub4       cmd_len_sc;
} shell_cmd_t;

/* Describes a command handler routine */
typedef void (*cmd_func)(shell_cmd_t *);

/* 
 * STRUCT shell_cmds_t - Table to maintain mapping between 
 * command string and shell_cmd_t 
 */
typedef struct _scs
{
  ub1      *cmd_scs;
  cmd_func  func_scs;
  ub4       min_scs;
  ub4       max_scs;
  ub1      *desc_scs;
} shell_cmds_t;

#define SHELL_MAX_ARGS 10

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 
/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* shell main loop */
bool shell_main(void);

/* shell init function   */
bool shell_init_func(void);

/* shell exit function   */
void shell_exit_func(void);

#endif
