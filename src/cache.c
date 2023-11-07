/*
 * cache.c - SRB 23F
 * 
 * See header file for more information about functions
 */

#include <stdlib.h>
#include <stdio.h>

#include "cache.h"

// helpful pre-processor macros 
#define p_error(msg) fprintf(stderr, "%s\n", msg)

#define DM_TAGMASK 0xfffffc00
#define DM_SETMASK 0x000003f0

#define TW_TAGMASK 0xfffffe00
#define TW_SETMASK 0x000001f0

#define FW_TAGMASK 0xffffff00
#define FW_SETMASK 0x000000f0

#define FA_TAGMASK 0xfffffff0
#define FA_SETMASK 0x00000000

// cache-type enum
enum cache_type {DIR_MAP, TWO_WAY, FOUR_WAY, FULL_ASS, ALL};



// cache structures
typedef struct line {
  int isValid;
  int tag;
  int lastTouched;
} line_t;

typedef struct set {
  line_t** lines;
} set_t;

typedef struct cache {
  set_t** sets;
} cache_t;

/* init_cache */
cache_t* init_cache (const int sets, const int linesPerSet) {
  cache_t* cache = malloc(sizeof(cache_t));
  if (cache == NULL) return NULL;
  cache->sets = malloc(sets*sizeof(set_t*));
  if (cache->sets == NULL) return NULL;
  for (int i = 0; i < sets; i++) {
    cache->sets[i] = malloc(sizeof(set_t));
    set_t* set = cache->sets[i];
    if (set == NULL) return NULL;
    set->lines = malloc(linesPerSet*sizeof(line_t*));
    if (set->lines == NULL) return NULL;
    for (int j = 0; j < linesPerSet; j++) {
      line_t* line = malloc(sizeof(line_t));
      line->isValid = 0;
      line->lastTouched = 0;
      line->tag = 0;
      set->lines[j] = line;
    }
  }
  return cache;
}

/* cache_add */
void cache_add(int verbose, int type, const unsigned int addr, const int count, cache_t* cache, int* hits, int* misses) {
  unsigned int tag;     // which tag?
  unsigned int set_ID;  // which set?
  int no_lines = 1;     // number of lines per set
  // calculate tag and set_ID from address
  switch (type) {
    case DIR_MAP:
      tag = (addr & DM_TAGMASK) >> 10;
      set_ID = (addr & DM_SETMASK) >> 4;
      break;
    case TWO_WAY:
      tag = (addr & TW_TAGMASK) >> 9;
      set_ID = (addr & TW_SETMASK) >> 4;
      no_lines = 2;
      break;
    case FOUR_WAY:
      tag = (addr & FW_TAGMASK) >> 8;
      set_ID = (addr & FW_SETMASK) >> 4;
      no_lines = 4;
      break;
    case FULL_ASS:
      tag = (addr & FA_TAGMASK) >> 4;
      set_ID = 0x0;
      no_lines = 64;
      break;
    default:
      p_error("invalid cache type");
      exit(66);
  }
  if (verbose) {
    printf("D%08d: addr 0x%08x looking for tag 0x%06x in set 0x%02x\n", count, addr, tag, set_ID);
    printf("\nState of Set 0x%02x:\n", set_ID);
  }
  set_t* set = cache->sets[set_ID];
  unsigned int least_recently_touched = 0;      // index of least recently touched line
  int hit_line = -1;                            // index of hit line
  line_t* line;                                 // current line
  for (unsigned int i = 0; i < no_lines; i++) {
    line = set->lines[i];
    if (verbose) printf("line 0x%02x V=%d tag 0x%07x last_touch=%07d\n", i, line->isValid, line->tag, line->lastTouched);
    if (line->isValid == 1 && line->tag == tag) {
      hit_line = i;
      break;
    }
    if (line->lastTouched < set->lines[least_recently_touched]->lastTouched) {
      least_recently_touched = i;
    }
  }
  if (hit_line != -1) {
    if (verbose) printf("\nFound it in line 0x%x.  Hit! Updating last_touch to %d\n", (unsigned int)hit_line, count);
    set->lines[hit_line]->lastTouched = count;
    (*hits)++;
  }
  else if (set->lines[least_recently_touched]->isValid == 0) {
    line = set->lines[least_recently_touched];
    if (verbose) printf("\nMiss! Found empty line 0x%x; adding block there; setting last_touch to %d\n", least_recently_touched, count);
    line->lastTouched = count;
    line->tag = tag;
    line->isValid = 1;
    (*misses)++;
  } else {
    if (verbose) printf("\nMiss! Evicting line 0x%x; adding block there; setting last_touch to %d\n", least_recently_touched, count);
    line = set->lines[least_recently_touched];
    line->lastTouched = count;
    line->tag = tag;
    (*misses)++;
  }
  if (verbose) printf("-----------------------------------------------\n");
}



/* free_cache */
void free_cache(cache_t* cache, const int sets, const int linesPerSet) {
  if (cache == NULL || sets < 1 || linesPerSet < 1) return;
  for (int i = 0; i < sets; i++) {
    if (cache->sets[i] == NULL) continue;
    for (int j = 0; j < linesPerSet; j++) {
      if (cache->sets[i]->lines == NULL) continue;
      free(cache->sets[i]->lines[j]);
    }
    if (cache->sets[i]->lines != NULL) free(cache->sets[i]->lines);
    free(cache->sets[i]);
  }
  free(cache->sets);
  free(cache);
}
