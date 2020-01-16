/* KalioOS (C) 2020 Pranav Bagur */

#include "if/heap.h"

/* -------------------------------------------------------------------------- 
                         Constants and types
   -------------------------------------------------------------------------- */ 
ub4     tub_sizes[] = {32, 128, 256, 512, 1024, 4096};
heap_t *heap_glob;
ub4     bundle_size;

/* -------------------------------------------------------------------------- 
                         Static functions
   -------------------------------------------------------------------------- */ 
/* 
 * SF: add_bundles - Add more bundles to the heap
 * 
 * ARGS :-
 *   nbundles - # bundles to add
 *
 * RET
 *   true if successful
 */
static bool
add_bundles(ub4 nbundles)
{
  /* Allocate a page */
  ub4       idx;
  bundle_t *bundle;
  ub4      *bundle_mem; 

  if ((heap_glob->total_bundles + nbundles) > MAX_BUNDLES)
    return false;

  bundle_mem = (ub4 *)kmalloc(PAGE_SIZE);
  memset((ub1 *)bundle_mem, PAGE_SIZE, 0);
  bundle = (bundle_t *)bundle_mem;

  /* Carve out nbundles from the allocated page */
  for (idx = 0; idx < nbundles; idx++, bundle++) {
    list_init(&bundle->chunks_bundle);
    bundle->chunks_count_bundle  = 0;
    bundle->chunks_in_use_bundle = 0;
    bundle->mem_bundle           = NULL; /* Lazy allocation */

    list_add_tail(&heap_glob->free_bundles_heap, &bundle->link_heap_bundle);
    heap_glob->free_bundles_count++;

    heap_glob->total_bundles++;
  }

  return true;
}

/* 
 * SF: init_tub - Initialize tub
 * 
 * ARGS :-
 *   tub - tub to initialize
 *   sz  - chunk size
 *
 * RET
 */
static void
init_tub(tub_t *tub, ub4 sz)
{
  list_init(&tub->bundles_tub);
  tub->bundles_count_tub = 0;

  list_init(&tub->avl_chunks_tub);
  tub->avl_chunks_count_tub = 0;

  tub->total_in_use_tub = 0;
  tub->total_tub = 0;

  tub->size_tub = sz;
}

/* 
 * SF: grow_tub - add free chunks to the tub
 * 
 * ARGS :-
 *   tub - tub to grow
 *
 * RET
 *   true if we have a bundle to grab
 */
static bool
grow_tub(tub_t *tub)
{
  list     *item;
  bundle_t *bundle;
  ub4       chunk_sz;
  ub4       cur_off;

  if (!heap_glob->free_bundles_count && 
      !add_bundles(GROW_BUNDLES_LIMIT))
      return false;

  item  = list_remove_front(&heap_glob->free_bundles_heap);
  heap_glob->free_bundles_count--;

  bundle = list_entry(item, bundle_t, link_heap_bundle);
  if (!bundle->mem_bundle)
    bundle->mem_bundle = (ub4 *)kmalloc(bundle_size);

  list_add_tail(&heap_glob->in_use_bundles_heap, &bundle->link_heap_bundle);
  heap_glob->in_use_bundles_count++;

  list_add_tail(&tub->bundles_tub, &bundle->link_tub_bundle);
  tub->bundles_count_tub++;

  chunk_sz = (sizeof(chunk_t) + tub->size_tub);
  cur_off  = 0;

  while((cur_off + chunk_sz) <= bundle_size) {
    chunk_t *chunk = (chunk_t *)((ub4)bundle->mem_bundle + cur_off);
    chunk->magic_chunk      = MAGIC_CHUNK;
    chunk->in_use_chunk     = false;
    chunk->size_chunk       = tub->size_tub; 
    chunk->bp_tub_chunk     = tub;
    chunk->bp_bundle_chunk  = bundle;

    list_add_tail(&bundle->chunks_bundle, &chunk->link_bundle_chunk);
    bundle->chunks_count_bundle++;

    list_add_tail(&tub->avl_chunks_tub, &chunk->link_avl_chunk);
    tub->avl_chunks_count_tub++;

    tub->total_tub++;
    cur_off += chunk_sz;
  }

#ifdef DEBUG
  printk("finished growing tub ");
  printk_num(tub->size_tub);
  printk(", ");
  printk_num(tub->avl_chunks_count_tub);
  printk("\n");
#endif
  return true;
}

/* 
 * SF: shrink_tub - free chunks back to the pool
 * 
 * ARGS :-
 *   tub    - tub to shrink
 *   bundle - bundle reserved for the tub to be released
 *            back to the pool
 *
 * RET
 */
static void
shrink_tub(tub_t *tub, bundle_t *bundle)
{
  list *item;

  ASSERT((bundle->chunks_in_use_bundle == 0));
  while (bundle->chunks_count_bundle) {
    chunk_t *chunk;

    item = list_remove_front(&bundle->chunks_bundle);
    bundle->chunks_count_bundle--;

    chunk = list_entry(item, chunk_t, link_bundle_chunk);
    ASSERT(!chunk->in_use_chunk);
    ASSERT((chunk->magic_chunk == MAGIC_CHUNK));

    list_remove(&tub->avl_chunks_tub, &chunk->link_avl_chunk);
    tub->avl_chunks_count_tub--;
    tub->total_tub--;

    chunk->bp_bundle_chunk = NULL;
    chunk->bp_tub_chunk    = NULL;
  }
  
  list_remove(&tub->bundles_tub, &bundle->link_tub_bundle);
  tub->bundles_count_tub--;

  list_remove(&heap_glob->in_use_bundles_heap, &bundle->link_heap_bundle);
  heap_glob->in_use_bundles_count--;

  /* 
   * We don't have a mechanism to release mem_bundle
   * Just push this bundle to the front so that this
   * is picked up next
   */
  memset((ub1 *)bundle->mem_bundle, bundle_size, 0);
  list_add_tail(&heap_glob->free_bundles_heap, &bundle->link_heap_bundle);
  heap_glob->free_bundles_count++;

#ifdef DEBUG
  printk("finished shrinking the tub ");
  printk_num(tub->size_tub);
  printk(": ");
  printk_num(tub->avl_chunks_count_tub);
  printk("\n");
#endif
}

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 

/* 
 * EF: kmalloc_heap - allocate heap memory
 * 
 * ARGS :-
 *   sz - required size
 *
 * RET
 *   address of allocated memory
 */
ub4 *kmalloc_heap(ub4 sz)
{
  ub4       tub_idx = 0;
  tub_t    *tub;
  list     *item;
  chunk_t  *chunk;
  ub4      *addr;
  bundle_t *bundle;

  ASSERT((sz < tub_sizes[N_TUBS - 1]));

  // find the right tub to pick chunk up from
  while(sz > tub_sizes[tub_idx]) 
    tub_idx++;

  tub = &heap_glob->tubs[tub_idx];
  if (!tub->avl_chunks_count_tub && 
      !grow_tub(tub))
    return NULL;

  ASSERT(tub->avl_chunks_count_tub);

  item  = list_remove_front(&tub->avl_chunks_tub);
  tub->avl_chunks_count_tub--;

  chunk = list_entry(item, chunk_t, link_avl_chunk);
  ASSERT((chunk->in_use_chunk == false));
  ASSERT((chunk->magic_chunk == MAGIC_CHUNK));
  addr  = (ub4 *)((ub4)chunk + sizeof(chunk_t));
  memset((ub1 *)addr, chunk->size_chunk, 0);
 
  chunk->in_use_chunk = true;
  tub->total_in_use_tub++;

  bundle = (bundle_t *)chunk->bp_bundle_chunk;
  bundle->chunks_in_use_bundle++;

#ifdef DEBUG
  printk("finished malloc ");
  printk_num(tub->size_tub);
  printk(": ");
  printk_num(tub->avl_chunks_count_tub);
  printk(": ");
  printk_num((ub4)chunk);
  printk(": ");
  printk_num((ub4)addr);
  printk("\n");
#endif

  return addr;
}

/* 
 * EF: kfree_heap - free heap memory
 * 
 * ARGS :-
 *   addr - address of allocated memory
 *
 * RET
 */
void
kfree_heap(ub4 *addr)
{
  chunk_t  *chunk;
  tub_t    *tub; 
  bundle_t *bundle;

  chunk = (chunk_t *) ((ub4)addr - sizeof(chunk_t));
  ASSERT(chunk->magic_chunk == MAGIC_CHUNK);
  ASSERT(chunk->in_use_chunk);
  
  chunk->in_use_chunk = false;
  tub = (tub_t *)chunk->bp_tub_chunk;
  list_add_tail(&tub->avl_chunks_tub, &chunk->link_avl_chunk);
  tub->avl_chunks_count_tub++;
  tub->total_in_use_tub--;

  bundle = (bundle_t *)chunk->bp_bundle_chunk;
  bundle->chunks_in_use_bundle--;

  /* TODO Need some watermark algo here */
  if (!bundle->chunks_in_use_bundle)
    shrink_tub(tub, bundle);

  
#ifdef DEBUG
  printk("finished free ");
  printk_num(tub->size_tub);
  printk(", ");
  printk_num(tub->avl_chunks_count_tub);
  printk(", ");
  printk_num((ub4)chunk);
  printk(", ");
  printk_num((ub4)addr);
  printk("\n");
#endif
}

/* 
 * EF: heap_init_func - module init function
 * 
 * ARGS :-
 *
 * RET - TRUE iff successful
 */
bool
heap_init_func(void)
{
  ub4 idx = 0;
  ub4 sz  = sizeof(*heap_glob);

  bundle_size = (tub_sizes[N_TUBS - 1] + sizeof(chunk_t));

  heap_glob = (heap_t *)kmalloc(sz);
  memset((ub1 *)heap_glob, sz, 0);

  /* No point handling mem alloc failures, in any */
  ASSERT(heap_glob);
  list_init(&heap_glob->free_bundles_heap);
  heap_glob->free_bundles_count = 0;

  list_init(&heap_glob->in_use_bundles_heap);
  heap_glob->in_use_bundles_count = 0;

  heap_glob->total_bundles = 0;

  for (idx = 0; idx < N_TUBS; idx++)
    init_tub(&heap_glob->tubs[idx], tub_sizes[idx]);
 
  if (!add_bundles(INIT_BUNDLES))
    return false;

  printk_system("Initialized heap..");
  return true;
}

/* 
 * EF: heap_exit_func - module exit function
 * 
 * ARGS :-
 *
 * RET 
 */
void
heap_exit_func(void)
{
}
