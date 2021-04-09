#pragma once
/* jdiffos.h
 *
 * Machine/environment dependent definitions for jdiff.c and jpatch.c
 *
 * Add a section here for your specific machine/environment.
 * If you send me your section anf makefile, I will be glad to include it within
 * the next release of jdiff.
 *
 * off_t --> type to use for file offsets
 * P8zd  --> printf formatter for off_t in 8 positions (including the %)
 * PRIzd --> printf formatter for off_t without length (exluding the %)
 * define _JDIFF_LARGEFILE if off_t is 64 bits.
 */
#if defined(_CYGWIN_LARGEFILE)
  #define off_t  off_t
  #define PRIzd  "lld"
  #define P8zd   "%8lld"
  #define _JDIFF_LARGEFILE
#elif defined(_CYGWIN_SMALLFILE)
  #define off_t  long int
  #define PRIzd  "ld"
  #define P8zd   "%8ld"
#elif defined(_NOCYGWIN_LARGEFILE) 
  /* cygwin's win32 lib apparently prints ll as l, will hopefully change within the future */
  #define off_t  long long
  #define PRIzd  "llx-%llx"
  #define P8zd   "%8llx-%8llx" 
  #define _JDIFF_LARGEFILE
#elif defined(_NOCYGWIN_SMALLFILE)
  #define off_t  off_t
  #define PRIzd  "ld"
  #define P8zd   "%8ld"
#else
  /* Should suit most systems. 

     If off_t is 32 bits on your system, this may yield some 
     unharmfull warnings because of the _JDIFF_LARGEFILE.

     Additionally, zd may not be supported by your system, 
     in which case some ascii output may become garbled,
     without affecting however the generation of patch-files. 
  */
  #define off_t  off_t
  #define PRIzd  "zd"
  #define P8zd   "%8zd"
  #define _JDIFF_LARGEFILE
#endif /* LARGEFILE */



#define ESC     0xA7
#define MOD     0xA6
#define INS     0xA5
#define DEL		0xA4
#define EQL     0xA3
#define BKT     0xA2


/*
* Local definitions
*/

#define ulong unsigned long int // unsigned long
#define MCH_MAX 128             // Maximum size of matching table
#define EOB EOF - 1             // End-Of-Buffer constant

#define fopen   fopen
#define fclose  fclose
#define fseek   fseek
