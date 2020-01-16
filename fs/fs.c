/* KalioOS (C) 2020 Pranav Bagur */

#include "if/fs.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
vfs_node_t *root_node;
vfs_node_t *cur_node;
ub4         cur_inode = 0;

/* -------------------------------------------------------------------------- 
                         Static inline functions
   -------------------------------------------------------------------------- */ 
static bool
fs_grow_file(vfs_node_t *node)
{
  ub4  new_size = node->allocated_len_vfs_node * 2;
  ub1 *buf      = NULL;

  buf = (ub1 *)kmalloc_heap(new_size);
  if (!buf)
    return false;

  memset((ub1 *)buf, new_size, 0);
  memcpy(node->file_buf_vfs_node, buf, node->file_len_vfs_node);

  kfree_heap((ub4 *)node->file_buf_vfs_node);
  node->file_buf_vfs_node = buf;
  node->allocated_len_vfs_node = new_size;

  return true;
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 

/* 
 * EF: fs_init_node - initialize node
 * 
 * ARGS :-
 *   name   - name of the node
 *   flags  - see vfs.h
 *   parent - parent node
 *
 * RET -
 *   If successful - initialized vfs_node_t
 *   Else          - NULL
 */
vfs_node_t *
fs_init_node(ub1 *name, ub4 flags, vfs_node_t *parent)
{
  vfs_node_t *node;

  node = (vfs_node_t *)kmalloc_heap(sizeof(*node));
  if (!node)
    goto err_exit;

  memset((ub1 *)node, sizeof(*node), 0);

  /* Initialize defaults for our initrd*/
  memcpy(name, node->name_vfs_node, strlen(name));
  node->magic_vfs_node            = VFS_NODE_MAGIC;
  node->flags_vfs_node            = flags;
  node->inode_vfs_node            = cur_inode++;
  node->opened_vfs_node           = false;
  node->parent_vfs_node           = parent;
  node->ptr_vfs_node              = NULL;
  node->allocated_len_vfs_node    = 0;
  node->file_len_vfs_node         = 0;
  node->file_buf_vfs_node         = NULL;
  node->read_vfs_node             = read_fs; 
  node->write_vfs_node            = write_fs; 
  node->open_vfs_node             = open_fs; 
  node->close_vfs_node            = close_fs; 
  node->ls_vfs_node               = ls_fs; 
  node->find_vfs_node             = find_fs; 
  node->nodes_list_count_vfs_node = 0; 
  list_init(&node->nodes_list_vfs_node);

  return node;

err_exit:
  printk("Failed to allocate node for: ");
  printk(name);
  printk("\n");
  return NULL;
}

/* 
 * EF: fs_exit_node - teardown node
 * 
 * ARGS :-
 *   node - node to be freed
 *
 * RET -
 */
void
fs_exit_node(vfs_node_t *node)
{
  kfree_heap((ub4 *)node);
}

/* 
 * EF: get_current_node - get current node
 * 
 * ARGS :-
 *
 * RET -
 *   address of current vfs_node_t
 */
vfs_node_t *
get_current_node(void)
{
  return cur_node;
}

/* 
 * EF: set_current_node - set current node
 * 
 * ARGS :-
 *   address of vfs_node_t to set current to
 *
 * RET -
 */
void
set_current_node(vfs_node_t *node)
{
  cur_node = node;
}

/* 
 * EF: read_fs - read from file/device
 * 
 * ARGS :-
 *   node   - address of vfs_node_t (file/device)
 *   size   - number of bytes to read
 *   buffer - buffer to read to
 *
 * RET -
 *   number of bytes read
 *
 */
ub4 read_fs(vfs_node_t *node, ub4 offset, ub4 size, ub1 *buffer)
{
  if (offset > node->file_len_vfs_node)
    return 0;
  //else if (offset + size > node->file_len_vfs_node)
  //  size = node->file_len_vfs_node - offset;

  printk_shell("reading: ");
  printk_shell(node->file_buf_vfs_node);
  printk_shell("\n");

  memcpy((ub1 *)(node->file_buf_vfs_node + offset), (ub1 *)buffer, size);
  return size;
}

/* 
 * EF: write_fs - write from file/device
 * 
 * ARGS :-
 *   node   - address of vfs_node_t (file/device)
 *   size   - number of bytes to write
 *   buffer - buffer to write from
 *
 * RET -
 *   number of bytes written
 *
 */
ub4 write_fs(vfs_node_t *node, ub4 offset, ub4 size, ub1 *buffer)
{
  if (offset + size > node->allocated_len_vfs_node)
    if (!fs_grow_file(node))
      return 0;

  printk_shell("writing: ");
  printk_shell(buffer);
  printk_shell("\n");
  memcpy((ub1 *)buffer, (ub1 *)(node->file_buf_vfs_node + offset), size);
  printk_shell(node->file_buf_vfs_node);
  printk_shell("\n");
  return size;
}

/* open file/device etc. */ 
vfs_node_t *open_fs(vfs_node_t *node)
{
  ASSERT(node->opened_vfs_node == false);
  ASSERT((node->flags_vfs_node & VFS_DIRECTORY) == 0);
  if (node->allocated_len_vfs_node == 0) {
    node->file_buf_vfs_node = (ub1 *)kmalloc_heap(DEFAULT_BUF_SIZE);
    if (!node)
      return NULL;

    memset((ub1 *)node->file_buf_vfs_node, DEFAULT_BUF_SIZE, 0);
    node->allocated_len_vfs_node = DEFAULT_BUF_SIZE;
  }

  node->opened_vfs_node = true;
  return node;
}

/* close file/device etc. */ 
void close_fs(vfs_node_t *node)
{
  ASSERT(node->opened_vfs_node == true);
  ASSERT((node->flags_vfs_node & VFS_DIRECTORY) == 0);
  node->opened_vfs_node = false;
}

/* list all child nodes */ 
void ls_fs(vfs_node_t *node)
{
  list *cur;

  list_for_each(cur, &node->nodes_list_vfs_node) {
    vfs_node_t *cur_node = list_entry(cur, vfs_node_t, link_vfs_node);

    erase_cursor();
    if ((cur_node->flags_vfs_node & VFS_DIRECTORY) != 0) {
      printk_shell("dir    ");
    }
    else if ((cur_node->flags_vfs_node & VFS_FILE) != 0) {
      printk_shell("file   ");
    }

    printk_shell(USERNAME);
    printk_shell("/");
    printk_shell(GROUPNAME);
    printk_shell("   ");
    printk_shell(cur_node->name_vfs_node);
    printk_shell("\n");
  }
}

/* find child node from name */ 
vfs_node_t *find_fs(vfs_node_t *node, ub1 *name)
{
  list *cur;

  list_for_each(cur, &node->nodes_list_vfs_node) {
    vfs_node_t *cur_node = list_entry(cur, vfs_node_t, link_vfs_node);

    if (strcmp(cur_node->name_vfs_node, name) == 0)
      return cur_node;
  }

  return NULL;
}


/* 
 * EF: fs_init_func - module init function
 * 
 * ARGS :-
 *
 * RET - TRUE iff successful
 */
bool
fs_init_func(void)
{
  vfs_node_t *node;
  ub4         i = 0;
  ub1        *node_names[5] = {"scratch", "var", "bin", "log", "home"};
  
  root_node = fs_init_node("/", VFS_DIRECTORY, NULL);

  for (i = 0; i < ARRAY_SIZE(node_names); i++)
  {
    node = fs_init_node(node_names[i], VFS_DIRECTORY, root_node);
    list_add_tail(&root_node->nodes_list_vfs_node, &node->link_vfs_node);
    root_node->nodes_list_count_vfs_node++;
  }

  cur_node = fs_init_node(USERNAME, VFS_DIRECTORY, node);
  list_add_tail(&node->nodes_list_vfs_node, &cur_node->link_vfs_node);
  cur_node->nodes_list_count_vfs_node++;

  printk_system("Initialized FS..");
  return true;
}

/* 
 * EF: fs_exit_func - module exit function
 * 
 * ARGS :-
 *
 * RET 
 */
void
fs_exit_func(void)
{
}
