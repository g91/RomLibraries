#ifndef CLASS_IMAGE_PNG_H
#define CLASS_IMAGE_PNG_H

#include "cimage.h"

#if IMAGELIB_SUPPORT_PNG

extern "C" {
#include "png/png.h"
}

class cImagePNG : public cImage
{
public:
	bool Decode( FILE *fptr );
	bool Encode( FILE *fptr )	{ return false; }
};

#endif


#endif //CLASS_IMAGE_PNG_H