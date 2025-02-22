/*
 * Copyright (c) 2002-2003 Michael David Adams.
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
 * @file jas_cm.h
 * @brief JasPer Color Management
 */

#ifndef JAS_CM_H
#define JAS_CM_H

/******************************************************************************\
*
\******************************************************************************/

/* The configuration header file should be included first. */
#include <jasper/jas_config.h>

#include <jasper/jas_types.h>
#include <jasper/jas_icc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup module_cm
 * @{
 */

/******************************************************************************\
* Types and Macros.
\******************************************************************************/

/*!
@brief
Color space.
*/
typedef unsigned jas_clrspc_t;

/*!
@brief
Transform operations
*/
typedef enum {
	JAS_CMXFORM_OP_FWD = 0,
	JAS_CMXFORM_OP_REV = 1,
	JAS_CMXFORM_OP_PROOF = 2,
	JAS_CMXFORM_OP_GAMUT = 3,
} jas_cmxform_op_t;

/*!
@brief
Rendering intents.
*/
typedef enum {
	JAS_CMXFORM_INTENT_PER = 0,
	JAS_CMXFORM_INTENT_RELCLR = 1,
	JAS_CMXFORM_INTENT_ABSCLR = 2,
	JAS_CMXFORM_INTENT_SAT = 3,
} jas_cmxform_intent_t;

/*!
@brief
Number of rendering intents.
*/
#define	JAS_CMXFORM_NUMINTENTS		4

/*!
@brief
Transform optimization.
*/
typedef enum {
	JAS_CMXFORM_OPTM_SPEED = 0,
	JAS_CMXFORM_OPTM_SIZE = 1,
	JAS_CMXFORM_OPTM_ACC = 2,
} jas_cmxform_optm_t;

/*!
@brief
Create a color space.
*/
#define	jas_clrspc_create(fam, mbr)	(((fam) << 8) | (mbr))

/*!
@brief
Get the family of a color space.
*/

#define	jas_clrspc_fam(clrspc)	((clrspc) >> 8)

/*!
@brief
Get the (family) member of a color space.
*/
#define	jas_clrspc_mbr(clrspc)	((clrspc) & 0xff)

/*!
@brief
Test if a color space is generic.
*/
#define	jas_clrspc_isgeneric(clrspc)	(!jas_clrspc_mbr(clrspc))

/*!
@brief
Test if a color space is unknown.
*/
#define	jas_clrspc_isunknown(clrspc)	((clrspc) & JAS_CLRSPC_UNKNOWNMASK)

#define	JAS_CLRSPC_UNKNOWNMASK	0x4000

/*!
@brief Color space families.
*/
#define	JAS_CLRSPC_FAM_UNKNOWN	0
#define	JAS_CLRSPC_FAM_XYZ	1
#define	JAS_CLRSPC_FAM_LAB	2
#define	JAS_CLRSPC_FAM_GRAY	3
#define	JAS_CLRSPC_FAM_RGB	4
#define	JAS_CLRSPC_FAM_YCBCR	5

/*!
@brief Specific color spaces.
*/
#define	JAS_CLRSPC_UNKNOWN	JAS_CLRSPC_UNKNOWNMASK
#define	JAS_CLRSPC_CIEXYZ	jas_clrspc_create(JAS_CLRSPC_FAM_XYZ, 1)
#define	JAS_CLRSPC_CIELAB	jas_clrspc_create(JAS_CLRSPC_FAM_LAB, 1)
#define	JAS_CLRSPC_SGRAY	jas_clrspc_create(JAS_CLRSPC_FAM_GRAY, 1)
#define	JAS_CLRSPC_SRGB		jas_clrspc_create(JAS_CLRSPC_FAM_RGB, 1)
#define	JAS_CLRSPC_SYCBCR	jas_clrspc_create(JAS_CLRSPC_FAM_YCBCR, 1)

/*!
@brief Generic color spaces.
*/
#define	JAS_CLRSPC_GENRGB	jas_clrspc_create(JAS_CLRSPC_FAM_RGB, 0)
#define	JAS_CLRSPC_GENGRAY	jas_clrspc_create(JAS_CLRSPC_FAM_GRAY, 0)
#define	JAS_CLRSPC_GENYCBCR	jas_clrspc_create(JAS_CLRSPC_FAM_YCBCR, 0)

#define	JAS_CLRSPC_CHANIND_YCBCR_Y	0
#define	JAS_CLRSPC_CHANIND_YCBCR_CB	1
#define	JAS_CLRSPC_CHANIND_YCBCR_CR	2

#define	JAS_CLRSPC_CHANIND_RGB_R	0
#define	JAS_CLRSPC_CHANIND_RGB_G	1
#define	JAS_CLRSPC_CHANIND_RGB_B	2

#define	JAS_CLRSPC_CHANIND_GRAY_Y	0

/*!
Real-number type.
*/
typedef double jas_cmreal_t;

struct jas_cmpxform_s;

/*!
@brief
Component format.
*/
typedef struct {
	long *buf;
	unsigned prec;
	int sgnd;
	unsigned width;
	unsigned height;
} jas_cmcmptfmt_t;

/*!
@brief
Pixmap (i.e., multicomponent) format.
*/
typedef struct {
	unsigned numcmpts;
	jas_cmcmptfmt_t *cmptfmts;
} jas_cmpixmap_t;

/*!
@brief
Transform operations.
*/
typedef struct {
	void (*destroy)(struct jas_cmpxform_s *pxform);
	int (*apply)(const struct jas_cmpxform_s *pxform, const jas_cmreal_t *in, jas_cmreal_t *out, unsigned  cnt);
	void (*dump)(struct jas_cmpxform_s *pxform);
} jas_cmpxformops_t;

/*!
@brief
Shaper matrix look-up table (LUT).
*/
typedef struct {
	jas_cmreal_t *data;
	unsigned size;
} jas_cmshapmatlut_t;

/*!
@brief
Shaper matrix.
*/
typedef struct {
	int mono;
	int order;
	int useluts;
	int usemat;
	jas_cmshapmatlut_t luts[3];
	jas_cmreal_t mat[3][4];
} jas_cmshapmat_t;

/*!
@brief
Shaper look-up table (LUT).
*/
typedef struct {
	int order;
} jas_cmshaplut_t;

/*!
@brief
Color space conversion.
*/
typedef struct {
	unsigned inclrspc;
	unsigned outclrspc;
} jas_cmclrspcconv_t;

/*!
@brief
Transform class.
*/
typedef struct jas_cmpxform_s {
	unsigned refcnt;
	const jas_cmpxformops_t *ops;
	unsigned numinchans;
	unsigned numoutchans;
	union {
		max_align_t dummy;
		jas_cmshapmat_t shapmat;
		jas_cmshaplut_t shaplut;
		jas_cmclrspcconv_t clrspcconv;
	} data;
} jas_cmpxform_t;

/*!
@brief
Primitive transform sequence class.
*/
typedef struct {
	unsigned numpxforms;
	unsigned maxpxforms;
	jas_cmpxform_t **pxforms;
} jas_cmpxformseq_t;

/*!
@brief
Primitive transform class.
*/
typedef struct {
	unsigned numinchans;
	unsigned numoutchans;
	jas_cmpxformseq_t *pxformseq;
} jas_cmxform_t;

#define	JAS_CMPROF_TYPE_DEV	1
#define	JAS_CMPROF_TYPE_CLRSPC	2

#define	JAS_CMPROF_NUMPXFORMSEQS	13

/*!
Color-management (CM) profile.
*/
typedef struct {
	jas_clrspc_t clrspc;
	unsigned numchans;
	unsigned refclrspc;
	unsigned numrefchans;
	jas_iccprof_t *iccprof;
	jas_cmpxformseq_t *pxformseqs[JAS_CMPROF_NUMPXFORMSEQS];
} jas_cmprof_t;

/******************************************************************************\
*
\******************************************************************************/

#if 0
typedef int_fast32_t jas_cmattrname_t;
typedef int_fast32_t jas_cmattrval_t;
typedef int_fast32_t jas_cmattrtype_t;
/* Load a profile. */
int jas_cmprof_load(jas_cmprof_t *prof, jas_stream_t *in, unsigned fmt);
/* Save a profile. */
int jas_cmprof_save(jas_cmprof_t *prof, jas_stream_t *out, unsigned fmt);
/* Set an attribute of a profile. */
int jas_cm_prof_setattr(jas_cm_prof_t *prof, jas_cm_attrname_t name, void *val);
/* Get an attribute of a profile. */
void *jas_cm_prof_getattr(jas_cm_prof_t *prof, jas_cm_attrname_t name);
#endif

/******************************************************************************\
* Color-management (CM) profile class.
\******************************************************************************/

/*!
@brief
Create a color-management profile from an ICC profile.

@details
This function creates a CM profile from an ICC profile.

@returns
If successful, a pointer to the created CM profile is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_cmprof_t *jas_cmprof_createfromiccprof(const jas_iccprof_t *iccprof);

/*!
@brief
Create a color-management profile from a color space.

@details
The function creates a CM profile from a color space.

@returns
If successful, a pointer to the created CM profile is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_cmprof_t *jas_cmprof_createfromclrspc(jas_clrspc_t clrspc);

/*!
@brief Destroy a color-management profile.

@details
*/
JAS_EXPORT
void jas_cmprof_destroy(jas_cmprof_t *prof);

/*!
@brief
Copy a color-management profile.

@details
This function creates a clone (i.e., copy) of a CM profile.

@returns
If successful, a pointer to the newly created CM profile is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_cmprof_t *jas_cmprof_copy(const jas_cmprof_t *prof);

/*!
@brief
Create a ICC profile from a CM profile.

@details
This function creates an ICC profile from a CM profile.

@returns
If successful, a pointer to the created ICC profile is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_iccprof_t *jas_iccprof_createfromcmprof(const jas_cmprof_t *prof);

/******************************************************************************\
* Color-Management (CM) Transform.
\******************************************************************************/

/*!
@brief
Create a transform from a CM profile.

@details

@returns
If successful, a pointer to the created transform is returned.
Otherwise, a null pointer is returned.
*/
JAS_EXPORT
jas_cmxform_t *jas_cmxform_create(const jas_cmprof_t *inprof,
  const jas_cmprof_t *outprof, const jas_cmprof_t *proofprof,
  jas_cmxform_op_t op, jas_cmxform_intent_t intent,
  jas_cmxform_optm_t optimize);

/*!
@brief
Destroy a transform.

@details
*/
JAS_EXPORT
void jas_cmxform_destroy(jas_cmxform_t *xform);

/*!
@brief
Apply a transform to data.

@details

@returns
If successful, zero is returned.
Otherwise, a nonzero value is returned.
*/
JAS_EXPORT
int jas_cmxform_apply(const jas_cmxform_t *xform, const jas_cmpixmap_t *in,
  jas_cmpixmap_t *out);

/******************************************************************************\
* Miscellany.
\******************************************************************************/

/*!
@brief
Get the number of channels associated with a particular color space.

@details

@returns
*/
unsigned jas_clrspc_numchans(jas_clrspc_t clrspc);

/*!
@brief
Get the color space associated with a color-management profile.

@details

@returns
*/
#define	jas_cmprof_clrspc(prof) ((prof)->clrspc)

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
