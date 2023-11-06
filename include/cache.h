/*
 * SRB - FALL 2023
 *
 * cache.h
 * 
 * library for cache simulation
 * 
*/

#ifndef __CACHE_H
#define __CACHE_H

#include <stdlib.h>

/*
 * cache structure
*/
typedef struct cache cache_t;

/*
 * init_cache 
 * 
 * initializes a fresh cache
 * 
 * params
 *    sets - number of sets in the cache
 *    linesPerSet - number of lines per set
 * 
 * we assume:
 *    sets and linesPerSet params are valid
 * 
 * we guarantee:
 *    enough memory is allocated for the new set
 * 
 * we return:
 *    pointer to initialized cache
 * 
 * caller must:
 *    call free_cache on returned cache
 */
cache_t* init_cache (const int sets, const int linesPerSet);
/* 
 * cache_add
 * 
 * add an address to the cache (any kind)
 *    uses a LRU replacement scheme
 * 
 * params:
 *    verbose - verbosity flag
 *    type - type of cache (to specify bit mask)
 *    addr - 32 bit address
 *    count - number of adds to the cache
 *    cache - pointer to the cache
 *    hits - pointer to number of hits
 *    misses - pointer to number of misses
 * 
 * we assume:
 *    cache is valid, int pointers are valid, type is in correct range (0-3)
 * 
 * we guarantee:
 *    address is added to cache and least recently touched data is evicted
 *    if cache_type is invalid, exits with code 66
 * 
 * we return:
 *    nothing, printed output to stdout if verbose flag enabled
 *    
 */
void cache_add(int verbose, int type, const unsigned int addr, const int count, cache_t* cache, int* hits, int* misses);

/*
 * free_cache
 * 
 * frees cache from memory
 * 
 * params:
 *    cache - cache to free
 *    sets - number of sets in the cache
 *    linesPerSet - number of lines per set
 * 
 * we assume:
 *    sets and linesPerSet params are valid
 * 
 * we guarantee:
 *    all memory associated with cache is freed
 * 
 * we return:
 *    nothing
 */

void free_cache(cache_t* cache, const int sets, const int linesPerSet);

#endif
