/*
 * cache_simulation - SRB F23
 *
 * Reads data from trace files
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include "../include/cache.h"

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

// function prototypes
int parseArgs (int argc, char* argv[], int* verbose, int* cType, char** filename, FILE** fp);

int main (int argc, char* argv[]) {

  float startTime = (float)clock()/CLOCKS_PER_SEC;

  char* filename = NULL;  // trace file
  FILE* fp = NULL;        // file pointer
  int ret = 0;            // return value
  int cType = DIR_MAP;    // cache_type
  int verbose = 0;        // verbosity flag

  if ((ret = parseArgs(argc, argv, &verbose, &cType, &filename, &fp)) != 0) {
    printf("usage: ./cache_simulation [-v] -t cache_type filename\n");
    exit(ret);
  }
  
  char ch = '\0';
  unsigned int addr = 0;
  cache_t* dm_cache;
  cache_t* tw_cache;
  cache_t* fw_cache;
  cache_t* fa_cache;
  switch (cType) {
    case DIR_MAP:
      dm_cache = init_cache(64, 1);
      if (dm_cache == NULL) exit(99);
      break;
    case TWO_WAY:
      tw_cache = init_cache(32, 2);
      if (tw_cache == NULL) exit(99);
      break;
    case FOUR_WAY:
      fw_cache = init_cache(16, 4);
      if (fw_cache == NULL) exit(99);
      break;
    case FULL_ASS:
      fa_cache = init_cache(1, 64);
      if (fa_cache == NULL) exit(99);
      break;
    case ALL:
      dm_cache = init_cache(64, 1);
      tw_cache = init_cache(32, 2);
      fw_cache = init_cache(16, 4);
      fa_cache = init_cache(1, 64);
      if (dm_cache == NULL || tw_cache == NULL || fw_cache == NULL || fa_cache == NULL) exit(99);
      break;
    default:
      printf("invalid cache_type: %d (must be range 0-4)\n", cType);
      exit(44);
  }

  // direct match stats
  int dm_hits = 0;
  int dm_misses = 0;

  // two-way match stats
  int tw_hits = 0;
  int tw_misses = 0;

  // four-way match stats
  int fw_hits = 0;
  int fw_misses = 0;

  // full-ass match stats
  int fa_hits = 0;
  int fa_misses = 0;

  int count = 0x1;

  // header......
  printf("%s\n", filename);
  printf("verbosity %d\n", verbose);
  if (verbose) {
    switch (cType) {
      case DIR_MAP:
        printf("tagmask %#x\n", DM_TAGMASK);
        printf("setmask %#x\n", DM_SETMASK);
        printf("mode = direct mapped\n");
        printf("64 blocks, 16 bytes in block; 64 sets, 1 line per set\n");
        break;
      case TWO_WAY:
        printf("tagmask %#x\n", TW_TAGMASK);
        printf("setmask %#x\n", TW_SETMASK);
        printf("mode = 2-way set associative\n");
        printf("64 blocks, 16 bytes in block; 32 sets, 2 line per set\n");
        break;
      case FOUR_WAY:
        printf("tagmask %#x\n", FW_TAGMASK);
        printf("setmask %#x\n", FW_SETMASK);
        printf("mode = 4-way set associative\n");
        printf("64 blocks, 16 bytes in block; 16 sets, 4 line per set\n");
        break;
      case FULL_ASS:
        printf("tagmask %#x\n", FA_TAGMASK);
        printf("setmask %#x\n", FA_SETMASK);
        printf("mode = fully associative\n");
        printf("64 blocks, 16 bytes in block; 1 set, 64 line per set\n");
        break;
      default: 
        printf("wtf\n");
        exit(77);
    }
  }
  while (fscanf(fp, "%c %x\n", &ch, &addr) != EOF) {
    switch (cType) {
      case DIR_MAP:
        cache_add(verbose, DIR_MAP, addr, count, dm_cache, &dm_hits, &dm_misses);
        break;
      case TWO_WAY:
        cache_add(verbose, TWO_WAY, addr, count, tw_cache, &tw_hits, &tw_misses);
        break;
      case FOUR_WAY:
        cache_add(verbose, FOUR_WAY, addr, count, fw_cache, &fw_hits, &fw_misses);
        break;
      case FULL_ASS:
        cache_add(verbose, FULL_ASS, addr, count, fa_cache, &fa_hits, &fa_misses);
        break;
      case ALL:
        cache_add(verbose, DIR_MAP, addr, count, dm_cache, &dm_hits, &dm_misses);
        cache_add(verbose, TWO_WAY, addr, count, tw_cache, &tw_hits, &tw_misses);
        cache_add(verbose, FOUR_WAY, addr, count, fw_cache, &fw_hits, &fw_misses);
        cache_add(verbose, FULL_ASS, addr, count, fa_cache, &fa_hits, &fa_misses);
        break;
      default:
        printf("wtf\n");
        exit(77);
    }
    count++;
  }

  switch (cType) {
    case DIR_MAP:
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", dm_hits, dm_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)dm_hits/(count-1), 1-(float)dm_hits/(count-1));
      printf("-----------------------------------------------\n");
      printf("-----------------------------------------------\n");
      free_cache(dm_cache, 64, 1);
      break;
    case TWO_WAY:
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", tw_hits, tw_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)tw_hits/(count-1), 1-(float)tw_hits/(count-1));
      printf("-----------------------------------------------\n");
      free_cache(tw_cache, 32, 2);
      break;
    case FOUR_WAY: 
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", fw_hits, fw_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)fw_hits/(count-1), 1-(float)fw_hits/(count-1));
      printf("-----------------------------------------------\n");
      free_cache(fw_cache, 16, 4);
      break;
    case FULL_ASS:
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", fa_hits, fa_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)fa_hits/(count-1), 1-(float)fa_hits/(count-1));
      printf("-----------------------------------------------\n");
      free_cache(fa_cache, 1, 64);
      break;
    case ALL:
      printf("Direct Managment Cache\n");
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", dm_hits, dm_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)dm_hits/(count-1), 1-(float)dm_hits/(count-1));
      printf("-----------------------------------------------\n");
      printf("-----------------------------------------------\n");

      printf("Two-Way Set Associative\n");
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", tw_hits, tw_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)tw_hits/(count-1), 1-(float)tw_hits/(count-1));
      printf("-----------------------------------------------\n");
      printf("-----------------------------------------------\n");

      printf("Four-Way Set Associative\n");
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", fw_hits, fw_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)fw_hits/(count-1), 1-(float)fw_hits/(count-1));
      printf("-----------------------------------------------\n");
      printf("-----------------------------------------------\n");

      printf("Fully Associative\n");
      printf("-----------------------------------------------\n");
      printf("Hits: %d; misses: %d; addresses: %u\n", fa_hits, fa_misses, count-1);
      printf("%1.6f hr %1.6f mr\n", (float)fa_hits/(count-1), 1-(float)fa_hits/(count-1));
      printf("-----------------------------------------------\n");
      free_cache(fa_cache, 1, 64);
      free_cache(dm_cache, 64, 1);
      free_cache(tw_cache, 32, 2);
      free_cache(fw_cache, 16, 4);
      break;
    default:
      printf("wtf\n");
  }


  fclose(fp);

  float endTime = (float)clock()/CLOCKS_PER_SEC;
  printf("Time Elapsed: %f\n", endTime - startTime);
  return ret;
  
}

int parseArgs (const int argc, char* argv[], int* verbose, int* cType, char** filename, FILE** fp) {


  int c;
  char* ca_type = NULL;
  
  while ((c = getopt(argc, argv, "vt:")) != -1) {
    switch (c) {
      case 'v':
        *verbose = 1;
        break;
      case 't':
        ca_type = optarg;
        break;
      case '?':
        if (optopt == 't')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort();
    }
  }

  if (ca_type != NULL) *cType = atoi(ca_type);

  if (*cType == ALL && *verbose) {
    printf("cannot use verbose output for all cache files simultaneously\n");
    return 4;
  }

  if (argc - optind != 1) {
    return 3;
  }
  *filename = argv[optind];

  if (strcmp(*filename, "-") == 0) {
    *fp = stdin;
    return 0;
  } else {
    *fp = fopen(*filename, "r");
    if (*fp == NULL) {
      p_error("Error opening file");
      return 2;
    }
  }
  
  return 0;
}
