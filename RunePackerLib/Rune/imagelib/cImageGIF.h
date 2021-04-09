#ifndef CLASS_IMAGE_GIF_H
#define CLASS_IMAGE_GIF_H

#include "cimage.h"

#if IMAGELIB_SUPPORT_GIF

class cImageGIF : public cImage
{
public:
	bool Decode( FILE *fptr );
	bool Encode( FILE *fptr )	{ return false; }
};

#endif


#endif //CLASS_IMAGE_GIF_H