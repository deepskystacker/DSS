/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeJPEG.cpp
Owner:	rich@woodbridgeinternalmed.com
Purpose:	Decode a JPEG file into a frame array.
			The only exported function is:
			bool DecodeJPEG( CDataSourceABC &ds, CFrameArray &arrFrames, int &nWidth, int &nHeight )
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
#include <jpeglib.h>
#include <jerror.h>
#include "ImgLibInt.h"
#include "ImgLib.h"

#include "DibLoader.h"
#include "Config.h"
#include "CompatibleDC.h"

#ifdef TRACING
	#define ITRACE TRACEA
#else	//	TRACING
	#define ITRACE 
#endif	//	TRACING

#ifdef IMGLIB_JPG
/////////////////////////////////////////////////////////////
// Here are the JPEG error handling routines. 
// On a error an int is thrown.
/////////////////////////////////////////////////////////////

static void win_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	//jpeg_error_mgr* myerr = (jpeg_error_mgr*) cinfo->err;

#ifdef _DEBUG
	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Output it */
	TRACEA( buffer );
#else
	UNREFERENCED_PARAMETER( cinfo );
#endif
	throw 1;
}



static void win_emit_message (j_common_ptr cinfo, int msg_level)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	//jpeg_error_mgr* myerr = (jpeg_error_mgr*) cinfo->err;
#ifdef _DEBUG
	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Send it to stderr, adding a newline */
	ITRACE(buffer);
#else
	UNREFERENCED_PARAMETER( cinfo );
#endif

	/* Return control to the setjmp point */
	if (msg_level < 0)
		throw 1;
}

static void win_output_message (j_common_ptr cinfo)
{
#ifdef _DEBUG
	char buffer[JMSG_LENGTH_MAX];
	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Send it to stderr, adding a newline */
	ITRACE(buffer);
#else
	UNREFERENCED_PARAMETER( cinfo );
#endif
}

static void win_format_message (j_common_ptr /* cinfo */, char * /* buffer */)
{
}

static void win_reset_error_mgr (j_common_ptr cinfo)
{
	cinfo->err->num_warnings = 0;
	/* trace_level is not reset since it is an application-supplied parameter */
	cinfo->err->msg_code = 0;	/* may be useful as a flag for "no error" */
}

static jpeg_error_mgr * jpeg_win_error (jpeg_error_mgr * err)
{
  err->error_exit = win_error_exit;
  err->emit_message = win_emit_message;
  err->output_message = win_output_message;
  err->format_message = win_format_message;
  err->reset_error_mgr = win_reset_error_mgr;

  err->trace_level = 0;		/* default = no tracing */
  err->num_warnings = 0;	/* no warnings emitted yet */
  err->msg_code = 0;		/* may be useful as a flag for "no error" */

  /* Initialize message table pointers */
  err->jpeg_message_table = 0;
  err->last_jpeg_message = 0;

  err->addon_message_table = NULL;
  err->first_addon_message = 0;	/* for safety */
  err->last_addon_message = 0;

  return err;
}

/////////////////////////////////////////////////////////////
// JPEG DataSourceABS source routines. 
/////////////////////////////////////////////////////////////

typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */

	CDataSourceABC* ds;		// The data source
	JOCTET * buffer;		// start of buffer */
	bool start_of_file;	/* have we gotten any data yet? */
	int buffer_size;	// Available data at initialization
} win_source_mgr;

typedef win_source_mgr * win_src_ptr;

#define INPUT_BUF_SIZE  0x8000	/* choose an efficiently fread'able size */

static void win_init_source (j_decompress_ptr cinfo)
{
  win_src_ptr src = (win_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = true;
  src->buffer_size = src->ds->GetSize();	// Do this here for efficiency
}


static boolean win_fill_input_buffer (j_decompress_ptr cinfo)
{
	win_src_ptr src = (win_src_ptr) cinfo->src;
	int nbytes;
 	
	// Do not cause an EOF, only read what is available.
	nbytes = min(INPUT_BUF_SIZE, src->buffer_size - src->ds->GetCurrentPos());
	if (!src->ds->ReadBytes(src->buffer, nbytes))
		nbytes = 0;

	if (nbytes <= 0) 
	{
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = false;

	return TRUE;
}


static void win_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	win_src_ptr src = (win_src_ptr) cinfo->src;

	/* Just a dumb implementation for now.  Could use fseek() except
	 * it doesn't work on pipes.  Not clear that being smart is worth
	 * any trouble anyway --- large skips are infrequent.
	 */
	if (num_bytes > 0)
	{
		if (num_bytes < (long)src->pub.bytes_in_buffer)
		{
			src->pub.bytes_in_buffer -= num_bytes;
			src->pub.next_input_byte += num_bytes;
			return;
		}
		num_bytes -= (long)src->pub.bytes_in_buffer;

		if (num_bytes > 0)
		{
			src->ds->SetRelativePos(num_bytes);
		}
		win_fill_input_buffer(cinfo);
	}
}


static void win_term_source (j_decompress_ptr /* cinfo */)
{
  /* no work necessary here */
}


static void jpeg_win_src (j_decompress_ptr cinfo, CDataSourceABC* ds)
{
	win_src_ptr src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_win_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
	if (cinfo->src == NULL) /* first time for this JPEG object? */
	{
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			 sizeof(win_source_mgr));
		src = (win_src_ptr) cinfo->src;
		src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			INPUT_BUF_SIZE * sizeof(JOCTET));
	}

	src = (win_src_ptr) cinfo->src;
	src->pub.init_source = win_init_source;
	src->pub.fill_input_buffer = win_fill_input_buffer;
	src->pub.skip_input_data = win_skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = win_term_source;
	src->ds = ds;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

bool DecodeJPEG( CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size )
{
	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	/* More stuff */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	GS::CDIB * pDib = NULL;

	// Setup the jpeg error routines to use ours
	cinfo.err = jpeg_win_error(&jerr);
	// Initialize the decompression object
	jpeg_create_decompress(&cinfo);

	// Specify the data source
	jpeg_win_src(&cinfo, &ds);

	// Read the file paramters
	jpeg_read_header(&cinfo, TRUE);
	// Start the decompressor
	jpeg_start_decompress(&cinfo);
	// Get needed info...
	size.cx = cinfo.output_width;
	size.cy = cinfo.output_height;

	// Bits will be 8 for grey scale, 24 otherwise
	int bits = cinfo.out_color_components * 8;

	CDibLoader *pDibLoader = new CDibLoader( size.cx, size.cy, bits );

	RGBQUAD *pct = pDibLoader->GetColorTable();

	// A colormapped image may not have a palette... then it is gray scale
	
	if (cinfo.out_color_components == 1)		// colormapped...
	{
		// Create a colormap...
		if (cinfo.actual_number_of_colors && cinfo.colormap)
		{
			for (int i=0; i < cinfo.actual_number_of_colors; i++)
			{
				pct->rgbReserved = 0xFF;
				pct->rgbRed = cinfo.colormap[0][i];
				pct->rgbGreen = cinfo.colormap[1][i];
				pct->rgbBlue = cinfo.colormap[2][i];
				pct++;
			}
		}
		else
		{
			// No colormap supplied... create grayscale
			int NumColors = 1<<(bits);
			for (int i=0; i<NumColors; i++)
			{
				BYTE CurColor = static_cast<BYTE>( (i*255)/(NumColors-1) );
				pct->rgbReserved = pct->rgbRed = pct->rgbGreen = pct->rgbBlue = CurColor;
				pct++;
			}
		}
	}
	
	// Get lengthof row data to be read, and allocate a buffer for it
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	// Get line array from GS::CDIB
	Container::CArray< BYTE * > arrLines;

	pDibLoader->GetLineArray( arrLines );

	// Read each scan line

	for (unsigned int line = 0; line < cinfo.output_height; line++)
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);

		// Need to convert RGB color space to BGR colorspace
		// used by windows bmps. Swap Red and blue entries
		if (cinfo.output_components == 3)
		{
			for (unsigned int _i = 0; _i < cinfo.output_width; _i++)
			{
				BYTE temp = buffer[0][_i * 3];
				buffer[0][_i * 3] = buffer[0][(_i * 3) + 2];
				buffer[0][(_i * 3) + 2] = temp;
			}
		}
		// Copy it into the bit array of the bitmap
		::CopyMemory(arrLines[line], buffer[0], row_stride);
  	}

	// Finish decompression
	jpeg_finish_decompress(&cinfo);
	// Release JPEG decompression object */
	jpeg_destroy_decompress(&cinfo);

	// Save our work!
	if( pDibLoader )
	{
		HDC hdc = GetDC( NULL );

		pDib = new GS::CDIB( hdc, size.cx, size.cy );
		{
			CCompatibleDC dc( hdc );
			HGDIOBJ hOld = SelectObject( dc, pDib->GetBitmap() );
			pDibLoader->Draw( dc, 0, 0 );
			SelectObject( dc, hOld );
		}
		ReleaseDC( NULL, hdc );
		delete pDibLoader;

		CFrame *pFrame = new CFrame( pDib, 0 );
		arrFrames.Add( pFrame );
	}

	if( pDib )
		return true;
	return false;
}

#endif	//	IMGLIB_JPG

