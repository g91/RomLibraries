#include <setjmp.h>
#include "cImageJPG.h"

#if IMAGELIB_SUPPORT_JPG

#define JPEG_BUFFER_SIZE			4096

struct JPGInternalStruct {
	struct {
		jpeg_error_mgr mgr;
		jmp_buf setjmp_buffer;
	} error_mgr;

	FILE *io;
	BYTE buffer[JPEG_BUFFER_SIZE];
};

void JPEG_init_source(j_decompress_ptr cinfo);
boolean JPEG_fill_input_buffer(j_decompress_ptr cinfo);
void JPEG_skip_input_data(j_decompress_ptr cinfo, long num_bytes);
void JPEG_term_source(j_decompress_ptr cinfo);
void JPEG_error_exit(j_common_ptr cinfo);
void JPEG_emit_message(j_common_ptr cinfo, int msg_level);

bool cImageJPG::Decode( FILE *fptr )
{
	if ( fptr == NULL )	return false;

	fseek( fptr, 0, SEEK_SET );
	// set up internal information
	JPGInternalStruct is;
	is.io = fptr;

	// initialize the source manager
	jpeg_source_mgr mgr;
	mgr.bytes_in_buffer = 0;
	mgr.next_input_byte = NULL;
	mgr.init_source       = JPEG_init_source;
	mgr.fill_input_buffer = JPEG_fill_input_buffer;
	mgr.skip_input_data   = JPEG_skip_input_data;
	mgr.resync_to_restart = jpeg_resync_to_restart;  // use default
	mgr.term_source       = JPEG_term_source;

	// initialize decompressor
	jpeg_decompress_struct cinfo;
	jpeg_create_decompress(&cinfo);
	cinfo.client_data = &is;

	cinfo.err = jpeg_std_error(&is.error_mgr.mgr);
	is.error_mgr.mgr.error_exit = JPEG_error_exit;

	if (setjmp(is.error_mgr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	cinfo.src = &mgr;
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	// do we support the number of color components?
	if (cinfo.output_components != 1 && cinfo.output_components != 3) {
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	// make a one-row-high sample array that will go away when done with image
	int row_stride = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	// allocate image
	unsigned width  = cinfo.output_width;
	unsigned height = cinfo.output_height;	
	Create( width, height, 24, IMAGELIB_FORMAT_JPG );

	BYTE *pixels = GetPixels();
	//memset(pixels, 0, width * height * 3);

	// read the scanlines
	bool finished = true;
	while (cinfo.output_scanline < height) {
		int num_rows = jpeg_read_scanlines(&cinfo, buffer, 1);
		if (num_rows == 0) {
			finished = false;
			break;
		}

		// copy scanline into pixel buffer
		if (cinfo.output_components == 1) {        // greyscale
			BYTE* in = (BYTE*)(*buffer);
			for (unsigned i = 0; i < width * num_rows; ++i) {
				*pixels++ = *in; // red
				*pixels++ = *in; // green
				*pixels++ = *in; // blue
				++in;
			}
		} else if (cinfo.output_components == 3) { // RGB
			RGB2BGR(pixels, (*buffer), num_rows * width);
			//memcpy( pixels, (*buffer), num_rows * width );
			pixels += num_rows * width * 3;
		}			
	}

	// finish up
	if (finished) {
		jpeg_finish_decompress(&cinfo);
	}
	jpeg_destroy_decompress(&cinfo);

	return true;
}

void JPEG_init_source(j_decompress_ptr cinfo)
{
	// no initialization required
}	

boolean JPEG_fill_input_buffer(j_decompress_ptr cinfo)
{
	JPGInternalStruct* is = (JPGInternalStruct*)(cinfo->client_data);
	cinfo->src->bytes_in_buffer = fread(is->buffer, 1, JPEG_BUFFER_SIZE, is->io);
	cinfo->src->next_input_byte = is->buffer;
	return (cinfo->src->bytes_in_buffer != 0);
}


void JPEG_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	//JPGInternalStruct* is = (JPGInternalStruct*)(cinfo->client_data);
	if (num_bytes > 0) {
		while (num_bytes > (long)cinfo->src->bytes_in_buffer) {
			num_bytes -= (long)cinfo->src->bytes_in_buffer;
			JPEG_fill_input_buffer(cinfo);
		}
		cinfo->src->next_input_byte += (size_t)num_bytes;
		cinfo->src->bytes_in_buffer -= (size_t)num_bytes;
	}
}


void JPEG_term_source(j_decompress_ptr cinfo)
{
	// nothing to do here...
}


void JPEG_error_exit(j_common_ptr cinfo)
{
	JPGInternalStruct* is = (JPGInternalStruct*)(cinfo->client_data);
	longjmp(is->error_mgr.setjmp_buffer, 1);
}


void JPEG_emit_message(j_common_ptr /*cinfo*/, int /*msg_level*/) 
{
	// ignore error messages
}

#endif //IMAGELIB_SUPPORT_JPG