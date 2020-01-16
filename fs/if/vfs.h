/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __VFS_H
#define __VFS_H

#include "../../common/if/types.h"
#include "../../common/if/common.h"
#include "../../common/if/list.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

#define VFS_FILE        1
#define VFS_DIRECTORY   2
#define VFS_SYMLINK     4
#define VFS_MOUNTPOINT  8

#define VFS_NODE_MAGIC  0x9124

typedef struct _vfs_node vfs_node_t;

typedef ub4          (*read_func_t)(vfs_node_t *, ub4, ub4, ub1*);
typedef ub4          (*write_func_t)(vfs_node_t *, ub4, ub4, ub1*);
typedef vfs_node_t * (*open_func_t)(vfs_node_t *);
typedef void         (*close_func_t)(vfs_node_t *);
typedef void         (*list_func_t)(vfs_node_t *);
typedef vfs_node_t * (*find_func_t) (vfs_node_t *, ub1 *name); 

struct _vfs_node
{
   list              link_vfs_node;
   ub4               magic_vfs_node;     /* magic #                           */
   ub1               name_vfs_node[32];  /* name of the node (dir/file name)  */
   ub4               flags_vfs_node;     /* file/dir/symlink/mountpoint?      */
   ub4               inode_vfs_node;     /* inode number                      */
   bool              opened_vfs_node;

   /* -------------- Interfaces --------------------------------------------- */
   read_func_t       read_vfs_node;      /* read data                         */
   write_func_t      write_vfs_node;     /* write data                        */
   open_func_t       open_vfs_node;      /* open                              */
   close_func_t      close_vfs_node;     /* close                             */
   list_func_t       ls_vfs_node;        /* list child nodes                  */
   find_func_t       find_vfs_node;      /* find node from name               */

   struct _vfs_node *parent_vfs_node;    /* parent directory                  */
   struct _vfs_node *ptr_vfs_node;       /* mountpoint or symlink             */

   /* -------------- File specific ------------------------------------------ */
#define DEFAULT_BUF_SIZE 64
   ub4               allocated_len_vfs_node;
   ub4               file_len_vfs_node;  /* file size                         */
   ub1              *file_buf_vfs_node;  /* buffer                            */   

   /* -------------- Dir specific ------------------------------------------- */
   ub4               nodes_list_count_vfs_node;
   list              nodes_list_vfs_node;
};
 
#endif
