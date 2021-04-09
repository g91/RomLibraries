#ifndef CLASS_IMAGE_JPG_H
#define CLASS_IMAGE_JPG_H

#include "cimage.h"

#if IMAGELIB_SUPPORT_JPG

extern "C" {
#include "jpeg/jpeglib.h"
}

class cImageJPG : public cImage
{
public:	
	bool Decode( FILE *fptr );	
	bool Encode( FILE *fptr )	{ return false; }
};

#endif


#endif //CLASS_IMAGE_JPEG_H