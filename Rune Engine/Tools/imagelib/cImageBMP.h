#ifndef CLASS_IMAGE_BMP_H
#define CLASS_IMAGE_BMP_H

#include "cimage.h"

#if IMAGELIB_SUPPORT_BMP

#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

#if !defined(BI_RGB)
	#define BI_RGB				0L
#endif
#if !defined(BI_RLE8)
	#define BI_RLE8				1L
#endif
#if !defined(BI_RLE4)
	#define BI_RLE4				2L
#endif

#if !defined(ALIGN_LONG)
	#define ALIGN_LONG(x)		((((x) + 3) >>2 ) <<2 )
#endif

class cImageBMP : public cImage
{
public:
	bool Decode( FILE *fptr );
	bool Encode( FILE *fptr )	{ return false; }
};

#endif


#endif //CLASS_IMAGE_BMP_H