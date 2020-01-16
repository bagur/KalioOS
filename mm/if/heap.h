/* KalioOS (C) 2020 Pranav Bagur */

#ifndef __HEAP_H
#define __HEAP_H

#include "../../common/if/types.h"
#include "../../common/if/common.h"
#include "../../common/if/list.h"
#include "paging.h"
#include "memory.h"
/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
#define N_TUBS             6
#define MAX_BUNDLES        100
#define INIT_BUNDLES       20
#define GROW_BUNDLES_LIMIT 10
#define MAGIC_CHUNK        0x71291
/* 
 * STRUCT bundle_t - Describes a bundle (divided into chunks depending on
 * the tub) 
 */
typedef struct _bundle
{
  list      link_heap_bundle;
  list      link_tub_bundle;
  list      chunks_bundle;
  ub4       chunks_count_bundle;

  ub4       chunks_in_use_bundle;
  ub4      *mem_bundle;
} bundle_t;

/* STRUCT tub_t - Describes a tub */
typedef struct _tub
{
  list      bundles_tub;
  ub4       bundles_count_tub;

  list      avl_chunks_tub;
  ub4       avl_chunks_count_tub;

  ub4       total_in_use_tub;
  ub4       total_tub;

  ub4       size_tub;
} tub_t;

/* STRUCT chunk_t - Describes a chunk of memory */
typedef struct _chunk
{
  list      link_avl_chunk;
  list      link_bundle_chunk;
  ub4       magic_chunk;
  bool      in_use_chunk;
  tub_t    *bp_tub_chunk;
  bundle_t *bp_bundle_chunk;
  ub4       size_chunk;
} chunk_t;

/* STRUCT heap_t - Describes the heap (holds multiple tubs) */
typedef struct _heap {
  list      free_bundles_heap;
  ub4       free_bundles_count;

  list      in_use_bundles_heap;
  ub4       in_use_bundles_count;

  ub4       total_bundles;
  tub_t     tubs[N_TUBS];
} heap_t;

/* -------------------------------------------------------------------------- 
                         Macros
   -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- 
                         Export function declarations
   -------------------------------------------------------------------------- */ 
/* reserve mem on the heap */
ub4 *kmalloc_heap(ub4 size);

/* free reserved mem on the heap */
void kfree_heap(ub4 *addr);

/* module init function   */ 
bool heap_init_func(void);

/* module exit function   */ 
void heap_exit_func(void);
#endif
