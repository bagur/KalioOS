/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __FS_H
#define __FS_H

#include "../../common/if/types.h"
#include "../../common/if/common.h"
#include "../../common/if/list.h"
#include "vfs.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* get current node */
vfs_node_t *get_current_node(void);

/* set current node */
void set_current_node(vfs_node_t *node);

/* initialize vfs node */ 
vfs_node_t *fs_init_node(ub1 *name, ub4 flags, vfs_node_t *parent);

/* teardown vfs node */ 
void fs_exit_node(vfs_node_t *node);

/* read from file/device etc. */ 
ub4 read_fs(vfs_node_t *node, ub4 offset, ub4 size, ub1 *buffer);

/* write to file/device etc. */ 
ub4 write_fs(vfs_node_t *node, ub4 offset, ub4 size, ub1 *buffer);

/* open file/device etc. */ 
vfs_node_t *open_fs(vfs_node_t *node);

/* close file/device etc. */ 
void close_fs(vfs_node_t *node);

/* list all child nodes */ 
void ls_fs(vfs_node_t *node);

/* find child node from name */ 
vfs_node_t *find_fs(vfs_node_t *node, ub1 *name); 

/* module init function   */ 
bool fs_init_func(void);

/* module exit function   */ 
void fs_exit_func(void);

#endif
