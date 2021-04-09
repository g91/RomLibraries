#ifndef IMAGEDEF_H
#define IMAGEDEF_H

// DLLs in Windows should use the standard calling convention
#ifndef IMAGE_CALL
#  if defined(WIN32) || defined(_WIN32)
#    define IMAGE_CALL __stdcall
#  else
#    define IMAGE_CALL
#  endif
#endif

// Export functions from the DLL
// 若要輸出成 DLL 時, 需要定義 IMAGE_EXPORTS
#ifndef IMAGE_DECL
#  if defined(WIN32) || defined(_WIN32)
#    ifdef IMAGE_EXPORTS
#      define IMAGE_DECL __declspec(dllexport)
#    else
#      define IMAGE_DECL
#    endif
#  else
#    define IMAGE_DECL
#  endif
#endif

#define IMAGE_FUNCTION(ret) extern "C" IMAGE_DECL ret IMAGE_CALL

/*
#if defined(_AFXDLL)||defined(_USRDLL)
#	define DLL_EXP __declspec(dllexport)
#elif defined(_MSC_VER)&&(_MSC_VER<1200)
#	define DLL_EXP __declspec(dllimport)
#else
#	define DLL_EXP
#endif
*/

#ifndef min
#	define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#	define max(a,b) (((a)>(b))?(a):(b))
#endif

#ifdef WIN32
#include <windows.h>
//#include <tchar.h>
#endif

#include <stdio.h>
#include <math.h>

#ifndef WIN32

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   UINT;

typedef DWORD          COLORREF;
typedef unsigned int   HANDLE;
typedef void*          HRGN;

#define	BOOL bool
#define	TRUE true
#define	FALSE false

typedef struct tagRECT
{
	long    left;
	long    top;
	long    right;
	long    bottom;
} RECT;

typedef struct tagPOINT
{
	long  x;
	long  y;
} POINT;

typedef struct tagRGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;

#pragma pack(1)

typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;
	long       biWidth;
	long       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	long       biXPelsPerMeter;
	long       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPCOREHEADER {
	DWORD   bcSize;
	WORD    bcWidth;
	WORD    bcHeight;
	WORD    bcPlanes;
	WORD    bcBitCount;
} BITMAPCOREHEADER;

typedef struct tagRGBTRIPLE {
	BYTE    rgbtBlue;
	BYTE    rgbtGreen;
	BYTE    rgbtRed;
} RGBTRIPLE;

#pragma pack()

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define RGB2GRAY(r,g,b)		(((b)*117 + (g)*601 + (r)*306) >> 10)

#endif

#endif //IMAGEDEF_H