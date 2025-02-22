/*
 * Copyright (c) 1999-2000 Image Power, Inc. and the University of
 *   British Columbia.
 * Copyright (c) 2001-2003 Michael David Adams.
 * All rights reserved.
 */

/* __START_OF_JASPER_LICENSE__
 * 
 * JasPer License Version 2.0
 * 
 * Copyright (c) 2001-2006 Michael David Adams
 * Copyright (c) 1999-2000 Image Power, Inc.
 * Copyright (c) 1999-2000 The University of British Columbia
 * 
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person (the
 * "User") obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 * 
 * 1.  The above copyright notices and this permission notice (which
 * includes the disclaimer below) shall be included in all copies or
 * substantial portions of the Software.
 * 
 * 2.  The name of a copyright holder shall not be used to endorse or
 * promote products derived from the Software without specific prior
 * written permission.
 * 
 * THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
 * LICENSE.  NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
 * THIS DISCLAIMER.  THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  NO ASSURANCES ARE
 * PROVIDED BY THE COPYRIGHT HOLDERS THAT THE SOFTWARE DOES NOT INFRINGE
 * THE PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OF ANY OTHER ENTITY.
 * EACH COPYRIGHT HOLDER DISCLAIMS ANY LIABILITY TO THE USER FOR CLAIMS
 * BROUGHT BY ANY OTHER ENTITY BASED ON INFRINGEMENT OF INTELLECTUAL
 * PROPERTY RIGHTS OR OTHERWISE.  AS A CONDITION TO EXERCISING THE RIGHTS
 * GRANTED HEREUNDER, EACH USER HEREBY ASSUMES SOLE RESPONSIBILITY TO SECURE
 * ANY OTHER INTELLECTUAL PROPERTY RIGHTS NEEDED, IF ANY.  THE SOFTWARE
 * IS NOT FAULT-TOLERANT AND IS NOT INTENDED FOR USE IN MISSION-CRITICAL
 * SYSTEMS, SUCH AS THOSE USED IN THE OPERATION OF NUCLEAR FACILITIES,
 * AIRCRAFT NAVIGATION OR COMMUNICATION SYSTEMS, AIR TRAFFIC CONTROL
 * SYSTEMS, DIRECT LIFE SUPPORT MACHINES, OR WEAPONS SYSTEMS, IN WHICH
 * THE FAILURE OF THE SOFTWARE OR SYSTEM COULD LEAD DIRECTLY TO DEATH,
 * PERSONAL INJURY, OR SEVERE PHYSICAL OR ENVIRONMENTAL DAMAGE ("HIGH
 * RISK ACTIVITIES").  THE COPYRIGHT HOLDERS SPECIFICALLY DISCLAIM ANY
 * EXPRESS OR IMPLIED WARRANTY OF FITNESS FOR HIGH RISK ACTIVITIES.
 * 
 * __END_OF_JASPER_LICENSE__
 */

/*!
 * @file jas_image.h
 * @brief JasPer Image Class
 */

#ifndef JAS_IMAGE_H
#define JAS_IMAGE_H

/******************************************************************************\
* Includes.
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#include <jasper/jas_stream.h>
#include <jasper/jas_types.h>
#include <jasper/jas_seq.h> /* IWYU pragma: export */
#include <jasper/jas_cm.h> /* IWYU pragma: export */
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup module_images
 * @{
 */

/******************************************************************************\
* Constants.
\******************************************************************************/

/*
 * Miscellaneous constants.
 */

/* Basic units */
#define JAS_IMAGE_KIBI			(JAS_CAST(size_t, 1024))
#define JAS_IMAGE_MEBI			(JAS_IMAGE_KIBI * JAS_IMAGE_KIBI)

/* The threshold at which image data is no longer stored in memory. */
#define JAS_IMAGE_INMEMTHRESH	(256 * JAS_IMAGE_MEBI)

/*
 * Component types
 */

#define	JAS_IMAGE_CT_UNKNOWN	0x10000
#define	JAS_IMAGE_CT_COLOR(n)	((n) & 0x7fff)
#define	JAS_IMAGE_CT_OPACITY	0x08000

#define	JAS_IMAGE_CT_RGB_R	0
#define	JAS_IMAGE_CT_RGB_G	1
#define	JAS_IMAGE_CT_RGB_B	2

#define	JAS_IMAGE_CT_YCBCR_Y	0
#define	JAS_IMAGE_CT_YCBCR_CB	1
#define	JAS_IMAGE_CT_YCBCR_CR	2

#define	JAS_IMAGE_CT_GRAY_Y	0

/******************************************************************************\
* Simple types.
\******************************************************************************/

/*!
@brief Image coordinate.
*/
typedef int_fast32_t jas_image_coord_t;
#define JAS_IMAGE_COORD_MAX INT_FAST32_MAX
#define JAS_IMAGE_COORD_MIN INT_FAST32_MIN

/*!
@brief Color space (e.g., RGB, YCbCr).
*/
typedef int_fast16_t jas_image_colorspc_t;

/*!
@brief Component type (e.g., color, opacity).
*/
typedef int_fast32_t jas_image_cmpttype_t;

/*!
@brief Component sample data format
(e.g., real/integer, signedness, precision).
*/
typedef int_fast16_t jas_image_smpltype_t;

/******************************************************************************\
* Image class and supporting classes.
\******************************************************************************/

/*!
@brief
Image component class.

@warning
Library users should never directly access any of the members of this
class.
The functions/macros provided by the JasPer library API should always
be used.
*/
typedef struct {

	/* The x-coordinate of the top-left corner of the component. */
	jas_image_coord_t tlx_;

	/* The y-coordinate of the top-left corner of the component. */
	jas_image_coord_t tly_;

	/* The horizontal sampling period in units of the reference grid. */
	jas_image_coord_t hstep_;

	/* The vertical sampling period in units of the reference grid. */
	jas_image_coord_t vstep_;

	/* The component width in samples. */
	jas_image_coord_t width_;

	/* The component height in samples. */
	jas_image_coord_t height_;

	/* The precision of the sample data (i.e., the number of bits per sample).
	If the samples are signed values, this quantity includes the sign bit. */
	unsigned prec_;

	/* The signedness of the sample data. */
	int sgnd_;

	/* The stream containing the component data. */
	jas_stream_t *stream_;

	/* The number of characters per sample in the stream. */
	unsigned cps_;

	/* The type of component (e.g., opacity, red, green, blue, luma). */
	jas_image_cmpttype_t type_;

} jas_image_cmpt_t;

/*!
@brief Image class.

@warning
Library users should never directly access any of the members of this
class.
The functions/macros provided by the JasPer library API should always
be used.
*/
typedef struct {

	/* The x-coordinate of the top-left corner of the image bounding box. */
	jas_image_coord_t tlx_;

	/* The y-coordinate of the top-left corner of the image bounding box. */
	jas_image_coord_t tly_;

	/* The x-coordinate of the bottom-right corner of the image bounding
	  box (plus one). */
	jas_image_coord_t brx_;

	/* The y-coordinate of the bottom-right corner of the image bounding
	  box (plus one). */
	jas_image_coord_t bry_;

	/* The number of components. */
	unsigned numcmpts_;

	/* The maximum number of components that this image can have (i.e., the
	  allocated size of the components array). */
	unsigned maxcmpts_;

	/* Per-component information. */
	jas_image_cmpt_t **cmpts_;

	/* The color space. */
	jas_clrspc_t clrspc_;

	/* The CM profile. */
	jas_cmprof_t *cmprof_;

	//bool inmem_;

} jas_image_t;

/*!
@brief Component parameters class.

@details
This data type exists solely/mainly for the purposes of the
jas_image_create function.
*/
typedef struct {

	/* The x-coordinate of the top-left corner of the component. */
	jas_image_coord_t tlx;

	/* The y-coordinate of the top-left corner of the component. */
	jas_image_coord_t tly;

	/* The horizontal sampling period in units of the reference grid. */
	jas_image_coord_t hstep;

	/* The vertical sampling period in units of the reference grid. */
	jas_image_coord_t vstep;

	/* The width of the component in samples. */
	jas_image_coord_t width;

	/* The height of the component in samples. */
	jas_image_coord_t height;

	/* The precision of the component sample data. */
	unsigned prec;

	/* The signedness of the component sample data. */
	int sgnd;

} jas_image_cmptparm_t;

/******************************************************************************\
* File format related classes.
\******************************************************************************/

/*!
@brief The maximum number of image data formats supported.
*/
#define	JAS_IMAGE_MAXFMTS	32

/*!
@brief Image format-dependent operations.
*/
typedef struct {

	/*! Decode image data from a stream. */
	jas_image_t *(*decode)(jas_stream_t *in, const char *opts);

	/*! Encode image data to a stream. */
	int (*encode)(jas_image_t *image, jas_stream_t *out, const char *opts);

	/*! Determine if stream data is in a particular format. */
	int (*validate)(jas_stream_t *in);

} jas_image_fmtops_t;

/*!
@brief Image format information.
*/
typedef struct {

	/*! The ID for this format. */
	int id;

	/*! The name by which this format is identified. */
	char *name;

	/* The primary file name extension associated with this format. */
	/* This member only exists for backward compatibility. */
	char *ext;

	/*! The table of file name extensions associated with this format. */
	char **exts;
	size_t max_exts;
	size_t num_exts;

	/*! A boolean flag indicating if this format is enabled. */
	int enabled;

	/*! A brief description of the format. */
	char *desc;

	/*! The operations for this format. */
	jas_image_fmtops_t ops;

} jas_image_fmtinfo_t;

/******************************************************************************\
* Image operations.
\******************************************************************************/

/*!
@brief Create an image.
*/
JAS_EXPORT
jas_image_t *jas_image_create(unsigned numcmpts,
  const jas_image_cmptparm_t *cmptparms, jas_clrspc_t clrspc);

/*!
@brief Create an "empty" image.
*/
JAS_EXPORT
jas_image_t *jas_image_create0(void);

/*!
@brief Clone an image.
*/
JAS_EXPORT
jas_image_t *jas_image_copy(jas_image_t *image);

/*!
@brief Deallocate any resources associated with an image.
*/
JAS_EXPORT
void jas_image_destroy(jas_image_t *image);

/*!
@brief Get the width of the image in units of the image reference grid.
*/
#define jas_image_width(image) \
	((image)->brx_ - (image)->tlx_)

/*!
@brief Get the height of the image in units of the image reference grid.
*/
#define	jas_image_height(image) \
	((image)->bry_ - (image)->tly_)

/*!
@brief Get the x-coordinate of the top-left corner of the image bounding box
on the reference grid.
*/
#define jas_image_tlx(image) \
	((image)->tlx_)

/*!
@brief Get the y-coordinate of the top-left corner of the image bounding box
  on the reference grid.
*/
#define jas_image_tly(image) \
	((image)->tly_)

/*!
@brief Get the x-coordinate of the bottom-right corner of the image bounding box
  on the reference grid (plus one).
*/
#define jas_image_brx(image) \
	((image)->brx_)

/*!
@brief Get the y-coordinate of the bottom-right corner of the image bounding
box on the reference grid (plus one).
*/
#define jas_image_bry(image) \
	((image)->bry_)

/*!
@brief Get the number of image components.
*/
#define	jas_image_numcmpts(image) \
	((image)->numcmpts_)

/*!
@brief Get the color model used by the image.
*/
#define	jas_image_clrspc(image) \
	((image)->clrspc_)

/*!
@brief Set the color model for an image.
*/
#define jas_image_setclrspc(image, clrspc) \
	((image)->clrspc_ = (clrspc))

#define jas_image_cmpttype(image, cmptno) \
	((image)->cmpts_[(cmptno)]->type_)
#define jas_image_setcmpttype(image, cmptno, type) \
	((image)->cmpts_[(cmptno)]->type_ = (type))

/*!
@brief Get the width of a component.
*/
#define	jas_image_cmptwidth(image, cmptno) \
	((image)->cmpts_[cmptno]->width_)

/*!
@brief Get the height of a component.
*/
#define	jas_image_cmptheight(image, cmptno) \
	((image)->cmpts_[cmptno]->height_)

/*!
@brief Get the signedness of the sample data for a component.
*/
#define	jas_image_cmptsgnd(image, cmptno) \
	((image)->cmpts_[cmptno]->sgnd_)

/*!
@brief Get the precision of the sample data for a component.
*/
#define	jas_image_cmptprec(image, cmptno) \
	((image)->cmpts_[cmptno]->prec_)

/*!
@brief Get the horizontal subsampling factor for a component.
*/
#define	jas_image_cmpthstep(image, cmptno) \
	((image)->cmpts_[cmptno]->hstep_)

/*!
@brief Get the vertical subsampling factor for a component.
*/
#define	jas_image_cmptvstep(image, cmptno) \
	((image)->cmpts_[cmptno]->vstep_)

/*!
@brief Get the x-coordinate of the top-left corner of a component.
*/
#define	jas_image_cmpttlx(image, cmptno) \
	((image)->cmpts_[cmptno]->tlx_)

/*!
@brief Get the y-coordinate of the top-left corner of a component.
*/
#define	jas_image_cmpttly(image, cmptno) \
	((image)->cmpts_[cmptno]->tly_)

/*!
@brief Get the x-coordinate of the bottom-right corner of a component
(plus "one").
*/
#define	jas_image_cmptbrx(image, cmptno) \
	((image)->cmpts_[cmptno]->tlx_ + (image)->cmpts_[cmptno]->width_ * \
	  (image)->cmpts_[cmptno]->hstep_)

/*!
@brief Get the y-coordinate of the bottom-right corner of a component
(plus "one").
*/
#define	jas_image_cmptbry(image, cmptno) \
	((image)->cmpts_[cmptno]->tly_ + (image)->cmpts_[cmptno]->height_ * \
	  (image)->cmpts_[cmptno]->vstep_)

/*!
@brief Test if all components are specified at the same positions in space.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
bool jas_image_cmpt_domains_same(const jas_image_t *image);

/*!
@brief Get the raw size of an image
(i.e., the nominal size of the image without any compression.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
uint_fast32_t jas_image_rawsize(const jas_image_t *image);

/*!
@brief Create an image from a stream in some specified format.
*/
JAS_EXPORT
jas_image_t *jas_image_decode(jas_stream_t *in, int fmt, const char *optstr);

/*!
@brief Write an image to a stream in a specified format.
*/
JAS_EXPORT
int jas_image_encode(jas_image_t *image, jas_stream_t *out, int fmt,
  const char *optstr);

/*!
@brief Read a rectangular region of an image component.

@details
The position and size of the rectangular region to be read is specified
relative to the component's coordinate system.
*/
JAS_EXPORT
int jas_image_readcmpt(jas_image_t *image, unsigned cmptno,
  jas_image_coord_t x, jas_image_coord_t y, jas_image_coord_t width,
  jas_image_coord_t height, jas_matrix_t *data);

/*!
@brief Write a rectangular region of an image component.
*/
JAS_EXPORT
int jas_image_writecmpt(jas_image_t *image, unsigned cmptno,
  jas_image_coord_t x, jas_image_coord_t y, jas_image_coord_t width,
  jas_image_coord_t height, const jas_matrix_t *data);

/*!
@brief Delete a component from an image.
*/
JAS_EXPORT
void jas_image_delcmpt(jas_image_t *image, unsigned cmptno);

/*!
@brief Add a component to an image.
*/
JAS_EXPORT
int jas_image_addcmpt(jas_image_t *image, int cmptno,
  const jas_image_cmptparm_t *cmptparm);

/*!
@brief Copy a component from one image to another.
*/
JAS_EXPORT
int jas_image_copycmpt(jas_image_t *dstimage, unsigned dstcmptno,
  jas_image_t *srcimage, unsigned srccmptno);

JAS_ATTRIBUTE_CONST
static inline bool JAS_IMAGE_CDT_GETSGND(uint_least8_t dtype)
{
	return (dtype >> 7) & 1;
}

JAS_ATTRIBUTE_CONST
static inline uint_least8_t JAS_IMAGE_CDT_SETSGND(bool sgnd)
{
	return (uint_least8_t)sgnd << 7;
}

JAS_ATTRIBUTE_CONST
static inline uint_least8_t JAS_IMAGE_CDT_GETPREC(uint_least8_t dtype)
{
	return dtype & 0x7f;
}

JAS_ATTRIBUTE_CONST
static inline uint_least8_t JAS_IMAGE_CDT_SETPREC(uint_least8_t dtype)
{
	return dtype & 0x7f;
}

JAS_ATTRIBUTE_PURE
static inline uint_least8_t jas_image_cmptdtype(const jas_image_t *image,
  unsigned cmptno)
{
	return JAS_IMAGE_CDT_SETSGND(image->cmpts_[cmptno]->sgnd_) |
		JAS_IMAGE_CDT_SETPREC(image->cmpts_[cmptno]->prec_);
}

/*!
@brief Depalettize an image
*/
JAS_EXPORT
int jas_image_depalettize(jas_image_t *image, unsigned cmptno,
  unsigned numlutents, const int_fast32_t *lutents, unsigned dtype,
  unsigned newcmptno);

/*!
@brief Read a component sample for an image.
*/
JAS_EXPORT
int jas_image_readcmptsample(jas_image_t *image, unsigned cmptno, unsigned x,
  unsigned y);

/*!
@brief Write a component sample for an image.
*/
JAS_EXPORT
void jas_image_writecmptsample(jas_image_t *image, unsigned cmptno,
  unsigned x, unsigned y, int_fast32_t v);

/*!
@brief Get an image component by its type.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
int jas_image_getcmptbytype(const jas_image_t *image, jas_image_cmpttype_t ctype);

/******************************************************************************\
* Image format-related operations.
\******************************************************************************/

/*!
@brief Clear the table of image formats.
*/
JAS_EXPORT
void jas_image_clearfmts(void);

#if defined(JAS_FOR_INTERNAL_USE_ONLY)
void jas_image_clearfmts_internal(jas_image_fmtinfo_t *image_fmtinfos,
  size_t *image_numfmts);
#endif

/*!
@brief Get a image format entry by its table index.
*/
JAS_EXPORT
const jas_image_fmtinfo_t *jas_image_getfmtbyind(int index);

/*!
@brief Get the number of image format table entries.
*/
JAS_EXPORT
int jas_image_getnumfmts(void);

#if 0
JAS_EXPORT
int jas_image_delfmtbyid(int id);
#endif

/*!
@brief Get the number of image format table entries.

@warning
This function may be removed in future versions of the library.
Do not rely on it.
*/
JAS_EXPORT
int jas_image_setfmtenable(int index, int enabled);

#if 0
// TODO: should this be added?
JAS_EXPORT
int jas_image_getfmtindbyname(const char* name);
#endif

/*!
@brief Add entry to table of image formats.
*/
JAS_EXPORT
int jas_image_addfmt(int id, const char *name, const char *ext,
  const char *desc, const jas_image_fmtops_t *ops);

#if defined(JAS_FOR_INTERNAL_USE_ONLY)
int jas_image_addfmt_internal(jas_image_fmtinfo_t *image_fmtinfos,
  size_t *image_numfmts, int id, const char *name, const char *ext,
  const char *desc, const jas_image_fmtops_t *ops);
#endif

/*!
@brief Get the ID for the image format with the specified name.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
int jas_image_strtofmt(const char *s);

/*!
@brief Get the name of the image format with the specified ID.
*/
JAS_ATTRIBUTE_CONST
JAS_EXPORT
const char *jas_image_fmttostr(int fmt);

/*!
@brief Lookup image format information by the format ID.
*/
JAS_ATTRIBUTE_CONST
JAS_EXPORT
const jas_image_fmtinfo_t *jas_image_lookupfmtbyid(int id);

/*!
@brief Lookup image format information by the format name.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
const jas_image_fmtinfo_t *jas_image_lookupfmtbyname(const char *name);

/*!
@brief Guess the format of an image file based on its name.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
int jas_image_fmtfromname(const char *filename);

/*!
@brief Get the format of image data in a stream.

@details
Note that only enabled codecs are used in determining the image format.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
int jas_image_getfmt(jas_stream_t *in);

/*!
@brief Get the color management profile of an image.
*/
#define	jas_image_cmprof(image)	((image)->cmprof_)

/*!
@brief
Test if the sampling of the image is homogeneous.
*/
JAS_ATTRIBUTE_PURE
JAS_EXPORT
int jas_image_ishomosamp(const jas_image_t *image);

/*!
@brief ???
*/
JAS_EXPORT
int jas_image_sampcmpt(jas_image_t *image, unsigned cmptno, unsigned newcmptno,
  jas_image_coord_t ho, jas_image_coord_t vo, jas_image_coord_t hs,
  jas_image_coord_t vs, int sgnd, unsigned prec);

/*!
@brief Write sample data in a component of an image.
*/
JAS_EXPORT
int jas_image_writecmpt2(jas_image_t *image, unsigned cmptno,
  jas_image_coord_t x, jas_image_coord_t y, jas_image_coord_t width,
  jas_image_coord_t height, const long *buf);

/*!
@brief Read sample data in a component of an image.
*/
JAS_EXPORT
int jas_image_readcmpt2(jas_image_t *image, unsigned cmptno,
  jas_image_coord_t x, jas_image_coord_t y, jas_image_coord_t width,
  jas_image_coord_t height, long *buf);

/*!
@brief Set the color management profile for an image.
*/
#define	jas_image_setcmprof(image, cmprof) ((image)->cmprof_ = cmprof)

/*!
@brief Change the color space for an image.
*/
JAS_EXPORT
jas_image_t *jas_image_chclrspc(jas_image_t *image,
  const jas_cmprof_t *outprof, jas_cmxform_intent_t intent);

/*!
@brief Dump the information for an image (for debugging).
*/
JAS_EXPORT
int jas_image_dump(jas_image_t *image, FILE *out);

/******************************************************************************\
* Image format-dependent operations.
\******************************************************************************/

#if defined(JAS_INCLUDE_JPG_CODEC)
/* Format-dependent operations for JPG support. */
//JAS_EXPORT
jas_image_t *jpg_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int jpg_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int jpg_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_HEIC_CODEC)
/* Format-dependent operations for HEIC support. */
//JAS_EXPORT
jas_image_t *jas_heic_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int jas_heic_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int jas_heic_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_MIF_CODEC)
/* Format-dependent operations for MIF support. */
//JAS_EXPORT
jas_image_t *mif_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int mif_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int mif_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_PNM_CODEC)
/* Format-dependent operations for PNM support. */
//JAS_EXPORT
jas_image_t *pnm_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int pnm_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int pnm_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_RAS_CODEC)
/* Format-dependent operations for Sun Rasterfile support. */
//JAS_EXPORT
jas_image_t *ras_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int ras_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int ras_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_BMP_CODEC)
/* Format-dependent operations for BMP support. */
//JAS_EXPORT
jas_image_t *bmp_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int bmp_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int bmp_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_JP2_CODEC)
/* Format-dependent operations for JP2 support. */
//JAS_EXPORT
jas_image_t *jp2_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int jp2_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int jp2_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_JPC_CODEC)
/* Format-dependent operations for JPEG-2000 code stream support. */
//JAS_EXPORT
jas_image_t *jpc_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int jpc_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int jpc_validate(jas_stream_t *in);
#endif

#if defined(JAS_INCLUDE_PGX_CODEC)
/* Format-dependent operations for PGX support. */
//JAS_EXPORT
jas_image_t *pgx_decode(jas_stream_t *in, const char *optstr);
//JAS_EXPORT
int pgx_encode(jas_image_t *image, jas_stream_t *out, const char *optstr);
//JAS_EXPORT
int pgx_validate(jas_stream_t *in);
#endif

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
