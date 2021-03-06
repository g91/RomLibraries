#if !defined(GRANNY_VERSION_H)
// #include "header_preamble.h" keep this simple for the rc compiler
// ========================================================================
// $File: //jeffr/granny/rt/granny_version.h $
// $DateTime: 2007/01/16 10:17:13 $
// $Change: 14220 $
// $Revision: #103 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(EXPMACRO)
#define EXPMACRO
#endif

#if !defined(EXPGROUP)
#define EXPGROUP(GroupName)
#endif

EXPGROUP(VersionGroup)


#define ProductVersion "2.7.0.14"  EXPMACRO
#define ProductMajorVersion    2  EXPMACRO
#define ProductMinorVersion    7  EXPMACRO
#define ProductCustomization   0  EXPMACRO
#define ProductBuildNumber     14  EXPMACRO

#define ProductReleaseName release EXPMACRO
#define ProductReleaseString "release"
//#define ProductReleaseName final EXPMACRO
//#define ProductReleaseString "final"

#define ProductCompanyName "RAD Game Tools, Inc." EXPMACRO
#define ProductName "Granny" EXPMACRO
#define ProductCopyrightYears "1999-2006"
#define ProductCopyright "1999-2006 by RAD Game Tools, Inc., All Rights Reserved."  EXPMACRO
#define ProductTrademarks ProductName " is a registered trademark of " ProductCompanyName EXPMACRO
#define ProductComments "Who's your Granny?" EXPMACRO

#define ProductSupportAddress "granny3@radgametools.com" EXPMACRO
#define ProductSupportPage "www.radgametools.com/granny.html" EXPMACRO

// #include "header_postfix.h" keep this simple for the rc compiler
#define GRANNY_VERSION_H
#endif
