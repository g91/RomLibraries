/* ========================================================================
   $RCSfile: x86_granny_accelerated_deformers.inl,v $
   $Date: 2002/03/02 04:48:25 $
   $Revision: 1.1.1.1 $
   $Creator: Mike Sartain $
   (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
   ======================================================================== */

/*
 * Defines which allow us to inline various asm routines.
 *
 *  1) Debug builds don't pay attention to the __forceinline modifier.
 *  2) __declspec(naked) routines also won't inline.
 *  3) All of this has been fixed on vc7.
 *  4) Unfortunately, I'm assuming this needs to still build with vc6.
 *  5) Which sucks.
 *
 * To workaround this mess:
 *  1) For debug builds we define make function calls to naked routines.
 *  2) For retail builds we remove the naked decl and inline the routine.
 */
#ifdef _DEBUG
    #define DECL_NAKED                __declspec(naked)
    #define CALLINLINEFUNC(_func)     __asm call _func
#else
    #define DECL_NAKED
    #define CALLINLINEFUNC(_func)     _func()
#endif

/*
 * The entire existence of this file is to try and create some sort of
 *  asm macros for the Direct / Indirect routines. To accomplish this
 *  the file is included twice: once with DIRECT_ROUTINES defined and
 *  once without. We add a D or I to the function names appropriately.
 */
#undef FUNC_NAME
#ifdef DIRECT_ROUTINES
  #define FUNC_NAME(_func)          _func##D
#else
  #define FUNC_NAME(_func)          _func##I
#endif
