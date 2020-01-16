/* KalioOS (C) 2020 Pranav Bagur */

#include "if/shell_cmds.h"
#include "../drivers/if/screen.h"
#include "../common/if/common.h"
#include "../common/if/stack.h"
#include "../fs/if/fs.h"

vfs_node_t *prev_node = NULL;

/* --------------------------------------------------------------------------
                         Static inline functions
   -------------------------------------------------------------------------- */
static inline void
shell_cmd_add_node(shell_cmd_t *cmd, vfs_node_t *cur_node, ub4 flag)
{
  list       *cur;

  list_for_each(cur, &cmd->arg_list_sc) {
    vfs_node_t   *node;
    shell_args_t *arg = list_entry(cur, shell_args_t, link_sa);

    if (!!find_fs(cur_node, arg->arg_sa)) {
      erase_cursor();
      printk_shell(arg->arg_sa);
      printk_shell(" already exists\n");
    }
    else {
      node = fs_init_node(arg->arg_sa, flag, cur_node);

      /* TODO disallow special charecters in dir/file name */
      list_add_tail(&cur_node->nodes_list_vfs_node, &node->link_vfs_node);
      cur_node->nodes_list_count_vfs_node++;
    }
  }
}

/* --------------------------------------------------------------------------
                         Export functions
   -------------------------------------------------------------------------- */
/*
 * EF: shell_cmd_clear - handler for command "clear"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_clear(shell_cmd_t *cmd)
{
  clear_screen_prompt();
}

/*
 * EF: shell_cmd_clear - handler for command "clear"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_whoami(shell_cmd_t *cmd)
{
  erase_cursor();
  printk_shell(USERNAME);
  printk_shell("\n");
}

/*
 * EF: shell_cmd_pwd - handler for command "pwd"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_pwd(shell_cmd_t *cmd)
{
  stack_t    *stack;
  vfs_node_t *node          = get_current_node();
  bool        add_delimiter = false;

  if (!(stack = stack_init()))
    return;

  while(node)
  {
    if (!stack_push(stack, (void *)node))
      goto err_exit;
    node = node->parent_vfs_node;
  }

  erase_cursor();
err_exit:
  while (!is_stack_empty(stack)) {
    node = (vfs_node_t *)stack_pop(stack);
    printk_shell(node->name_vfs_node);
    if (add_delimiter)
      printk_shell("/");
    add_delimiter = true;
  }
  printk_shell("\n");
  stack_exit(stack);
}

/*
 * EF: shell_cmd_mkdir - handler for command "mkdir"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_mkdir(shell_cmd_t *cmd)
{
  shell_cmd_add_node(cmd, get_current_node(), VFS_DIRECTORY);
}

/*
 * EF: shell_cmd_rmdir - handler for command "rmdir"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_rmdir(shell_cmd_t *cmd)
{
  vfs_node_t *cur_node = get_current_node();
  list       *cur;

  list_for_each(cur, &cmd->arg_list_sc) {
    shell_args_t *arg = list_entry(cur, shell_args_t, link_sa);
    vfs_node_t   *node = find_fs(cur_node, arg->arg_sa);

    if (!node || node->magic_vfs_node != VFS_NODE_MAGIC) {
      erase_cursor();
      printk_shell(arg->arg_sa);
      printk_shell(" not found\n");
    }
    else if (!(node->flags_vfs_node & VFS_DIRECTORY)) {
      erase_cursor();
      printk_shell(arg->arg_sa);
      printk_shell(" not a directory\n");
    }
    else if (node->nodes_list_count_vfs_node > 0) {
      erase_cursor();
      printk_shell(arg->arg_sa);
      printk_shell(" directory not empty\n");
    }
    else {
      erase_cursor();
      printk_shell("Directory ");
      printk_shell(node->name_vfs_node);
      printk_shell(" deleted\n");
      list_remove(&cur_node->nodes_list_vfs_node, &node->link_vfs_node);
      cur_node->nodes_list_count_vfs_node--;
      fs_exit_node(node);
    }
  }
}

/*
 * EF: shell_cmd_touch - handler for command "touch"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_touch(shell_cmd_t *cmd)
{
  shell_cmd_add_node(cmd, get_current_node(), VFS_FILE);
}

/*
 * EF: shell_cmd_rm - handler for command "rm"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_rm(shell_cmd_t *cmd)
{
  vfs_node_t *cur_node = get_current_node();
  list       *cur;

  list_for_each(cur, &cmd->arg_list_sc) {
    shell_args_t *arg = list_entry(cur, shell_args_t, link_sa);
    vfs_node_t   *node = find_fs(cur_node, arg->arg_sa);

    if (!node || node->magic_vfs_node != VFS_NODE_MAGIC) {
      erase_cursor();
      printk_shell(arg->arg_sa);
      printk_shell(" not found\n");
    }
    else if (!(node->flags_vfs_node & VFS_FILE)) {
      erase_cursor();
      printk_shell(arg->arg_sa);
      printk_shell(" not a file\n");
    }
    else {
      erase_cursor();
      printk_shell("File ");
      printk_shell(node->name_vfs_node);
      printk_shell(" deleted\n");
      list_remove(&cur_node->nodes_list_vfs_node, &node->link_vfs_node);
      cur_node->nodes_list_count_vfs_node--;
      fs_exit_node(node);
    }
  }
}

/*
 * EF: shell_cmd_cd - handler for command "cd"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_cd(shell_cmd_t *cmd)
{
  vfs_node_t *cur_node = get_current_node();
  list       *cur;

  list_for_each(cur, &cmd->arg_list_sc) {
    shell_args_t *arg = list_entry(cur, shell_args_t, link_sa);

    if (strcmp(arg->arg_sa, "..", 2) == 0) {
      vfs_node_t *node = cur_node->parent_vfs_node;

      if (node && node->magic_vfs_node == VFS_NODE_MAGIC) {
        prev_node = get_current_node();
        set_current_node(node);
      }
    }
    else if (strcmp(arg->arg_sa, ".", 1) == 0) {
      /* NOOP */
    }
    else if (strcmp(arg->arg_sa, "-", 1) == 0) {
      if (prev_node != NULL){
        vfs_node_t *cur_node =  get_current_node();

        set_current_node(prev_node);
        prev_node = cur_node;
      }
    }
    else {
      vfs_node_t *node = find_fs(cur_node, arg->arg_sa);

      if (!node || node->magic_vfs_node != VFS_NODE_MAGIC) {
        erase_cursor();
        printk_shell(arg->arg_sa);
        printk_shell(" not found\n");
      }
      else {
        prev_node = get_current_node();
        set_current_node(node);
      }
    }

    break;
  }
}

/*
 * EF: shell_cmd_exit - handler for command "exit"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_exit(shell_cmd_t *cmd)
{
  EXIT();
}

/*
 * EF: shell_cmd_ls - handler for command "echo"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_echo(shell_cmd_t *cmd)
{
  list       *cur;

  erase_cursor();
  list_for_each(cur, &cmd->arg_list_sc) {
    shell_args_t *arg = list_entry(cur, shell_args_t, link_sa);
    
    printk_shell(arg->arg_sa);
  }
  printk_shell("\n");
}

/*
 * EF: shell_cmd_write - handler for command "write"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_write(shell_cmd_t *cmd)
{
  list *cur;
  ub1  *file_name = NULL;
  ub1  *buf       = NULL;

  list_for_each(cur, &cmd->arg_list_sc) {
    shell_args_t *arg = list_entry(cur, shell_args_t, link_sa);
    
    if (!file_name)
      file_name = (ub1 *)arg->arg_sa; // arg1
    else
      buf = (ub1 *)arg->arg_sa; // arg2
  }

  if (file_name && buf) {
    vfs_node_t *node = find_fs(get_current_node(), file_name);

    if (!node || node->magic_vfs_node != VFS_NODE_MAGIC) {
      erase_cursor();
      printk_shell(file_name);
      printk_shell(" not found\n");
    }
    else {
      if (open_fs(node)) {
        write_fs(node, 0, strlen(buf), buf);
        close_fs(node);
      }
    }
  }
}

/*
 * EF: shell_cmd_cat - handler for command "cat"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_cat(shell_cmd_t *cmd)
{
  list *cur;
  shell_args_t *arg;
  ub1  *file_name = NULL;
  ub1  *buf       = NULL;

  cur = (&cmd->arg_list_sc)->next;
  arg = list_entry(cur, shell_args_t, link_sa);
  file_name = arg->arg_sa;

  if (file_name) {
    vfs_node_t *node = find_fs(get_current_node(), file_name);

    if (!node || node->magic_vfs_node != VFS_NODE_MAGIC) {
      erase_cursor();
      printk_shell(file_name);
      printk_shell(" not found\n");
    }
    else {
      if (open_fs(node)) {
        ub1 *buf = (ub1 *)kmalloc_heap(DEFAULT_BUF_SIZE);
        if (buf) {
          memset((ub1 *)buf, DEFAULT_BUF_SIZE, 0);
          read_fs(node, 0, DEFAULT_BUF_SIZE, buf);
          erase_cursor();
          printk_shell(buf);
          printk_shell("\n");
          kfree_heap((ub4 *) buf);
        }
        close_fs(node);
      }
    }
  }
}

/*
 * EF: shell_cmd_ls - handler for command "ls"
 *
 * ARGS :- parsed command structure
 *
 * RET
 */
void
shell_cmd_ls(shell_cmd_t *cmd)
{
  vfs_node_t *node = get_current_node();

  (*node->ls_vfs_node)(node);
}
