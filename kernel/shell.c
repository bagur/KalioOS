/* KalioOS (C) 2020 Pranav Bagur */

#include "if/shell.h"
#include "if/shell_cmds.h"
#include "../drivers/if/screen.h"
#include "../drivers/if/keyboard.h"
#include "../common/if/common.h"

/* --------------------------------------------------------------------------
                         Static function declarations
   -------------------------------------------------------------------------- */
static void shell_process(ub4 numentries);
static void shell_process_cmd(ub1 *cmd);
static void shell_print_command(shell_cmd_t *cmd);

/* --------------------------------------------------------------------------
                         Constants and types
   -------------------------------------------------------------------------- */
/*
 * TODO Use a Trie? Might have to rewrite this entire file
 */
ub1 local_shell_buf[KEYBOARD_RING_BUF_MAX];
ub4 local_shell_buf_idx;

shell_cmds_t cmds[13] = {
  {"clear",  shell_cmd_clear,  0, 0,              "clear screen"},
  {"whoami", shell_cmd_whoami, 0, 0,              "print current uid"},
  {"pwd",    shell_cmd_pwd,    0, 0,              "print working dir"},
  {"mkdir",  shell_cmd_mkdir,  1, SHELL_MAX_ARGS, "create dirs"},
  {"rmdir",  shell_cmd_rmdir,  1, SHELL_MAX_ARGS, "remove dirs"},
  {"touch",  shell_cmd_touch,  1, SHELL_MAX_ARGS, "create files"},
  {"rm",     shell_cmd_rm,     1, SHELL_MAX_ARGS, "remove dirs"},
  {"cd",     shell_cmd_cd,     1, 1,              "move to directory"},
  {"exit",   shell_cmd_exit,   0, 0,              "shutdown system"},
  {"echo",   shell_cmd_echo,   1, 1,              "echo back the arg"},
  {"write",  shell_cmd_write,  2, 2,              "write to file"},
  {"cat",    shell_cmd_cat,    1, 1,              "read file"},
  {"ls",     shell_cmd_ls,     0, 0,              "list children of cur node"}
};

/* --------------------------------------------------------------------------
                         Static inline functions
   -------------------------------------------------------------------------- */
/* === SIF: Print help message for a command === */
static inline void
shell_print_help(ub4 idx)
{
  erase_cursor();
  printk_shell("HELP: ");
  printk_shell(cmds[idx].cmd_scs);
  printk_shell(" [");
  printk_shell_num(cmds[idx].min_scs);
  printk_shell(", ");
  printk_shell_num(cmds[idx].max_scs);
  printk_shell("] ");
  printk_shell(cmds[idx].desc_scs);
  printk_shell("\n");
}

/* === SIF: Check the arguments for a command === */
static inline bool
shell_check_args(shell_cmd_t *cur_cmd, ub4 idx)
{
  if (cur_cmd->args_count_sc >= cmds[idx].min_scs &&
      cur_cmd->args_count_sc <= cmds[idx].max_scs)
    return true;
  
  shell_print_help(idx);
  return false;
}

/* === SIF: Add a token to the current command (or cmd itself) === */
static inline bool
shell_add_token(shell_cmd_t *cur_cmd, ub1 *cur_token, ub4 len)
{
  if (cur_cmd->cmd_sc == NULL) {
    /* This token is our command */
    cur_cmd->cmd_sc     = cur_token;
    cur_cmd->cmd_len_sc = len;
  }
  else {
    shell_args_t *arg;

    /* Found a new argument - add it to our list */
    arg = (shell_args_t *)kmalloc_heap(sizeof(*arg));
    if (!arg)
      goto err_exit;

    memset((ub1 *)arg, sizeof(*arg), 0);
    arg->arg_sa     = cur_token; /* refer to local_shell_buf */
    arg->arg_len_sa = len;
    list_add_tail(&cur_cmd->arg_list_sc, &arg->link_sa);
    cur_cmd->args_count_sc++;
  }

  return true;

err_exit:
  return false;
}

/* --------------------------------------------------------------------------
                         Static functions
   -------------------------------------------------------------------------- */
/*
 * SF: shell_print_command - print the current parsed command and args
 *
 * ARGS :-
 *   cmd - parsed command structure (see shell.h)
 *
 * RET
 */
static void
shell_print_command(shell_cmd_t *cmd) {
  list *cur;

  printk("cmd:");
  printk(cmd->cmd_sc);

  list_for_each(cur, &cmd->arg_list_sc) {
    shell_args_t *arg = list_entry(cur, shell_args_t, link_sa);
    printk("\narg:");
    printk(arg->arg_sa);
  }
  printk("\n");
}

/*
 * SF: shell_free_tokens - Free our allocated cmd structure and args
 *
 * ARGS :-
 *   cmd - parsed cmd structure (see shell.h)
 *
 * RET
 */
static void
shell_free_tokens(shell_cmd_t *cur_cmd)
{
  list *item;

  while(cur_cmd->args_count_sc) {
    shell_args_t *arg;

    item = list_remove_front(&cur_cmd->arg_list_sc);
    cur_cmd->args_count_sc--;

    arg = list_entry(item, shell_args_t, link_sa);
    kfree_heap((ub4 *)arg);
  }

  kfree_heap((ub4 *)cur_cmd);
}

/*
 * SF: shell_tokenize - tokenize the current command
 *
 * ARGS :-
 *   cmd_str - current command string that the user has entered
 *
 * RET
 *   shell_cmd_t - see shell.h for structure
 */
static shell_cmd_t *
shell_tokenize(ub1 *cmd_str)
{
  shell_cmd_t *cur_cmd;
  ub1         *cur_token;
  ub4          i       = 0;
  ub4          len     = 0;
  bool         esc_set = false;

  cur_cmd = (shell_cmd_t *)kmalloc_heap(sizeof(*cur_cmd));
  if (!cur_cmd) {
    goto err_exit;
  }

  memset((ub1 *)cur_cmd, sizeof(*cur_cmd), 0);
  list_init(&cur_cmd->arg_list_sc);
  cur_cmd->args_count_sc = 0;
  cur_cmd->cmd_len_sc    = 0;
  cur_cmd->cmd_sc        = NULL;

  /* get rid of leading whitespaces */
  while (cmd_str[i] == ' ') i++;

  cur_token = &cmd_str[i];
  while(cmd_str[i] != '\0') {
    len++;

    if (cmd_str[i] == '\"') {
      esc_set = !esc_set; 
      i++;
    }
    else if (cmd_str[i] != ' ' || esc_set)
      i++;
    else {
      cmd_str[i] = '\0';
      if (!shell_add_token(cur_cmd, cur_token, len))
        goto mem_exit;

      i++;
      /* get rid of leading whitespaces */
      while (cmd_str[i] == ' ') i++;
      cur_token = &cmd_str[i];
      len       = 0;
    }
  }

  /* process the last token */
  if (cur_token[0] != '\0') {
    if (!shell_add_token(cur_cmd, cur_token, len + 1))
      goto mem_exit;
  }

  return cur_cmd;

mem_exit:
  shell_free_tokens(cur_cmd);
err_exit:
  printk("Failed to tokenize ");
  printk(cmd_str);
  printk("\n");
  return NULL;
}

/*
 * SF: shell_process_cmd - process current shell command
 *
 * ARGS :-
 *   cmd_str - current command string that the user has entered
 *
 * RET
 */
static void
shell_process_cmd(ub1 *cmd_str)
{
  ub4          i = 0;
  shell_cmd_t *cur_cmd;
  bool         found_cmd = false;

  if (cmd_str) {
    cur_cmd = shell_tokenize(cmd_str);

    if (cur_cmd) {
#ifdef DEBUG
      shell_print_command(cur_cmd);
#endif
      for (i = 0; i < ARRAY_SIZE(cmds); i++) {
        if (strcmp(cur_cmd->cmd_sc, cmds[i].cmd_scs) == 0) {
          if (shell_check_args(cur_cmd, i)) {
            (*cmds[i].func_scs)(cur_cmd);
          }
          found_cmd = true;
        }
      }

      if (!found_cmd) {
        erase_cursor();
        printk_shell("Invalid command : ");
        printk_shell(cur_cmd->cmd_sc);
        printk_system("\n");
      }

      shell_free_tokens(cur_cmd);
    }
  }
}

/* --------------------------------------------------------------------------
                         Export functions
   -------------------------------------------------------------------------- */
/*
 * EF: shell_main - shell main loop
 *
 * ARGS :-
 *
 * RET - FALSE if exit
 *       TRUE otherwise
 */
bool
shell_main()
{
  ub8  flags;
  ub1  cur_ch;
  bool found_cmd = false;

  /* synchronize by disabling interrupts */
  lock_intr(&flags);
  while (rb_pop(rb_keyboard, &cur_ch)) {
    if (local_shell_buf_idx >= KEYBOARD_RING_BUF_MAX)
      local_shell_buf_idx = 0;

    if (cur_ch == '\b') {
      /*
       * Backspace charecter. Delete the previous char
       * from our local buffer
       */
      if (local_shell_buf_idx > 0) {
        local_shell_buf_idx--;
        local_shell_buf[local_shell_buf_idx] = '\0';
      }
    }
    else if (cur_ch == '\n') {
      /*
       * Found a command. Break out of the loop to
       * process
       */
      local_shell_buf[local_shell_buf_idx] = '\0';
      local_shell_buf_idx = 0;
      found_cmd = true;
      break;
    }
    else {
      /*
       * Add the current charecter to our local buffer
       */
      local_shell_buf[local_shell_buf_idx++] = cur_ch;
    }
  }
  unlock_intr(&flags);

  if (found_cmd)
    shell_process_cmd(local_shell_buf);

  return true;
}

/*
 * EF: shell_init_func - init shell
 *
 * ARGS :-
 *
 * RET - TRUE
 */
bool
shell_init_func()
{
  local_shell_buf_idx = 0;
  printk_system("Initialized shell..");
  return true;
}

/*
 * EF: shell_exit_func - exit shell
 *
 * ARGS :-
 *
 * RET -
 */
void
shell_exit_func()
{
}
