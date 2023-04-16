/****************************************************************************
**
** NOTE THIS FILE ISN'T ACTUALLY USED by DeepSkyStacker, it is here because
** it contains lots of useful information.

** Copyright (C) 2023 David C. Partridge
**      Based on FCam TIFFTags.cpp
**      https://graphics.stanford.edu/papers/fcam/html/files.html
** 
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include "stdafx.h"
#include <map>
#include <mutex>

#include "TIFFTags.h"

namespace TIFF
{
    const TiffEntryInfo tiffEntryTypes[] 
    {
        //// BASELINE TAGS
        {
            "Artist",
            // Person who created the image.
            TIFFTAG_ARTIST,
            TIFF_ASCII
            // Note: some older TIFF files used this tag for storing Copyright information.
        },{
            "BitsPerSample",
            // Number of bits per component.
            TIFFTAG_BITSPERSAMPLE,
            TIFF_SHORT
            // N = SamplesPerPixel
            // Note that this field allows a different number of bits per component for each
            // component corresponding to a pixel. For example, RGB color data could use a
            // different number of bits per component for each of the three color planes. Most RGB
            // files will have the same number of BitsPerSample for each component. Even in this
            // case, the writer must write all three values.
            // Default = 1. See also SamplesPerPixel.
            //
            // From DNG Spec:
            // Supported values are from 8 to 32 bits/sample. The depth
            // must be the same for each sample if SamplesPerPixel is not
            // equal to 1. If BitsPerSample is not equal to 8 or 16 or 32,
            // then the bits must be packed into bytes using the TIFF
            // default FillOrder of 1 (big-endian), even if the TIFF file
            // itself uses little-endian byte order.
        },{
            "CellLength",
            // The length of the dithering or halftoning matrix used to create a dithered or
            // halftoned bilevel file.
            TIFFTAG_CELLLENGTH,
            TIFF_SHORT
            // N = 1
            // This field should only be present if Threshholding = 2
            // No default. See also Threshholding.
        },{
            "CellWidth",
            // The width of the dithering or halftoning matrix used to create a dithered or
            // halftoned bilevel file
            TIFFTAG_CELLWIDTH,
            TIFF_SHORT
            // N = 1
            // No default. See also Threshholding.
        },{
            "ColorMap",
            // A color map for palette color images.
            32,
            TIFF_SHORT
            // N = 3 * (2**BitsPerSample)
            // This field defines a Red-Green-Blue color map (often called a lookup table) for
            // palette-color images. In a palette-color image, a pixel value is used to index into
            // an RGB lookup table. For example, a palette-color pixel having a value of 0
            // would be displayed according to the 0th Red, Green, Blue triplet.
            // In a TIFF ColorMap, all the Red values come first, followed by the Green values,
            // then the Blue values. The number of values for each color is 2**BitsPerSample.
            // Therefore, the ColorMap field for an 8-bit palette-color image would have 3 * 256
            // values.
            // The width of each value is 16 bits, as implied by the type of SHORT. 0 represents
            // the minimum intensity, and 65535 represents the maximum intensity. Black is
            // represented by 0,0,0, and white by 65535, 65535, 65535.
            // See also PhotometricInterpretation—palette color.
            // No default. ColorMap must be included in all palette-color images.
        },{
            "Compression",
            // Compression scheme used on the image data.
            TIFFTAG_COMPRESSION,
            TIFF_SHORT
            // N = 1
            // 1 = No compression, but pack data into bytes as tightly as possible leaving no unused
            // bits except at the end of a row.
            // If Then the sample values are stored as an array of type:
            // BitsPerSample = 16 for all samples SHORT
            // BitsPerSample = 32 for all samples LONG
            // Otherwise BYTE
            // Each row is padded to the next BYTE/SHORT/LONG boundary, consistent with
            // the preceding BitsPerSample rule.
            // If the image data is stored as an array of SHORTs or LONGs, the byte ordering
            // must be consistent with that specified in bytes 0 and 1 of the TIFF file header.
            // Therefore, little-endian format files will have the least significant bytes preceding
            // the most significant bytes, while big-endian format files will have the opposite
            // order.
            // If the number of bits per component is not a power of 2, and you are willing to give up
            // some space for better performance, use the next higher power of 2. For example, if
            // your data can be represented in 6 bits, set BitsPerSample to 8 instead of 6, and then
            // convert the range of the values from [0,63] to [0,255].
            // Rows must begin on byte boundaries. (SHORT boundaries if the data is stored as
            // SHORTs, LONG boundaries if the data is stored as LONGs).
            // Some graphics systems require image data rows to be word-aligned or double-word-
            // aligned, and padded to word-boundaries or double-word boundaries. Uncompressed
            // TIFF rows will need to be copied into word-aligned or double-word-aligned row
            // buffers before being passed to the graphics routines in these environments.
            // 2 = CCITT Group 3 1-Dimensional Modified Huffman run-length encoding. See
            // Section 10. BitsPerSample must be 1, since this type of compression is defined
            // only for bilevel images.
            // 32773 = PackBits compression, a simple byte-oriented run-length scheme. See Section 9
            // for details.
            // Data compression applies only to the image data, pointed to by StripOffsets.
            // Default = 1.
            //
            // From DNG Spec:
            // Two Compression tag values are supported:
            // Value = 1: Uncompressed data.
            // Value = 7: JPEG compressed data, either baseline DCT JPEG, or lossless JPEG compression.
            // If PhotometricInterpretation = 6 (YCbCr) and BitsPerSample
            // = 8/8/8, or if PhotometricInterpretation = 1 (BlackIsZero)
            // and BitsPerSample = 8, then the JPEG variant must be
            // baseline DCT JPEG. Otherwise, the JPEG variant must be
            // lossless Huffman JPEG. For lossless JPEG, the internal
            // width/length/components in the JPEG stream are not required
            // to match the strip or tile's width/length/components. Only
            // the total sample counts need to match. It is common for CFA
            // images to be encoded with a different width, length or
            // component count to allow the JPEG compression predictors to
            // work across like colors.
        },{
            "Copyright",
            // Copyright notice.
            TIFFTAG_COPYRIGHT,
            TIFF_ASCII
            // Copyright notice of the person or organization that claims the copyright to the
            // image. The complete copyright statement should be listed in this field including
            // any dates and statements of claims. For example, “Copyright, John Smith, 19xx.
            // All rights reserved.”
        },{
            "DateTime",
            // Date and time of image creation.
            TIFFTAG_DATETIME,
            TIFF_ASCII
            // N = 20
            // The format is: “YYYY:MM:DD HH:MM:SS”, with hours like those on a 24-hour
            // clock, and one space character between the date and the time. The length of the
            // string, including the terminating NUL, is 20 bytes.
        },{
            "ExtraSamples",
            // Description of extra components.
            TIFFTAG_EXTRASAMPLES,
            TIFF_SHORT
            // N = m
            // Specifies that each pixel has m extra components whose interpretation is defined
            // by one of the values listed below. When this field is used, the SamplesPerPixel
            // field has a value greater than the PhotometricInterpretation field suggests.
            // For example, full-color RGB data normally has SamplesPerPixel=3. If
            // SamplesPerPixel is greater than 3, then the ExtraSamples field describes the
            // meaning of the extra samples. If SamplesPerPixel is, say, 5 then ExtraSamples
            // will contain 2 values, one for each extra sample.
            // ExtraSamples is typically used to include non-color information, such as opacity,
            // in an image. The possible values for each item in the field's value are:
            // 0 = Unspecified data
            // 1 = Associated alpha data (with pre-multiplied color)
            // 2 = Unassociated alpha data
            // Associated alpha data is opacity information; it is fully described in Section 21.
            // Unassociated alpha data is transparency information that logically exists indepen-
            // dent of an image; it is commonly called a soft matte. Note that including both
            // unassociated Land associated alpha is undefined because associated alpha specifies
            // that color components are pre-multiplied by the alpha component, while
            // unassociated alpha specifies the opposite.
            // By convention, extra components that are present must be stored as the “last com-
            // ponents” in each pixel. For example, if SamplesPerPixel is 4 and there is 1 extra
            // component, then it is located in the last component location (SamplesPerPixel-1)
            // in each pixel.
            // Components designated as “extra” are just like other components in a pixel. In
            // particular, the size of such components is defined by the value of the
            // BitsPerSample field.
            // With the introduction of this field, TIFF readers must not assume a particular
            // SamplesPerPixel value based on the value of the PhotometricInterpretation field.
            // For example, if the file is an RGB file, SamplesPerPixel may be greater than 3.
            // The default is no extra samples. This field must be present if there are extra
            // samples.
            // See also SamplesPerPixel, AssociatedAlpha.
        },{
            "FillOrder",
            // The logical order of bits within a byte.
            TIFFTAG_FILLORDER,
            TIFF_SHORT
            // N = 1
            // 1 = pixels are arranged within a byte such that pixels with lower column values are
            // stored in the higher-order bits of the byte.
            // 1-bit uncompressed data example: Pixel 0 of a row is stored in the high-order bit
            // of byte 0, pixel 1 is stored in the next-highest bit, ..., pixel 7 is stored in the low-
            // order bit of byte 0, pixel 8 is stored in the high-order bit of byte 1, and so on.
            // CCITT 1-bit compressed data example: The high-order bit of the first compres-
            // sion code is stored in the high-order bit of byte 0, the next-highest bit of the first
            // compression code is stored in the next-highest bit of byte 0, and so on.
            // 2 = pixels are arranged within a byte such that pixels with lower column values are
            // stored in the lower-order bits of the byte.
            // We recommend that FillOrder=2 be used only in special-purpose applications. It
            // is easy and inexpensive for writers to reverse bit order by using a 256-byte lookup
            // table1 and the data is
            // either uncompressed or compressed using CCITT 1D or 2D compression, to
            // avoid potentially ambigous situations.
            // Support for FillOrder=2 is not required in a Baseline TIFF compliant reader
            // Default is FillOrder = 1.
            // FreeByteCounts
            // For each string of contiguous unused bytes in a TIFF file, the number of bytes in
            // the string.
            // Tag = 289  (121.H)
            // Type = LONG
            // Not recommended for general interchange.
            // See also FreeOffsets.
        },{
            "FreeOffsets",
            // For each string of contiguous unused bytes in a TIFF file, the byte offset of the
            // string.
            TIFFTAG_FREEOFFSETS,
            TIFF_LONG
            // Not recommended for general interchange.
            // See also FreeByteCounts.
        },{
            "GrayResponseCurve",
            // For grayscale data, the optical density of each possible pixel value.
            TIFFTAG_GRAYRESPONSECURVE,
            TIFF_SHORT
            // N = 2**BitsPerSample
            // The 0th value of GrayResponseCurve corresponds to the optical density of a pixel
            // having a value of 0, and so on.
            // This field may provide useful information for sophisticated applications, but it is
            // currently ignored by most TIFF readers.
            // See also GrayResponseUnit, PhotometricInterpretation.
        },{
            "GrayResponseUnit",
            // The precision of the information contained in the GrayResponseCurve.
            TIFFTAG_GRAYRESPONSEUNIT,
            TIFF_SHORT
            // N = 1
            // Because optical density is specified in terms of fractional numbers, this field is
            // necessary to interpret the stored integer information. For example, if
            // GrayScaleResponseUnits is set to 4 (ten-thousandths of a unit), and a
            // GrayScaleResponseCurve number for gray level 4 is 3455, then the resulting
            // actual value is 0.3455.
            // Optical densitometers typically measure densities within the range of 0.0 to 2.0.
            // 1 = Number represents tenths of a unit.
            // 2 = Number represents hundredths of a unit.
            // 3 = Number represents thousandths of a unit.
            // 4 =  Number represents ten-thousandths of a unit.
            // 5 = Number represents hundred-thousandths of a unit.
            // Modifies GrayResponseCurve.
            // See also GrayResponseCurve.
            // For historical reasons, the default is 2. However, for greater accuracy, 3 is recom-
            // mended.
        },{
            "HostComputer",
            // The computer and/or operating system in use at the time of image creation.
            TIFFTAG_HOSTCOMPUTER,
            TIFF_ASCII
            // See also Make, Model, Software.
        },{
            "ImageDescription",
            // A string that describes the subject of the image.
            TIFFTAG_IMAGEDESCRIPTION,
            TIFF_ASCII
            // For example, a user may wish to attach a comment such as “1988 company pic-
            // nic” to an image.
        },{
            "ImageLength",
            // The number of rows of pixels in the image.
            TIFFTAG_IMAGELENGTH,
            TIFF_LONG // or SHORT
            // N = 1
            // No default. See also ImageWidth.
        },{
            "ImageWidth",
            // The number of columns in the image, i.e., the number of pixels per row.
            TIFFTAG_IMAGEWIDTH,
            TIFF_LONG // or SHORT
            // N = 1
            // No default. See also ImageLength.
        },{
            "Make",
            // The scanner manufacturer.
            TIFFTAG_MAKE,
            TIFF_ASCII
            // Manufacturer of the scanner, video digitizer, or other type of equipment used to
            // generate the image. Synthetic images should not include this field.
            // See also Model, Software.
        },{
            "MaxSampleValue",
            // The maximum component value used.
            TIFFTAG_MAXSAMPLEVALUE,
            TIFF_SHORT
            // N = SamplesPerPixel
            // This field is not to be used to affect the visual appearance of an image when it is
            // displayed or printed. Nor should this field affect the interpretation of any other
            // field; it is used only for statistical purposes.
            // Default is 2**(BitsPerSample) - 1.
        },{
            "MinSampleValue",
            // The minimum component value used.
            TIFFTAG_MINSAMPLEVALUE,
            TIFF_SHORT
            // N = SamplesPerPixel
            // See also MaxSampleValue.
            // Default is 0.
        },{
            "Model",
            // The scanner model name or number.
            TIFFTAG_MODEL,
            TIFF_ASCII
            // The model name or number of the scanner, video digitizer, or other type of equip-
            // ment used to generate the image.
            // See also Make, Software.
        },{
            "NewSubfileType",
            // A general indication of the kind of data contained in this subfile.
            TIFFTAG_SUBFILETYPE,
            TIFF_LONG
            // N = 1 Replaces the old SubfileType field, due to
            // limitations in the definition of that field.
            // NewSubfileType is mainly useful when there are multiple
            // subfiles in a single TIFF file.  This field is made up of a
            // set of 32 flag bits. Unused bits are expected to be 0. Bit
            // 0 is the low-order bit.  Currently defined values are: Bit
            // 0 is 1 if the image is a reduced-resolution version of
            // another image in this TIFF file; else the bit is 0.  Bit 1
            // is 1 if the image is a single page of a multi-page image
            // (see the PageNumber field description); else the bit is 0.
            // Bit 2 is 1 if the image defines a transparency mask for
            // another image in this TIFF file.  The
            // PhotometricInterpretation value must be 4, designating a
            // transparency mask.  These values are defined as bit flags
            // because they are independent of each other.  Default is 0.
            //
            // From DNG Spec: In DNG versions earlier than 1.2.0.0, full
            // resolution raw images should use NewSubFileType equal to
            // 0. Rendered previews or reduced resolution versions of raw
            // images should use NewSubFileType equal to 1. DNG 1.2.0.0
            // allows a new value for NewSubFileType equal to
            // 10001.H. This value, used for alternative or non-primary
            // rendered previews, allows for multiple renderings (not just
            // multiple sizes of a single rendering) to be stored in a DNG
            // file. DNG reading software that displays a preview for a
            // DNG file should, by default, display a preview from an IFD
            // with NewSubFileType equal to 1. Alternative renderings
            // should only be displayed if requested by the user.

        },{
            "Orientation",
            // The orientation of the image with respect to the rows and columns.
            TIFFTAG_ORIENTATION,
            TIFF_SHORT
            // N = 1
            // 1 = The 0th row represents the visual top of the image, and the 0th column represents
            // the visual left-hand side.
            // 2 = The 0th row represents the visual top of the image, and the 0th column represents
            // the visual right-hand side.
            // 3 = The 0th row represents the visual bottom of the image, and the 0th column repre-
            // sents the visual right-hand side.
            // 4 = The 0th row represents the visual bottom of the image, and the 0th column repre-
            // sents the visual left-hand side.
            // 5 = The 0th row represents the visual left-hand side of the image, and the 0th column
            // represents the visual top.
            // 6 = The 0th row represents the visual right-hand side of the image, and the 0th column
            // represents the visual top.
            // 7 = The 0th row represents the visual right-hand side of the image, and the 0th column
            // represents the visual bottom.
            // 8 = The 0th row represents the visual left-hand side of the image, and the 0th column
            // represents the visual bottom.
            // Default is 1.
            // Support for orientations other than 1 is not a Baseline TIFF requirement.
            //
            // From DNG spec:
            // Orientation is a required tag for DNG. With the Orientation
            // tag present, file browsers can perform lossless rotation of
            // DNG files by modifying a single byte of the file. DNG
            // readers should support all possible orientations, including
            // mirrored orientations. Note that the mirrored orientations
            // are not allowed by the TIFF-EP specification, so writers
            // should not use them if they want their files be compatible
            // with both specifications.
        },{
            "PhotometricInterpretation",
            // The color space of the image data.
            TIFFTAG_PHOTOMETRIC,
            TIFF_SHORT
            // N = 1
            // 0 = WhiteIsZero. For bilevel and grayscale images:  0 is imaged as white.
            // 2**BitsPerSample-1 is imaged as black. This is the normal value for Compres-
            // sion=2.
            // 1 =  BlackIsZero. For bilevel and grayscale images:  0 is imaged as black.
            // 2**BitsPerSample-1 is imaged as white. If this value is specified for Compres-
            // sion=2, the image should display and print reversed.
            // 2 = RGB. In the RGB model, a color is described as a combination of the three pri-
            // mary colors of light (red, green, and blue) in particular concentrations. For each of
            // the three components, 0 represents minimum intensity, and 2**BitsPerSample - 1
            // represents maximum intensity. Thus an RGB value of (0,0,0) represents black,
            // and (255,255,255) represents white, assuming 8-bit components. For
            // PlanarConfiguration = 1, the components are stored in the indicated order:  first
            // Red, then Green, then Blue2, the StripOffsets for the
            // component planes are stored in the indicated order:  first the Red component plane
            // StripOffsets, then the Green plane StripOffsets, then the Blue plane StripOffsets.
            // 3= Palette color.  In this model, a color is described with a single component. The
            // value of the component is used as an index into the red, green and blue curves in
            // the ColorMap field to retrieve an RGB triplet that defines the color. When
            // PhotometricInterpretation=3 is used, ColorMap must be present and
            // SamplesPerPixel must be 1.
            // 4 =  Transparency Mask.
            // This means that the image is used to define an irregularly shaped region of another
            // image in the same TIFF file. SamplesPerPixel and BitsPerSample must be 1.
            // PackBits compression is recommended. The 1-bits define the interior of the re-
            // gion; the 0-bits define the exterior of the region.
            // A reader application can use the mask to determine which parts of the image to
            // display. Main image pixels that correspond to 1-bits in the transparency mask are
            // imaged to the screen or printer, but main image pixels that correspond to 0-bits in
            // the mask are not displayed or printed.
            // The image mask is typically at a higher resolution than the main image, if the
            // main image is grayscale or color so that the edges can be sharp.
            // There is no default for PhotometricInterpretation, and it is required. Do not rely
            // on applications defaulting to what you want.
            //
            // From DNG Spec:

            // The following values are supported for thumbnail and preview IFDs only:
            // 1 = BlackIsZero. Assumed to be in a gamma 2.2 color space, unless otherwise specified using PreviewColorSpace tag.
            // 2 = RGB. Assumed to be in the sRGB color space, unless otherwise specified using the PreviewColorSpace tag.
            // 6 = YCbCr. Used for JPEG encoded preview images.
            // The following values are supported for the raw IFD, and are assumed to be the camera's native color space:
            // 32803 = CFA (Color Filter Array).
            // 34892 = LinearRaw.
            // The CFA PhotometricInterpretation value is documented in
            // the TIFF-EP specification. Its use requires the use of the
            // CFARepeatPatternDim and CFAPattern tags in the same
            // IFD. The origin of the repeating CFA pattern is the
            // top-left corner of the ActiveArea rectangle. The LinearRaw
            // PhotometricInterpretation value is intended for use by
            // cameras that do not use color filter arrays, but instead
            // capture all color components at each pixel. It can also be
            // used for CFA data that has already been de-mosaiced. The
            // LinearRaw value can be used in reduced resolution IFDs,
            // even if the raw IFD uses the CFA PhotometricInterpretation
            // value.
        },{
            "PlanarConfiguration",
            // How the components of each pixel are stored.
            TIFFTAG_PLANARCONFIG,
            TIFF_SHORT
            // N = 1
            // 1 =  Chunky format. The component values for each pixel are stored contiguously.
            // The order of the components within the pixel is specified by
            // PhotometricInterpretation. For example, for RGB data, the data is stored as
            // RGBRGBRGB…
            // 2 = Planar format. The components are stored in separate “component planes.”  The
            // values in StripOffsets and StripByteCounts are then arranged as a 2-dimensional
            // array, with SamplesPerPixel rows and StripsPerImage columns. (All of the col-
            // umns for row 0 are stored first, followed by the columns of row 1, and so on.)
            // PhotometricInterpretation describes the type of data stored in each component
            // plane. For example, RGB data is stored with the Red components in one compo-
            // nent plane, the Green in another, and the Blue in another.
            // PlanarConfiguration=2 is not currently in widespread use and it is not recom-
            // mended for general interchange. It is used as an extension and Baseline TIFF
            // readers are not required to support it.
            // If SamplesPerPixel is 1, PlanarConfiguration is irrelevant, and need not be in-
            // cluded.
            // If a row interleave effect is desired, a writer might write out the data as
            // PlanarConfiguration=2—separate sample planes—but break up the planes into
            // multiple strips (one row per strip, perhaps) and interleave the strips.
            // Default is 1. See also BitsPerSample, SamplesPerPixel.
        },{
            "ResolutionUnit",
            // The unit of measurement for XResolution and YResolution.
            TIFFTAG_RESOLUTIONUNIT,
            TIFF_SHORT
            // N = 1
            // To be used with XResolution and YResolution.
            // 1 = No absolute unit of measurement. Used for images that may have a non-square
            // aspect ratio, but no meaningful absolute dimensions.
            // The drawback of ResolutionUnit=1 is that different applications will import the image
            // at different sizes. Even if the decision is arbitrary, it might be better to use dots per
            // inch or dots per centimeter, and to pick XResolution and YResolution so that the
            // aspect ratio is correct and the maximum dimension of the image is about four inches
            // (the “four” is arbitrary.)
            // 2 =  Inch.
            // 3 =  Centimeter.
            // Default is 2.
        },{
            "RowsPerStrip",
            // The number of rows per strip.
            TIFFTAG_ROWSPERSTRIP,
            TIFF_LONG // or SHORT
            // N = 1
            // TIFF image data is organized into strips for faster random access and efficient I/O
            // buffering.
            // RowsPerStrip and ImageLength together tell us the number of strips in the entire
            // image. The equation is:
            // StripsPerImage = floor ((ImageLength + RowsPerStrip - 1) / RowsPerStrip).
            // StripsPerImage is not a field. It is merely a value that a TIFF reader will want to
            // compute because it specifies the number of StripOffsets and StripByteCounts for the
            // image.
            // Note that either SHORT or LONG values can be used to specify RowsPerStrip.
            // SHORT values may be used for small TIFF files. It should be noted, however, that
            // earlier TIFF specification revisions required LONG values and that some software
            // may not accept SHORT values.
            // The default is 2**32 - 1, which is effectively infinity. That is, the entire image is
            // one strip.
            // Use of a single strip is not recommended. Choose RowsPerStrip such that each strip is
            // about 8K bytes, even if the data is not compressed, since it makes buffering simpler
            // for readers. The “8K” value is fairly arbitrary, but seems to work well.
            // See also ImageLength, StripOffsets, StripByteCounts, TileWidth, TileLength,
            // TileOffsets, TileByteCounts.
        },{
            "SamplesPerPixel",
            // The number of components per pixel.
            TIFFTAG_SAMPLESPERPIXEL,
            TIFF_SHORT
            // N = 1
            // SamplesPerPixel is usually 1 for bilevel, grayscale, and palette-color images.
            // SamplesPerPixel is usually 3 for RGB images.
            // Default = 1. See also BitsPerSample, PhotometricInterpretation, ExtraSamples.
        },{
            "Software",
            // Name and version number of the software package(s) used to create the image.
            TIFFTAG_SOFTWARE,
            TIFF_ASCII
            // See also Make, Model.
        },{
            "StripByteCounts",
            // For each strip, the number of bytes in the strip after compression.
            TIFFTAG_STRIPBYTECOUNTS,
            TIFF_LONG // or SHORT, but make sure to write LONG
            // N = StripsPerImage for PlanarConfiguration equal to 1.
            // = SamplesPerPixel * StripsPerImage for PlanarConfiguration equal to 2
            // This tag is required for Baseline TIFF files.
            // No default.
            // See also StripOffsets, RowsPerStrip, TileOffsets, TileByteCounts.
        },{
            "StripOffsets",
            // For each strip, the byte offset of that strip.
            TIFFTAG_STRIPOFFSETS,
            TIFF_LONG // or SHORT, but make sure to write LONG
            // N = StripsPerImage for PlanarConfiguration equal to 1.
            // = SamplesPerPixel * StripsPerImage for PlanarConfiguration equal to 2
            // The offset is specified with respect to the beginning of the TIFF file. Note that this
            // implies that each strip has a location independent of the locations of other strips.
            // This feature may be useful for editing applications. This required field is the only
            // way for a reader to find the image data. (Unless TileOffsets is used; see
            // TileOffsets.)
            // Note that either SHORT or LONG values may be used to specify the strip offsets.
            // SHORT values may be used for small TIFF files. It should be noted, however, that
            // earlier TIFF specifications required LONG strip offsets and that some software
            // may not accept SHORT values.
            // For maximum compatibility with operating systems such as MS-DOS and Win-
            // dows, the StripOffsets array should be less than or equal to 64K bytes in length,
            // and the strips themselves, in both compressed and uncompressed forms, should
            // not be larger than 64K bytes.
            // No default. See also StripByteCounts, RowsPerStrip, TileOffsets,
            // TileByteCounts.
        },{
            "SubfileType",
            // A general indication of the kind of data contained in this subfile.
            TIFFTAG_OSUBFILETYPE,
            TIFF_SHORT
            // N = 1
            // Currently defined values are:
            // 1 = full-resolution image data
            // 2 = reduced-resolution image data
            // 3 = a single page of a multi-page image (see the PageNumber field description).
            // Note that several image types may be found in a single TIFF file, with each subfile
            // described by its own IFD.
            // No default.
            // This field is deprecated. The NewSubfileType field should be used instead.
        },{
            "Threshholding",
            // For black and white TIFF files that represent shades of gray, the technique used to
            // convert from gray to black and white pixels.
            TIFFTAG_THRESHHOLDING,
            TIFF_SHORT
            // N = 1
            // 1 = No dithering or halftoning has been applied to the image data.
            // 2 = An ordered dither or halftone technique has been applied to the image data.
            // 3 = A randomized process such as error diffusion has been applied to the image data.
            // Default is Threshholding = 1. See also CellWidth, CellLength.
        },{
            "XResolution",
            // The number of pixels per ResolutionUnit in the ImageWidth direction.
            TIFFTAG_XRESOLUTION,
            TIFF_RATIONAL
            // N = 1
            // It is not mandatory that the image be actually displayed or printed at the size implied
            // by this parameter. It is up to the application to use this information as it wishes.
            // No default. See also YResolution, ResolutionUnit.
        },{
            "YResolution",
            // The number of pixels per ResolutionUnit in the ImageLength direction.
            TIFFTAG_YRESOLUTION ,
            TIFF_RATIONAL
            // N = 1
            // No default. See also XResolution, ResolutionUnit.
            //
        },
        //// SUPPLEMENT 1 TAGS
        {
            "SubIFDs",
            TIFFTAG_SUBIFD,
            TIFF_IFD // or “LONG”. “IFD” is preferred.
            // Note: When writing, TIFF_IFD tags are always saved as LONG for compatibility with dcraw and dcraw-derived applications.
            // N = number of child IFDs
            // Each value is an offset (from the beginning of the TIFF file, as always) to a child
            // IFD. Child images provide extra information for the parent image—such as a
            // subsampled version of the parent image.
            // TIFF data type 13, “IFD,” is otherwise identical to LONG, but is only used to
            // point to other valid IFDs.
        },
        //// TIFF Extension tags
        {
            "TileWidth",
            TIFFTAG_TILEWIDTH,
            TIFF_SHORT // or LONG
            // N = 1
            // The tile width in pixels.  This is the number of columns in each tile.
            // Assuming integer arithmetic, three computed values that are useful in the follow-
            // ing field descriptions are:
            // TilesAcross = (ImageWidth + TileWidth - 1) / TileWidth
            // TilesDown = (ImageLength + TileLength - 1) / TileLength
            // TilesPerImage = TilesAcross * TilesDown
            // These computed values are not TIFF fields; they are simply values determined by
            // the ImageWidth, TileWidth, ImageLength, and TileLength fields.
            // TileWidth and ImageWidth together determine the number of tiles that span the
            // width of the image (TilesAcross). TileLength and ImageLength together deter-
            // mine the number of tiles that span the length of the image (TilesDown).
            // We recommend choosing TileWidth and TileLength such that the resulting tiles
            // are about 4K to 32K bytes before compression. This seems to be a reasonable
            // value for most applications and compression schemes.
            // TileWidth must be a multiple of 16. This restriction improves performance in
            // some graphics environments and enhances compatibility with compression
            // schemes such as JPEG.
            // Tiles need not be square.
            // Note that ImageWidth can be less than TileWidth, although this means that the
            // tiles are too large or that you are using tiling on really small images, neither of
            // which is recommended. The same observation holds for ImageLength and
            // TileLength.
            // No default. See also TileLength, TileOffsets, TileByteCounts.
        },{
            "TileLength",
            TIFFTAG_TILELENGTH,
            TIFF_SHORT // or LONG
            // N = 1
            // The tile length (height) in pixels. This is the number of rows in each tile.
            // TileLength must be a multiple of 16 for compatibility with compression schemes
            // such as JPEG.
            // Replaces RowsPerStrip in tiled TIFF files.
            // No default. See also TileWidth, TileOffsets, TileByteCounts.
        },{
            "TileOffsets",
            TIFFTAG_TILEOFFSETS,
            TIFF_LONG
            // N = TilesPerImage for PlanarConfiguration = 1
            // = SamplesPerPixel * TilesPerImage for PlanarConfiguration = 2
            // For each tile, the byte offset of that tile, as compressed and stored on disk. The
            // offset is specified with respect to the beginning of the TIFF file. Note that this
            // implies that each tile has a location independent of the locations of other tiles.
            // Offsets are ordered left-to-right and top-to-bottom. For PlanarConfiguration = 2,
            // the offsets for the first component plane are stored first, followed by all the offsets
            // for the second component plane, and so on.
            // No default. See also TileWidth, TileLength, TileByteCounts.
        },{
            "TileByteCounts",
            TIFFTAG_TILEBYTECOUNTS,
            TIFF_SHORT // or LONG
            // N = TilesPerImage for PlanarConfiguration = 1
            // = SamplesPerPixel * TilesPerImage for PlanarConfiguration = 2
            // For each tile, the number of (compressed) bytes in that tile.
            // See TileOffsets for a description of how the byte counts are ordered.
            // No default. See also TileWidth, TileLength, TileOffsets.
        },
        // TIFF/EP Tags (knowledge inferred from many places, ISO spec costs money)
        {
            "TIFF/EPStandardID",
            37398,
            TIFF_BYTE,
            // Note: Below inferred from various sources
            // Count:
            // 4
            // Value:
            // See below
            // Default:
            // Required tag
            // Usage:
            // IFD 0
            // Description:
            // Should always be 1 0 0 0
        },
        {
            "CFAPattern",
            TIFFTAG_CFAPATTERN,
            TIFF_BYTE
            // Count                Number of pixels in a single CFA pattern repetition
            // Default                None
            // Description
            // Indicates the color filter array (CFA) geometric pattern of the image sensor when a one-chip color area sensor is used.
            // It does not apply to all sensing methods.
            // The value consists of an id code for every pixel in that pattern. The pattern shape is given by CFARepeatPatternDim, the pixels
            // are listed left-to-right, top-to-bottom, with the first pattern starting at the top-left corner of the sensor.
            // This identification code can be:
            // 0 = Red
            // 1 = Green
            // 2 = Blue
            // 3 = Cyan
            // 4 = Magenta
            // 5 = Yellow
            // 6 = White
        },        {
            "CFARepeatPatternDim",
            TIFFTAG_CFAREPEATPATTERNDIM,
            TIFF_SHORT
            // Count 2
            // Default ?
            // Description
            // (inferred from Boris's saving code, web info - Eddy)
            // Dimensions of the CFA repeating pattern, as two shorts. The colors that make up the pattern are given by CFAPattern
        },
        //// DNG TAGS
        {
            "DNGVersion",
            TIFFTAG_DNGVERSION,
            TIFF_BYTE,
            // Count:
            // 4
            // Value:
            // See below
            // Default:
            // Required tag
            // Usage:
            // IFD 0
            // Description:
            // This tag encodes the DNG four-tier version number. For
            // files compliant with this version of the DNG specification
            // (1.2.0.0), this tag should contain the bytes: 1, 2, 0, 0.
        },{
            "DNGBackwardVersion",
            TIFFTAG_DNGBACKWARDVERSION,
            TIFF_BYTE,
            // Count:
            // 4
            // Value:
            // See below
            // Default:
            // DNGVersion with the last two bytes set to zero.
            // Usage:
            // IFD 0
            // Description:
            // This tag specifies the oldest version of the Digital
            // Negative specification for which a file is
            // compatible. Readers should not attempt to read a file if
            // this tag specifies a version number that is higher than the
            // version number of the specification the reader was based
            // on.  In addition to checking the version tags, readers
            // should, for all tags, check the types, counts, and values,
            // to verify it is able to correctly read the file.  For more
            // information on compatibility with previous DNG versions,
            // see Appendix A: Compatibility with Previous Versions.
        },{
            "UniqueCameraModel",
            TIFFTAG_UNIQUECAMERAMODEL,
            TIFF_ASCII,
            // Count:
            // String length including null
            // Value:
            // Null terminated string
            // Default:
            // Required tag
            // Usage:
            // IFD 0
            // Description:
            // UniqueCameraModel defines a unique, non-localized name for
            // the camera model that created the image in the raw
            // file. This name should include the manufacturer's name to
            // avoid conflicts, and should not be localized, even if the
            // camera name itself is localized for different markets (see
            // LocalizedCameraModel).
            // This string may be used by reader software to index into
            // per-model preferences and replacement profiles.  Examples
            // of unique model names are:
            // •"Canon EOS 300D"
            // •"Fujifilm FinePix S2Pro"
            // •"Kodak ProBack645"
            // •"Minolta DiMAGE A1"
            // •"Nikon D1X"
            // •"Olympus C-5050Z"
            // •"Pentax *istD"
            // •"Sony F828"
        },{
            "LocalizedCameraModel",
            TIFFTAG_LOCALIZEDCAMERAMODEL,
            TIFF_ASCII //or BYTE,
            // Count:
            // Byte count including null
            // Value:
            // Null terminated UTF-8 encoded Unicode string
            // Default:
            // Same as UniqueCameraModel
            // Usage:
            // IFD 0
            // Description:
            // Similar to the UniqueCameraModel field, except the name can
            // be localized for different markets to match the
            // localization of the camera name.
        },{
            "CFAPlaneColor",
            TIFFTAG_CFAPLANECOLOR,
            TIFF_BYTE,
            // Count:
            // ColorPlanes
            // Value:
            // See below
            // Default:
            // 0, 1, 2 (red, green, blue)
            // Usage:
            // Raw IFD
            // Description:
            // CFAPlaneColor provides a mapping between the values in the
            // CFAPattern tag and the plane numbers in LinearRaw
            // space. This is a required tag for non-RGB CFA images
        },{
            "CFALayout",
            TIFFTAG_CFALAYOUT,
            TIFF_SHORT,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 1
            // Usage:
            // Raw IFD
            // Description:
            // CFALayout describes the spatial layout of the CFA. The currently defined values are:
            // 1 = Rectangular (or square) layout
            // 2 = Staggered layout A: even columns are offset down by 1/2 row
            // 3 = Staggered layout B: even columns are offset up by 1/2 row
            // 4 = Staggered layout C: even rows are offset right by 1/2 column
            // 5 = Staggered layout D: even rows are offset left by 1/2 column
            // 6 = Staggered layout E: even rows are offset up by 1/2 row, even columns are offset left by 1/2 column
            // 7 = Staggered layout F: even rows are offset up by 1/2 row, even columns are offset right by 1/2 column
            // 8 = Staggered layout G: even rows are offset down by 1/2 row, even columns are offset left by 1/2 column
            // 9 = Staggered layout H: even rows are offset down by 1/2 row, even columns are offset right by 1/2 column
            // Note that for the purposes of this tag, rows and columns are numbered starting with one.
            // Layout values 6 through 9 were added with DNG version 1.3.0.0.
        },{
            "LinearizationTable",
            TIFFTAG_LINEARIZATIONTABLE,
            TIFF_SHORT,
            // Count:
            // N
            // Value:
            // See below
            // Default:
            // Identity table (0, 1, 2, 3, etc.)
            // Usage:
            // Raw IFD
            // Description:
            // LinearizationTable describes a lookup table that maps
            // stored values into linear values. This tag is typically
            // used to increase compression ratios by storing the raw data
            // in a non-linear, more visually uniform space with fewer
            // total encoding levels.  If SamplesPerPixel is not equal to
            // one, this single table applies to all the samples for each
            // pixel.  See Chapter 5, “Mapping Raw Values to Linear
            // Reference Values” on page 65 for details of the processing
            // model.
        },{
            "BlackLevelRepeatDim",
            TIFFTAG_BLACKLEVELREPEATDIM,
            TIFF_SHORT,
            // Count:
            // 2
            // Value:
            // Value: 0: BlackLevelRepeatRows
            // Value: 1: BlackLevelRepeatCols
            // Default:
            // 1, 1
            // Usage:
            // Raw IFD
            // Description:
            // This tag specifies repeat pattern size for the BlackLevel tag.
        },{
            "BlackLevel",
            TIFFTAG_BLACKLEVEL,
            TIFF_SHORT // or LONG or RATIONAL,
            // Count:
            // BlackLevelRepeatRows * BlackLevelRepeatCols * SamplesPerPixel
            // Value:
            // See below
            // Default:
            // 0
            // Usage:
            // Raw IFD
            // Description:
            // This tag specifies the zero light (a.k.a. thermal black or
            // black current) encoding level, as a repeating pattern. The
            // origin of this pattern is the top-left corner of the
            // ActiveArea rectangle. The values are stored in
            // row-column-sample scan order.  See Chapter 5, “Mapping Raw
            // Values to Linear Reference Values” on page 65 for details
            // of the processing model.
        },{
            "BlackLevelDeltaH",
            TIFFTAG_BLACKLEVELDELTAH,
            TIFF_SRATIONAL,
            // Count:
            // ActiveArea width
            // Value:
            // See below
            // Default:
            // All zeros
            // Usage:
            // Raw IFD
            // Description:
            // If the zero light encoding level is a function of the image
            // column, BlackLevelDeltaH specifies the difference between
            // the zero light encoding level for each column and the
            // baseline zero light encoding level.  If SamplesPerPixel is
            // not equal to one, this single table applies to all the
            // samples for each pixel.  See Chapter 5, “Mapping Raw Values
            // to Linear Reference Values” on page 65 for details of the
            // processing model.
        },{
            "BlackLevelDeltaV",
            TIFFTAG_BLACKLEVELDELTAV,
            TIFF_SRATIONAL,
            // Count:
            // ActiveArea length
            // Value:
            // See below
            // Default:
            // All zeros
            // Usage:
            // Raw IFD
            // Description:
            // If the zero light encoding level is a function of the image
            // row, this tag specifies the difference between the zero
            // light encoding level for each row and the baseline zero
            // light encoding level.  If SamplesPerPixel is not equal to
            // one, this single table applies to all the samples for each
            // pixel.  See Chapter 5, “Mapping Raw Values to Linear
            // Reference Values” on page 65 for details of the processing
            // model.
        },{
            "WhiteLevel",
            TIFFTAG_WHITELEVEL,
            TIFF_SHORT //or LONG,
            // Count:
            // SamplesPerPixel
            // Value:
            // See below
            // Default:
            // (2 ** BitsPerSample) - 1
            // Usage:
            // Raw IFD
            // Description:
            // This tag specifies the fully saturated encoding level for
            // the raw sample values. Saturation is caused either by the
            // sensor itself becoming highly non-linear in response, or by
            // the camera's analog to digital converter clipping.  See
            // Chapter 5, “Mapping Raw Values to Linear Reference Values”
            // on page 65 for details of the processing model.
        },{
            "DefaultScale",
            TIFFTAG_DEFAULTSCALE,
            TIFF_RATIONAL,
            // Count:
            // 2
            // Value:
            // Value: 0: DefaultScaleH
            // Value: 1: DefaultScaleV
            // Default:
            // 1.0, 1.0
            // Usage:
            // Raw IFD
            // Description:
            // DefaultScale is required for cameras with non-square
            // pixels. It specifies the default scale factors for each
            // direction to convert the image to square pixels. Typically
            // these factors are selected to approximately preserve total
            // pixel count.  For CFA images that use CFALayout equal to 2,
            // 3, 4, or 5, such as the Fujifilm SuperCCD, these two values
            // should usually differ by a factor of 2.0.
        },{
            "BestQualityScale",
            TIFFTAG_BESTQUALITYSCALE,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // see below
            // Default:
            // 1.0
            // Usage:
            // Raw IFD
            // Description:
            // For some cameras, the best possible image quality is not
            // achieved by preserving the total pixel count during
            // conversion. For example, Fujifilm SuperCCD images have
            // maximum detail when their total pixel count is doubled.
            // This tag specifies the amount by which the values of the
            // DefaultScale tag need to be multiplied to achieve the best
            // quality image size.
        },{
            "DefaultCropOrigin",
            TIFFTAG_DEFAULTCROPORIGIN,
            TIFF_SHORT //or LONG or RATIONAL,
            // Count:
            // 2
            // Value:
            // Value: 0: DefaultCropOriginH
            // Value: 1: DefaultCropOriginV
            // Default:
            // 0, 0
            // Usage:
            // Raw IFD
            // Description:
            // Raw images often store extra pixels around the edges of the
            // final image. These extra pixels help prevent interpolation
            // artifacts near the edges of the final image.
            // Default:CropOrigin specifies the origin of the final image
            // area, in raw image coordinates (i.e., before the
            // DefaultScale has been applied), relative to the top-left
            // corner of the ActiveArea rectangle.
        },{
            "DefaultCropSize",
            TIFFTAG_DEFAULTCROPSIZE,
            TIFF_SHORT // or LONG or RATIONAL,
            // Count:
            // 2
            // Value:
            // Value: 0: DefaultCropSizeH
            // Value: 1: DefaultCropSizeV
            // Default:
            // ImageWidth, ImageLength
            // Usage:
            // Raw IFD
            // Description:
            // Raw images often store extra pixels around the edges of the
            // final image. These extra pixels help prevent interpolation
            // artifacts near the edges of the final image.
            // Default:CropSize specifies the size of the final image
            // area, in raw image coordinates (i.e., before the
            // DefaultScale has been applied).
        },{
            "CalibrationIlluminant1",
            TIFFTAG_CALIBRATIONILLUMINANT1,
            TIFF_SHORT,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 0 (unknown)
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // The illuminant used for the first set of color calibration
            // tags. The legal values for this tag are the same as the
            // legal values for the LightSource EXIF tag.  See Chapter 6,
            // “Mapping Camera Color Space to CIE XYZ Space” on page 67
            // for details of the color-processing model.
        },{
            "CalibrationIlluminant2",
            TIFFTAG_CALIBRATIONILLUMINANT2,
            TIFF_SHORT,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // The illuminant used for an optional second set of color
            // calibration tags. The legal values for this tag are the
            // same as the legal values for the CalibrationIlluminant1
            // tag; however, if both are included, neither is allowed to
            // have a value of 0 (unknown).  See Chapter 6, “Mapping
            // Camera Color Space to CIE XYZ Space” on page 67 for details
            // of the color-processing model.
        },{
            "ColorMatrix1",
            TIFFTAG_COLORMATRIX1,
            TIFF_SRATIONAL,
            // Count:
            // ColorPlanes * 3
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // ColorMatrix1 defines a transformation matrix that converts
            // XYZ values to reference camera native color space values,
            // under the first calibration illuminant. The matrix values
            // are stored in row scan order.  The ColorMatrix1 tag is
            // required for all non-monochrome DNG files.  See Chapter 6,
            // “Mapping Camera Color Space to CIE XYZ Space” on page 67
            // for details of the color-processing model.
        },{
            "ColorMatrix2",
            TIFFTAG_COLORMATRIX2,
            TIFF_SRATIONAL,
            // Count:
            // ColorPlanes * 3
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // ColorMatrix2 defines a transformation matrix that converts
            // XYZ values to reference camera native color space values,
            // under the second calibration illuminant. The matrix values
            // are stored in row scan order.  See Chapter 6, “Mapping
            // Camera Color Space to CIE XYZ Space” on page 67 for details
            // of the color-processing model.
        },{
            "CameraCalibration1",
            TIFFTAG_CAMERACALIBRATION1,
            TIFF_SRATIONAL,
            // Count:
            // ColorPlanes * ColorPlanes
            // Value:
            // See below
            // Default:
            // Identity matrix
            // Usage:
            // IFD 0
            // Description:
            // CameraCalibration1 defines a calibration matrix that
            // transforms reference camera native space values to
            // individual camera native space values under the first
            // calibration illuminant. The matrix is stored in row scan
            // order.  This matrix is stored separately from the matrix
            // specified by the ColorMatrix1 tag to allow raw converters
            // to swap in replacement color matrices based on
            // UniqueCameraModel tag, while still taking advantage of any
            // per-individual camera calibration performed by the camera
            // manufacturer.See Chapter 6, “Mapping Camera Color Space to
            // CIE XYZ Space” on page 67 for details of the
            // color-processing model.
        },{
            "CameraCalibration2",
            TIFFTAG_CAMERACALIBRATION2,
            TIFF_SRATIONAL,
            // Count:
            // ColorPlanes * ColorPlanes
            // Value:
            // See below
            // Default:
            // Identity matrix
            // Usage:
            // IFD 0
            // Description:
            // CameraCalibration2 defines a calibration matrix that
            // transforms reference camera native space values to
            // individual camera native space values under the second
            // calibration illuminant. The matrix is stored in row scan
            // order.  This matrix is stored separately from the matrix
            // specified by the ColorMatrix2 tag to allow raw converters
            // to swap in replacement color matrices based on
            // UniqueCameraModel tag, while still taking advantage of any
            // per-individual camera calibration performed by the camera
            // manufacturer.  See Chapter 6, “Mapping Camera Color Space
            // to CIE XYZ Space” on page 67 for details of the
            // color-processing model.
        },{
            "ReductionMatrix1",
            TIFFTAG_REDUCTIONMATRIX1,
            TIFF_SRATIONAL,
            // Count:
            // 3 * ColorPlanes
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // ReductionMatrix1 defines a dimensionality reduction matrix
            // for use as the first stage in converting color camera
            // native space values to XYZ values, under the first
            // calibration illuminant. This tag may only be used if
            // ColorPlanes is greater than 3. The matrix is stored in row
            // scan order.  See Chapter 6, “Mapping Camera Color Space to
            // CIE XYZ Space” on page 67 for details of the
            // color-processing model.
        },{
            "ReductionMatrix2",
            TIFFTAG_REDUCTIONMATRIX2,
            TIFF_SRATIONAL,
            // Count:
            // 3 * ColorPlanes
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // ReductionMatrix2 defines a dimensionality reduction matrix
            // for use as the first stage in converting color camera
            // native space values to XYZ values, under the second
            // calibration illuminant. This tag may only be used if
            // ColorPlanes is greater than 3. The matrix is stored in row
            // scan order.  See Chapter 6, “Mapping Camera Color Space to
            // CIE XYZ Space” on page 67 for details of the
            // color-processing model.
        },{
            "AnalogBalance",
            TIFFTAG_ANALOGBALANCE,
            TIFF_RATIONAL,
            // Count:
            // ColorPlanes
            // Value:
            // See below
            // Default:
            // All 1.0
            // Usage:
            // IFD 0
            // Description:
            // Normally the stored raw values are not white balanced,
            // since any digital white balancing will reduce the dynamic
            // range of the final image if the user decides to later
            // adjust the white balance; however, if camera hardware is
            // capable of white balancing the color channels before the
            // signal is digitized, it can improve the dynamic range of
            // the final image.
            // AnalogBalance defines the gain, either analog (recommended)
            // or digital (not recommended) that has been applied the
            // stored raw values.
            // See Chapter 6, “Mapping Camera Color Space to CIE XYZ
            // Space” on page 67 for details of the color-processing
            // model.
        },{
            "AsShotNeutral",
            TIFFTAG_ASSHOTNEUTRAL,
            TIFF_SHORT //or RATIONAL,
            // Count:
            // ColorPlanes
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0
            // Description:
            // AsShotNeutral specifies the selected white balance at time
            // of capture, encoded as the coordinates of a perfectly
            // neutral color in linear reference space values. The
            // inclusion of this tag precludes the inclusion of the
            // AsShotWhiteXY tag.
        },{
            "AsShotWhiteXY",
            TIFFTAG_ASSHOTWHITEXY,
            TIFF_RATIONAL,
            // Count:
            // 2
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0
            // Description:
            // AsShotWhiteXY specifies the selected white balance at time
            // of capture, encoded as x-y chromaticity coordinates. The
            // inclusion of this tag precludes the inclusion of the
            // AsShotNeutral tag.
        },{
            "BaselineExposure",
            TIFFTAG_BASELINEEXPOSURE,
            TIFF_SRATIONAL,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 0.0
            // Usage:
            // IFD 0
            // Description:
            // Camera models vary in the trade-off they make between
            // highlight headroom and shadow noise. Some leave a
            // significant amount of highlight headroom during a normal
            // exposure. This allows significant negative exposure
            // compensation to be applied during raw conversion, but also
            // means normal exposures will contain more shadow
            // noise. Other models leave less headroom during normal
            // exposures. This allows for less negative exposure
            // compensation, but results in lower shadow noise for normal
            // exposures.
            // Because of these differences, a raw converter needs to vary
            // the zero point of its exposure compensation control from
            // model to model. BaselineExposure specifies by how much (in
            // EV units) to move the zero point. Positive values result in
            // brighter default results, while negative values result in
            // darker default results.
        },{
            "BaselineNoise",
            TIFFTAG_BASELINENOISE,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 1.0
            // Usage:
            // IFD 0
            // Description:
            // BaselineNoise specifies the relative noise level of the
            // camera model at a baseline ISO value of 100, compared to a
            // reference camera model.
            // Since noise levels tend to vary approximately with the
            // square root of the ISO value, a raw converter can use this
            // value, combined with the current ISO, to estimate the
            // relative noise level of the current image.
        },{
            "BaselineSharpness",
            TIFFTAG_BASELINESHARPNESS,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 1.0
            // Usage:
            // IFD 0
            // Description:
            // BaselineSharpness specifies the relative amount of
            // sharpening required for this camera model, compared to a
            // reference camera model. Camera models vary in the strengths
            // of their anti-aliasing filters. Cameras with weak or no
            // filters require less sharpening than cameras with strong
            // anti-aliasing filters.
        },{
            "BayerGreenSplit",
            TIFFTAG_BAYERGREENSPLIT,
            TIFF_LONG,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 0
            // Usage:
            // Raw IFD
            // Description:
            // BayerGreenSplit only applies to CFA images using a Bayer
            // pattern filter array. This tag specifies, in arbitrary
            // units, how closely the values of the green pixels in the
            // blue/green rows track the values of the green pixels in the
            // red/green rows.
            // A value of zero means the two kinds of green pixels track
            // closely, while a non-zero value means they sometimes
            // diverge. The useful range for this tag is from 0 (no
            // divergence) to about 5000 (quite large divergence).
        },{
            "LinearResponseLimit",
            TIFFTAG_LINEARRESPONSELIMIT,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 1.0
            // Usage:
            // IFD 0
            // Description:
            // Some sensors have an unpredictable non-linearity in their
            // response as they near the upper limit of their encoding
            // range. This non-linearity results in color shifts in the
            // highlight areas of the resulting image unless the raw
            // converter compensates for this effect.
            // LinearResponseLimit specifies the fraction of the encoding
            // range above which the response may become significantly
            // non-linear.
        },{
            "CameraSerialNumber",
            TIFFTAG_CAMERASERIALNUMBER,
            TIFF_ASCII,
            // Count:
            // String length including null
            // Value:
            // Null terminated string
            // Default:
            // None
            // Usage:
            // IFD 0
            // Description:
            // CameraSerialNumber contains the serial number of the camera or camera body that captured the image.
        },{
            "LensInfo",
            TIFFTAG_LENSINFO,
            TIFF_RATIONAL,
            // Count:
            // 4
            // Value:
            // Value: 0: Minimum focal length in mm.
            // Value: 1: Maximum focal length in mm.
            // Value: 2: Minimum (maximum aperture) f-stop at minimum focal length.
            // Value: 3: Minimum (maximum aperture) f-stop at maximum focal length.
            // Default:
            // none
            // Usage:
            // IFD 0
            // Description:
            // LensInfo contains information about the lens that captured
            // the image. If the minimum f-stops are unknown, they should
            // be encoded as 0/0.
        },{
            "ChromaBlurRadius",
            TIFFTAG_CHROMABLURRADIUS,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // Chroma blur radius in pixels
            // Default:
            // See below
            // Usage:
            // Raw IFD
            // Description:
            // ChromaBlurRadius provides a hint to the DNG reader about
            // how much chroma blur should be applied to the image. If
            // this tag is omitted, the reader will use its default amount
            // of chroma blurring.
            // Normally this tag is only included for non-CFA images,
            // since the amount of chroma blur required for mosaic images
            // is highly dependent on the de-mosaic algorithm, in which
            // case the DNG reader's default value is likely optimized for
            // its particular de-mosaic algorithm.
        },{
            "AntiAliasStrength",
            TIFFTAG_ANTIALIASSTRENGTH,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // Relative strength of the camera's anti-alias filter
            // Default:
            // 1.0
            // Usage:
            // Raw IFD
            // Description:
            // AntiAliasStrength provides a hint to the DNG reader about
            // how strong the camera's anti-alias filter is. A value of
            // 0.0 means no anti-alias filter (i.e., the camera is prone
            // to aliasing artifacts with some subjects), while a value of
            // 1.0 means a strong anti-alias filter (i.e., the camera
            // almost never has aliasing artifacts).
            // Note that this tag overlaps in functionality with the
            // BaselineSharpness tag. The primary difference is the
            // AntiAliasStrength tag is used as a hint to the de-mosaic
            // algorithm, while the BaselineSharpness tag is used as a
            // hint to a sharpening algorithm applied later in the
            // processing pipeline.
        },{
            "ShadowScale",
            TIFFTAG_SHADOWSCALE,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 1.0
            // Usage:
            // IFD 0
            // Description:
            // This tag is used by Adobe Camera Raw to control the sensitivity of its "Shadows" slider.
        },{
            "DNGPrivateData",
            TIFFTAG_DNGPRIVATEDATA,
            TIFF_BYTE,
            // Count: Length of private data block in bytes Value: See
            // below Default: None Usage: IFD 0 Description:
            // DNGPrivateData provides a way for camera manufacturers to
            // store private data in the DNG file for use by their own raw
            // converters, and to have that data preserved by programs
            // that edit DNG files.
            // The private data must follow these rules:
            // The private data must start with a null-terminated ASCII
            // string identifying the data. The first part of this string
            // must be the manufacturer's name, to avoid conflicts between
            // manufacturers.
            // *The private data must be self-contained. All offsets
            // within the private data must be offsets relative to the
            // start of the private data, and must not point to bytes
            // outside the private data.
            // *The private data must be byte-order independent. If a DNG
            // file is converted from a big-endian file to a
            // little-endian file, the data must remain valid.
        },{
            "MakerNoteSafety",
            TIFFTAG_MAKERNOTESAFETY,
            TIFF_SHORT,
            // Count:
            // 1
            // Value:
            // 0 (unsafe) or 1 (safe)
            // Default:
            // 0
            // Usage:
            // IFD 0
            // Description:
            // MakerNoteSafety lets the DNG reader know whether the EXIF
            // MakerNote tag is safe to preserve along with the rest of
            // the EXIF data. File browsers and other image management
            // software processing an image with a preserved MakerNote
            // should be aware that any thumbnail image embedded in the
            // MakerNote may be stale, and may not reflect the current
            // state of the full size image.
            // A MakerNote is safe to preserve if it follows these rules:
            // *The MakerNote data must be self-contained. All offsets
            // within the MakerNote must be offsets relative to the start
            // of the MakerNote, and must not point to bytes outside the
            // MakerNote.
            // *The MakerNote data must be byte-order independent. Moving
            // *the data to a file with a different byte order must not
            // *invalidate it.
        },{
            "RawDataUniqueID",
            TIFFTAG_RAWDATAUNIQUEID,
            TIFF_BYTE,
            // Count:
            // 16
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // This tag contains a 16-byte unique identifier for the raw
            // image data in the DNG file. DNG readers can use this tag to
            // recognize a particular raw image, even if the file's name
            // or the metadata contained in the file has been changed.  If
            // a DNG writer creates such an identifier, it should do so
            // using an algorithm that will ensure that it is very
            // unlikely two different images will end up having the same
            // identifier.
        },{
            "OriginalRawFileName",
            TIFFTAG_ORIGINALRAWFILENAME,
            TIFF_ASCII // or BYTE,
            // Count:
            // Byte count including null
            // Value:
            // Null terminated UTF-8 encoded Unicode string
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // If the DNG file was converted from a non-DNG raw file, then
            // this tag contains the file name of that original raw file.
        },{
            "OriginalRawFileData",
            TIFFTAG_ORIGINALRAWFILEDATA,
            TIFF_UNDEFINED,
            // Count:
            // Byte count of embedded data
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // If the DNG file was converted from a non-DNG raw file, then
            // this tag contains the compressed contents of that original
            // raw file.  The contents of this tag always use the
            // big-endian byte order.
            // The tag contains a sequence of data blocks. Future versions
            // of the DNG specification may define additional data blocks,
            // so DNG readers should ignore extra bytes when parsing this
            // tag. DNG readers should also detect the case where data
            // blocks are missing from the end of the sequence, and should
            // assume a default value for all the missing blocks.  There
            // are no padding or alignment bytes between data blocks. The
            // sequence of data blocks is:
            // 1.Compressed data fork of original raw file.
            // 2.Compressed Mac OS resource fork of original raw file.
            // 3.Mac OS file type (4 bytes) of original raw file.
            // 4.Mac OS file creator (4 bytes) of original raw file.
            // 5.Compressed data fork of sidecar ".THM" file.
            // 6.Compressed Mac OS resource fork of sidecar ".THM" file.
            // 7.Mac OS file type (4 bytes) of sidecar ".THM" file.
            // 8.Mac OS file creator (4 bytes) of sidecar ".THM" file.
            // If the Mac OS file types or creator codes are unknown, zero is stored.
            // If the Mac OS resource forks do not exist, they should be encoded as zero length forks.
            // Each fork (data or Mac OS resource) is compressed and encoded as:
            // ForkLength = first four bytes. This is the uncompressed
            // length of this fork. If this value is zero, then no more
            // data is stored for this fork.
            // From ForkLength, compute the number of 64K compression blocks used for this data (the last block is usually smaller than 64K):
            // ForkBlocks = Floor ((ForkLength + 65535) / 65536)
            // The next (ForkBlocks + 1) 4-byte values are an index into
            // the compressed data. The first ForkBlock values are offsets
            // from the start of the data for this fork to the start of
            // the compressed data for the corresponding compression
            // block. The last value is an offset from the start of the
            // data for this fork to the end of the data for this fork.
            // Following this index is the ZIP compressed data for each 64K compression block.
        },{
            "ActiveArea",
            TIFFTAG_ACTIVEAREA,
            TIFF_SHORT // or LONG,
            // Count:
            // 4
            // Value:
            // See below
            // Default:
            // 0, 0, ImageLength, ImageWidth
            // Usage:
            // Raw IFD
            // Description:
            // This rectangle defines the active (non-masked) pixels of
            // the sensor. The order of the rectangle coordinates is: top,
            // left, bottom, right.
        },{
            "MaskedAreas",
            TIFFTAG_MASKEDAREAS,
            TIFF_SHORT // or LONG,
            // Count:
            // 4 * number of rectangles
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // Raw IFD
            // Description:
            // This tag contains a list of non-overlapping rectangle
            // coordinates of fully masked pixels, which can be optionally
            // used by DNG readers to measure the black encoding level.
            // The order of each rectangle's coordinates is: top, left,
            // bottom, right.  If the raw image data has already had its
            // black encoding level subtracted, then this tag should not
            // be used, since the masked pixels are no longer useful.
            // Note that DNG writers are still required to include
            // estimate and store the black encoding level using the black
            // level DNG tags. Support for the MaskedAreas tag is not
            // required of DNG readers.
        },{
            "AsShotICCProfile",
            TIFFTAG_ASSHOTICCPROFILE,
            TIFF_UNDEFINED,
            // Count:
            // Length of ICC profile in bytes
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // This tag contains an ICC profile that, in conjunction with
            // the AsShotPreProfileMatrix tag, provides the camera
            // manufacturer with a way to specify a default color
            // rendering from camera color space coordinates (linear
            // reference values) into the ICC profile connection space.
            // The ICC profile connection space is an output referred
            // colorimetric space, whereas the other color calibration
            // tags in DNG specify a conversion into a scene referred
            // colorimetric space. This means that the rendering in this
            // profile should include any desired tone and gamut mapping
            // needed to convert between scene referred values and output
            // referred values.  DNG readers that have their own tone and
            // gamut mapping controls (such as Adobe Camera Raw) will
            // probably ignore this tag pair.
        },{
            "AsShotPreProfileMatrix",
            TIFFTAG_ASSHOTPREPROFILEMATRIX,
            TIFF_SRATIONAL,
            // Count:
            // 3 * ColorPlanes or ColorPlanes * ColorPlanes
            // Value:
            // See below
            // Default:
            // Identity matrix
            // Usage:
            // IFD 0
            // Description:
            // This tag is used in conjunction with the AsShotICCProfile
            // tag. It specifies a matrix that should be applied to the
            // camera color space coordinates before processing the values
            // through the ICC profile specified in the AsShotICCProfile
            // tag.  The matrix is stored in the row scan order.
            // If ColorPlanes is greater than three, then this matrix can
            // (but is not required to) reduce the dimensionality of the
            // color data down to three components, in which case the
            // AsShotICCProfile should have three rather than ColorPlanes
            // input components.
        },{
            "CurrentICCProfile",
            TIFFTAG_CURRENTICCPROFILE,
            TIFF_UNDEFINED,
            // Count:
            // Length of ICC profile in bytes
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // This tag is used in conjunction with the CurrentPreProfileMatrix tag.
            // The CurrentICCProfile and CurrentPreProfileMatrix tags have
            // the same purpose and usage as the AsShotICCProfile and
            // AsShotPreProfileMatrix tag pair, except they are for use by
            // raw file editors rather than camera manufacturers.
        },{
            "CurrentPreProfileMatrix",
            TIFFTAG_CURRENTPREPROFILEMATRIX,
            TIFF_SRATIONAL,
            // Count:
            // 3 * ColorPlanes or ColorPlanes * ColorPlanes
            // Value:
            // See below
            // Default:
            // Identity matrix
            // Usage:
            // IFD 0
            // Description:
            // This tag is used in conjunction with the CurrentICCProfile tag.
            // The CurrentICCProfile and CurrentPreProfileMatrix tags have
            // the same purpose and usage as the AsShotICCProfile and
            // AsShotPreProfileMatrix tag pair, except they are for use by
            // raw file editors rather than camera manufacturers.
        },{
            "ColorimetricReference",
            50879,
            TIFF_SHORT,
            // Count:
            // 1
            // Value:
            // 0 or 1
            // Default:
            // 0
            // Usage:
            // IFD 0
            // Description:
            // The DNG color model documents a transform between camera
            // colors and CIE XYZ values. This tag describes the
            // colorimetric reference for the CIE XYZ values.
            // 0 = The XYZ values are scene-referred.
            // 1 = The XYZ values are output-referred, using the ICC
            // profile perceptual dynamic range.
            // This tag allows output-referred data to be stored in DNG
            // files and still processed correctly by DNG readers.
        },{
            "CameraCalibrationSignature",
            50931,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Empty string
            // Usage:
            // IFD 0
            // Description:
            // A UTF-8 encoded string associated with the
            // CameraCalibration1 and CameraCalibration2 tags. The
            // CameraCalibration1 and CameraCalibration2 tags should only
            // be used in the DNG color transform if the string stored in
            // the CameraCalibrationSignature tag exactly matches the
            // string stored in the ProfileCalibrationSignature tag for
            // the selected camera profile.
        },{
            "ProfileCalibrationSignature",
            50932,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Empty string
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // A UTF-8 encoded string associated with the camera profile
            // tags. The CameraCalibration1 and CameraCalibration2 tags
            // should only be used in the DNG color transfer if the string
            // stored in the CameraCalibrationSignature tag exactly
            // matches the string stored in the
            // ProfileCalibrationSignature tag for the selected camera
            // profile.
        },{
            "ExtraCameraProfiles",
            TIFFTAG_EXTRACAMERAPROFILES,
            TIFF_LONG,
            // Count:
            // Number of extra camera profiles
            // Value:
            // Offsets to Camera Profile IFDs
            // Default:
            // Empty list
            // Usage:
            // IFD 0
            // Description:
            // A list of file offsets to extra Camera Profile IFDs. The
            // format of a camera profile begins with a 16-bit byte order
            // mark (MM or II) followed by a 16-bit "magic" number equal
            // to 0x4352 (CR), a 32-bit IFD offset, and then a standard
            // TIFF format IFD. All offsets are relative to the start of
            // the byte order mark. Note that the primary camera profile
            // tags should be stored in IFD 0, and the ExtraCameraProfiles
            // tag should only be used if there is more than one camera
            // profile stored in the DNG file.
        },{
            "AsShotProfileName",
            50934,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // A UTF-8 encoded string containing the name of the "as shot"
            // camera profile, if any.
        },{
            "NoiseReductionApplied",
            50935,
            TIFF_RATIONAL,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 0/0
            // Usage:
            // Raw IFD
            // Description:
            // This tag indicates how much noise reduction has been
            // applied to the raw data on a scale of 0.0 to 1.0. A 0.0
            // value indicates that no noise reduction has been applied. A
            // 1.0 value indicates that the "ideal" amount of noise
            // reduction has been applied, i.e. that the DNG reader should
            // not apply additional noise reduction by default. A value of
            // 0/0 indicates that this parameter is unknown.
        },{
            "ProfileName",
            50936,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // A UTF-8 encoded string containing the name of the camera
            // profile. This tag is optional if there is only a single
            // camera profile stored in the file but is required for all
            // camera profiles if there is more than one camera profile
            // stored in the file.
        },{
            "ProfileHueSatMapDims",
            50937,
            TIFF_LONG,
            // Count:
            // 3
            // Value:
            // HueDivisions SaturationDivisions ValueDivisions
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag specifies the number of input samples in each
            // dimension of the hue/saturation/value mapping tables. The
            // data for these tables are stored in ProfileHueSatMapData1
            // and ProfileHueSatMapData2 tags. Allowed values include the
            // following:
            // *HueDivisions >= 1
            // *SaturationDivisions >= 2
            // *ValueDivisions >=1
            // The most common case has ValueDivisions equal to 1, so only
            // hue and saturation are used as inputs to the mapping table.
        },{
            "ProfileHueSatMapData1",
            50938,
            TIFF_FLOAT,
            // Count:
            // HueDivisions * SaturationDivisions * ValueDivisions * 3
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag contains the data for the first
            // hue/saturation/value mapping table. Each entry of the table
            // contains three 32-bit IEEE floating-point values. The first
            // entry is hue shift in degrees; the second entry is
            // saturation scale factor; and the third entry is a value
            // scale factor. The table entries are stored in the tag in
            // nested loop order, with the value divisions in the outer
            // loop, the hue divisions in the middle loop, and the
            // saturation divisions in the inner loop. All zero input
            // saturation entries are required to have a value scale
            // factor of 1.0. The hue/saturation/value table application
            // is described in detail in Chapter 6.
        },{
            "ProfileHueSatMapData2",
            50939,
            TIFF_FLOAT,
            // Count:
            // HueDivisions * SaturationDivisions * ValueDivisions * 3
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag contains the data for the second
            // hue/saturation/value mapping table. Each entry of the table
            // contains three 32-bit IEEE floating-point values. The first
            // entry is hue shift in degrees; the second entry is a
            // saturation scale factor; and the third entry is a value
            // scale factor. The table entries are stored in the tag in
            // nested loop order, with the value divisions in the outer
            // loop, the hue divisions in the middle loop, and the
            // saturation divisions in the inner loop. All zero input
            // saturation entries are required to have a value scale
            // factor of 1.0. The hue/saturation/value table application
            // is described in detail in Chapter 6.
        },{
            "ProfileToneCurve",
            50940,
            TIFF_FLOAT,
            // Count:
            // Samples * 2
            // Value:
            // See below
            // Default:
            // None
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag contains a default tone curve that can be applied
            // while processing the image as a starting point for user
            // adjustments. The curve is specified as a list of 32-bit
            // IEEE floating-point value pairs in linear gamma. Each
            // sample has an input value in the range of 0.0 to 1.0, and
            // an output value in the range of 0.0 to 1.0. The first
            // sample is required to be (0.0, 0.0), and the last sample is
            // required to be (1.0, 1.0). Interpolated the curve using a
            // cubic spline.
        },{
            "ProfileEmbedPolicy",
            50941,
            TIFF_LONG,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // 0
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag contains information about the usage rules for the
            // associated camera profile. The valid values and meanings
            // are:
            // *0 = “allow copying”. The camera profile can be used to
            // *process, or be embedded in, any DNG file. It can be copied
            // *from DNG files to other DNG files, or copied from DNG
            // *files and stored on the user’s system for use in
            // *processing or embedding in any DNG file. The camera
            // *profile may not be used to process non-DNG files.
            // *1 = “embed if used”. This value applies the same rules as
            // *“allow copying”, except it does not allow copying the
            // *camera profile from a DNG file for use in processing any
            // *image other than the image in which it is embedded, unless
            // *the profile is already stored on the user’s system.
            // *2 = “embed never”. This value only applies to profiles
            // *stored on a user’s system but not already embedded in DNG
            // *files. These stored profiles can be used to process images
            // *but cannot be embedded in files. If a camera profile is
            // *already embedded in a DNG file, then this value has the
            // *same restrictions as “embed if used”.
            // *3 = “no restrictions”. The camera profile creator has not
            // *placed any restrictions on the use of the camera profile.
        },{
            "ProfileCopyright",
            50942,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Optional
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // A UTF-8 encoded string containing the copyright information
            // for the camera profile. This string always should be
            // preserved along with the other camera profile tags.
        },{
            "ForwardMatrix1",
            50964,
            TIFF_SRATIONAL,
            // Count:
            // 3 * ColorPlanes
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag defines a matrix that maps white balanced camera
            // colors to XYZ D50 colors. Application is described in
            // detail in Chapter 6.
        },{
            "ForwardMatrix2",
            50965,
            TIFF_SRATIONAL,
            // Count:
            // 3 * ColorPlanes
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag defines a matrix that maps white balanced camera
            // colors to XYZ D50 colors. Application is described in
            // detail in Chapter 6.
        },{
            "PreviewApplicationName",
            50966,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Optional
            // Usage:
            // Preview IFD
            // Description:
            // A UTF-8 encoded string containing the name of the
            // application that created the preview stored in the IFD.
        },{
            "PreviewApplicationVersion",
            50967,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Optional
            // Usage:
            // Preview IFD
            // Description:
            // A UTF-8 encoded string containing the version number of the application that created the preview stored in the IFD.
        },{
            "PreviewSettingsName",
            50968,
            TIFF_ASCII // or BYTE,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Optional
            // Usage:
            // Preview IFD
            // Description:
            // A UTF-8 encoded string containing the name of the
            // conversion settings (for example, snapshot name) used for
            // the preview stored in the IFD.
        },{
            "PreviewSettingsDigest",
            50969,
            TIFF_BYTE,
            // Count:
            // 16
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // Preview IFD
            // Description:
            // A unique ID of the conversion settings (for example, MD5 digest) used to render the preview stored in the IFD.
        },{
            "PreviewColorSpace",
            50970,
            TIFF_LONG,
            // Count:
            // 1
            // Value:
            // See below
            // Default:
            // See below
            // Usage:
            // Preview IFD
            // Description:
            // This tag specifies the color space in which the rendered preview in this IFD is stored. The valid values include:
            // *0 = Unknown
            // *1 = Gray Gamma 2.2
            // *2 = sRGB
            // *3 = Adobe RGB
            // *4 = ProPhoto RGB
            // The default value for this tag is sRGB for color previews and Gray Gamma 2.2 for monochrome previews.
        },{
            "PreviewDateTime",
            50971,
            TIFF_ASCII,
            // Count:
            // Length of string including null
            // Value:
            // Null terminated string
            // Default:
            // Optional
            // Usage:
            // Preview IFD
            // Description:
            // This tag is an ASCII string containing the name of the
            // date/time at which the preview stored in the IFD was
            // rendered. The date/time is encoded using ISO 8601 format.
        },{
            "RawImageDigest",
            50972,
            TIFF_BYTE,
            // Count:
            // 16
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // This tag is an MD5 digest of the raw image data. All pixels
            // in the image are processed in row-scan order. Each pixel is
            // zero padded to 16 or 32 bits deep (16-bit for data less
            // than or equal to 16 bits deep, 32-bit otherwise). The data
            // for each pixel is processed in little-endian byte order.
        },{
            "OriginalRawFileDigest",
            50973,
            TIFF_BYTE,
            // Count:
            // 16
            // Value:
            // See below
            // Default:
            // Optional
            // Usage:
            // IFD 0
            // Description:
            // This tag is an MD5 digest of the data stored in the OriginalRawFileData tag.
        },{
            "SubTileBlockSize",
            50974,
            TIFF_SHORT // or LONG,
            // Count:
            // 2
            // Value:
            // SubTileBlockRows SubTileBlockCols
            // Default:
            // 1, 1
            // Usage:
            // Raw IFD
            // Description:
            // Normally, the pixels within a tile are stored in simple
            // row-scan order. This tag specifies that the pixels within a
            // tile should be grouped first into rectangular blocks of the
            // specified size. These blocks are stored in row-scan
            // order. Within each block, the pixels are stored in row-scan
            // order. The use of a non-default value for this tag requires
            // setting the DNGBackwardVersion tag to at least 1.2.0.0.
        },{
            "RowInterleaveFactor",
            50975,
            TIFF_SHORT // or LONG,
            // Count:
            // 1
            // Value:
            // RowInterleaveFactor
            // Default:
            // 1
            // Usage:
            // Raw IFD
            // Description:
            // This tag specifies that rows of the image are stored in
            // interleaved order. The value of the tag specifies the
            // number of interleaved fields. The use of a non-default
            // value for this tag requires setting the DNGBackwardVersion
            // tag to at least 1.2.0.0.
        },{
            "ProfileLookTableDims",
            50981,
            TIFF_LONG,
            // Count:
            // 3
            // Value:
            // HueDivisions SaturationDivisions ValueDivisions
            // Default:
            // none
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag specifies the number of input samples in each
            // dimension of a default "look" table. The data for this
            // table is stored in the ProfileLookTableData tag. Allowed
            // values include:
            // HueDivisions >= 1
            // SaturationDivisions >= 2
            // Value:Divisions >= 1
        },{
            "ProfileLookTableData",
            50982,
            TIFF_FLOAT,
            // Count:
            // HueDivisions * SaturationDivisions * ValueDivisions * 3
            // Value:
            // See below
            // Default:
            // none
            // Usage:
            // IFD 0 or Camera Profile IFD
            // Description:
            // This tag contains a default "look" table that can be
            // applied while processing the image as a starting point for
            // user adjustment. This table uses the same format as the
            // tables stored in the ProfileHueSatMapData1 and
            // ProfileHueSatMapData2 tags, and is applied in the same
            // color space. However, it should be applied later in the
            // processing pipe, after any exposure compensation and/or
            // fill light stages, but before any tone curve stage.  Each
            // entry of the table contains three 32-bit IEEE
            // floating-point values. The first entry is hue shift in
            // degrees, the second entry is a saturation scale factor, and
            // the third entry is a value scale factor.  The table entries
            // are stored in the tag in nested loop order, with the value
            // divisions in the outer loop, the hue divisions in the
            // middle loop, and the saturation divisions in the inner
            // loop.  All zero input saturation entries are required to
            // have a value scale factor of 1.0.
        },{
            "OpcodeList1",
            51008,
            TIFF_UNDEFINED,
            // Count:
            // Variable
            // Value:
            // Opcode List
            // Default:
            // Empty List
            // Usage:
            // Raw IFD
            // Description:
            // Specifies the list of opcodes that should be applied to the
            // raw image, as read directly from the file. The format and
            // processing details of an opcode list are described in
            // Chapter 7, "Opcode List Processing".
        },{
            "OpcodeList2",
            51009,
            TIFF_UNDEFINED,
            // Count:
            // Variable
            // Value:
            // Opcode List
            // Default:
            // Empty List
            // Usage:
            // Raw IFD
            // Description:
            // Specifies the list of opcodes that should be applied to the
            // raw image, just after it has been mapped to linear
            // reference values. The format and processing details of an
            // opcode list are described in Chapter 7, "Opcode List
            // Processing".
        },{
            "OpcodeList3",
            51022,
            TIFF_UNDEFINED,
            // Count:
            // Variable
            // Value:
            // Opcode List
            // Default:
            // Empty List
            // Usage:
            // Raw IFD
            // Description:
            // Specifies the list of opcodes that should be applied to the
            // raw image, just after it has been demosaiced. The format
            // and processing details of an opcode list are described in
            // Chapter 7, "Opcode List Processing".
        },{
            "NoiseProfile",
            51041,
            TIFF_DOUBLE,
            // Count:
            // 2 or 2 * ColorPlanes
            // Value:
            // See Below
            // Default:
            // Value:s are estimated from BaselineNoise tag (see below).
            // Usage:
            // Raw IFD
            // Description:
            // NoiseProfile describes the amount of noise in a raw
            // image. Specifically, this tag models the amount of
            // signal-dependent photon (shot) noise and signal-independent
            // sensor readout noise, two common sources of noise in raw
            // images. The model assumes that the noise is white and
            // spatially independent, ignoring fixed pattern effects and
            // other sources of noise (e.g., pixel response
            // non-uniformity, spatially-dependent thermal effects, etc.).
            // This tag is intended to be used to describe the amount of
            // noise present in unprocessed raw image data. When noise
            // reduction has already been applied to the raw data (i.e.,
            // NoiseReductionApplied0), this tag may be used to estimate
            // the white component of the residual noise.  For the
            // purposes of this tag, noise is defined as the standard
            // deviation of a random variable x, where xrepresents a
            // recorded linear signal in the range x01[,]. The
            // two-parameter noise model isNixSixOi+= for parameters
            // (SiOi), where Siis a scale term that models the amount of
            // sensor amplification, and Oiis an offset term that models
            // the amount of sensor readout noise. A more detailed
            // explanation of this model is given below.  The data
            // elements for this tag are the nsets of noise model
            // parameters:S1O1S2O2SnO Note that n must be 1 (i.e.,
            // tag count is 2) or equal to the number of color planes in
            // the image (i.e., tag count is 2ColorPlanes). When n1=, the
            // two specified parameters (S1O1) define the same noise
            // model for all image planes. Whenn is equal to the number of
            // image planes, the parameters (SiOi) define the noise model
            // for the ith image plane, e.g., (S1O1) correspond to the
            // first image plane, (S2O2) correspond to the second image
            // plane, etc. The order of the parameters follows the plane
            // order specified by the CFAPlaneColor tag.  EachSi term must
            // be positive (Si0), and eachOi term must be non-negative
            // (Oi0).  A BaselineNoise tag value of 1.0 at ISO 100
            // corresponds approximately to NoiseProfile parameter values
            // of Si2105–=and Oi4.5107–=(e.g., standard deviation of
            // 0.00201 when x0.18=); these values may be used to estimate
            // absolute noise levels in an image when the NoiseProfile tag
            // is missing. When both tags are present, however, DNG
            // readers should prefer using the NoiseProfile data, since it
            // describes noise levels more precisely than BaselineNoise.
            // A more detailed description of the noise model is given
            // below. This tag models two common sources of noise:
            // 1.Photon (shot) noise p, which has a white Poisson
            // distribution, and 2.Electronic readout noise r, which is
            // present even in the absence of light and is assumed to have
            // an approximately white normal (Gaussian) distribution.
            // Assuming thatp andr are independent random variables, the
            // square of the total noise (i.e., the variance) can be
            // expressed as the sum of the squares of the individual
            // sources of noise:N2p2r2+=1 In this expression, the
            // variables N, p, andr are expressed in B-bit recorded
            // digital values, where common values ofB include 12, 14, and
            // 16 bits. Ifxˆ is the average signal level expressed in
            // photons, then its variance will also be xˆ, since a random
            // variable with a Poisson distribution has a variance equal
            // to its mean:pˆ2xˆ=2 wherepˆ denotes the photon noise,
            // expressed in photons. The conversion factor between photons
            // (xˆpˆ) and B-bit digital values (xp) is the gain factor
            // g:xgxˆ=3pgpˆ=4 Substituting equations 2, 3, and 4
            // into equation 1 yields:N2gpˆ2r2+=g2xˆr2+=gxr2+=
            // Therefore the total noiseN can be expressed as a
            // two-parameter function of the signal x:Nxgxr2+=SisOi+=
            // for model parameters SigOir2==.  This tag uses the
            // convention of a normalized noise model, i.e.,Nx() is the
            // standard deviation (i.e., noise) of a random variable x,
            // where x represents a recorded linear signal in the range
            // x01[,]. The specified parameters (SiOi) must also be
            // appropriately normalized.
        },
        //// EXIF tags
        {
            "ExifVersion",
            // The version of this standard supported. Nonexistence of this field is taken to mean nonconformance to the standard
            // (see section 4.2). Conformance to this standard is indicated by recording "0220" as 4-byte ASCII. Since the type is
            // UNDEFINED, there is no NULL for termination.
            EXIFTAG_EXIFVERSION,
            TIFF_UNDEFINED
            // Count = 4
            // Default = "0220"
        },{
            "FlashpixVersion",
            // The Flashpix format version supported by a FPXR file. If the FPXR function supports Flashpix format Ver. 1.0, this
            // is indicated similarly to ExifVersion by recording "0100" as 4-byte ASCII. Since the type is UNDEFINED, there is no
            // NULL for termination.
            EXIFTAG_FLASHPIXVERSION,
            TIFF_UNDEFINED
            // Count = 4
            // Default = "0100"
            // 0100 = Flashpix Format Version 1.0
            // Other = reserved
        },{
            "ColorSpace",
            // The color space information tag (ColorSpace) is always recorded as the color space specifier.
            // Normally sRGB (=1) is used to define the color space based on the PC monitor conditions and environment. If a
            // color space other than sRGB is used, Uncalibrated (=FFFF.H) is set. Image data recorded as Uncalibrated can be
            // treated as sRGB when it is converted to Flashpix. On sRGB see Annex E.
            EXIFTAG_COLORSPACE,
            TIFF_SHORT
            // Count = 1
            // 1 = sRGB
            // FFFF.H = Uncalibrated
            // Other = reserved
        },{
            "PixelXDimension",
            // Information specific to compressed data. When a compressed file is recorded, the valid width of the meaningful
            // image shall be recorded in this tag, whether or not there is padding data or a restart marker. This tag should not
            // exist in an uncompressed file. For details see section 2.8.1 and Annex F.
            EXIFTAG_PIXELXDIMENSION,
            TIFF_LONG // or SHORT
            // Count = 1
            // Default = none
        },{
            "PixelYDimension",
            // Information specific to compressed data. When a compressed file is recorded, the valid height of the meaningful
            // image shall be recorded in this tag, whether or not there is padding data or a restart marker. This tag should not
            // exist in an uncompressed file. For details see section 2.8.1 and Annex F. Since data padding is unnecessary in the
            // vertical direction, the number of lines recorded in this valid image height tag will in fact be the same as that
            // recorded in the SOF.
            EXIFTAG_PIXELYDIMENSION,
            TIFF_LONG // or SHORT
            // Count = 1
        },{
            "ComponentsConfiguration",
            // Information specific to compressed data. The channels of each component are arranged in order from the 1st
            // component to the 4th. For uncompressed data the data arrangement is given in the PhotometricInterpretation tag.
            // However, since PhotometricInterpretation can only express the order of Y,Cb and Cr, this tag is provided for cases
            // when compressed data uses components other than Y, Cb, and Cr and to enable support of other sequences.
            EXIFTAG_COMPONENTSCONFIGURATION,
            TIFF_UNDEFINED
            // Count = 4
            // Default = 4 5 6 0 (if RGB uncompressed)
            // 1 2 3 0 (other cases)
            // 0 = does not exist
            // 1 = Y
            // 2 = Cb
            // 3 = Cr
            // 4 = R
            // 5 = G
            // 6 = B
            // Other = reserved
        },{
            "CompressedBitsPerPixel",
            // Information specific to compressed data. The compression mode used for a compressed image is indicated in unit
            // bits per pixel.
            EXIFTAG_COMPRESSEDBITSPERPIXEL,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "MakerNote",
            // A tag for manufacturers of Exif writers to record any desired information. The contents are up to the manufacturer,
            // but this tag should not be used for any other than its intended purpose.
            EXIFTAG_MAKERNOTE,
            TIFF_UNDEFINED
            // Count = Any
            // Default = none
        },{
            "UserComment",
            // A tag for Exif users to write keywords or comments on the image besides those in ImageDescription, and without
            // the character code limitations of the ImageDescription tag.
            EXIFTAG_USERCOMMENT,
            TIFF_UNDEFINED
            // Count = Any
            // Default = none
            // The character code used in the UserComment tag is identified based on an ID code in a fixed 8-byte area at the
            // start of the tag data area. The unused portion of the area is padded with NULL ("00.H"). ID codes are assigned by
            // means of registration. The designation method and references for each character code are given in Table 6 . The
            // value of Count N is determined based on the 8 bytes in the character code area and the number of bytes in the
            // user comment part. Since the TYPE is not ASCII, NULL termination is not necessary (see Figure 9).
            // Table 6 Character Codes and their Designation
            // Character Code    Code Designation (8 Bytes)                      References
            // ASCII             41.H, 53.H, 43.H, 49.H, 49.H, 00.H, 00.H, 00.H  ITU-T T.50 IA5
            // JIS               4A.H, 49.H, 53.H, 00.H, 00.H, 00.H, 00.H, 00.H  JIS X208-1990
            // Unicode           55.H, 4E.H, 49.H, 43.H, 4F.H, 44.H, 45.H, 00.H  Unicode Standard
            // Undefined         00.H, 00.H, 00.H, 00.H, 00.H, 00.H, 00.H, 00.H  Undefined
        },{
            "DateTimeOriginal",
            // The date and time when the original image data was generated. For a DSC the date and time the picture was taken
            // are recorded. The format is "YYYY:MM:DD HH:MM:SS" with time shown in 24-hour format, and the date and time
            // separated by one blank character [20.H]. When the date and time are unknown, all the character spaces except
            // colons (":") may be filled with blank characters, or else the Interoperability field may be filled with blank characters.
            // The character string length is 20 bytes including NULL for termination. When the field is left blank, it is treated as
            // unknown.
            EXIFTAG_DATETIMEORIGINAL,
            TIFF_ASCII
            // Count = 20
            // Default = none
        },{
            "DateTimeDigitized",
            // The date and time when the image was stored as digital data. If, for example, an image was captured by DSC and at the same
            // time the file was recorded, then the DateTimeOriginal and DateTimeDigitized will have the same contents. The format is
            // "YYYY:MM:DD HH:MM:SS" with time shown in 24-hour format, and the date and time separated by one blank character [20.H].
            // When the date and time are unknown, all the character spaces except colons (":") may be filled with blank characters, or else
            // the Interoperability field may be filled with blank characters. The character string length is 20 bytes including NULL for
            // termination. When the field is left blank, it is treated as unknown.
            EXIFTAG_DATETIMEDIGITIZED,
            TIFF_ASCII
            // Count = 20
            // Default = none
        },{
            "SubsecTime",
            // A tag used to record fractions of seconds for the DateTime tag.
            EXIFTAG_SUBSECTIME,
            TIFF_ASCII
            // Count = Any
            // Default = none
        },{
            "SubsecTimeOriginal",
            // A tag used to record fractions of seconds for the DateTimeOriginal tag.
            EXIFTAG_SUBSECTIMEORIGINAL,
            TIFF_ASCII
            // N = Any
            // Default = none
        },{
            "SubsecTimeDigitized",
            // A tag used to record fractions of seconds for the DateTimeDigitized tag.
            EXIFTAG_SUBSECTIMEDIGITIZED,
            TIFF_ASCII
            // N = Any
            // Default = none
            // Note－Recording subsecond data (SubsecTime, SubsecTimeOriginal, SubsecTimeDigitized)
            // The tag type is ASCII and the string length including NULL is variable length. When the number of valid
            // digits is up to the second decimal place, the subsecond value goes in the Value position. When it is up to
            // four decimal places, an address value is Interoperability, with the subsecond value put in the location
            // pointed to by that address. (Since the count of ASCII type field Interoperability is a value that includes
            // NULL, when the number of valid digits is up to four decimal places the count is 5, and the offset value goes
            // in the Value Offset field. See section 2.6.2.) Note that the subsecond tag differs from the DateTime tag and
            // other such tags already defined in TIFF Rev. 6.0, and that both are recorded in the Exif IFD.
            // Ex.: September 9, 1998, 9:15:30.130 (the number of valid digits is up to the third decimal place)
            // DateTime 1996:09:01 09:15:30 [NULL]
            // SubSecTime 130 [NULL]
            // If the string length is longer than the number of valid digits, the digits are aligned with the start of the area
            // and the rest is filled with blank characters [20.H]. If the subsecond data is unknown, the Interoperability
            // area can be filled with blank characters.
            // Examples when subsecond data is 0.130 seconds:
            // Ex. 1) '1','3','0',[NULL]
            // Ex. 2) '1','3','0',[20.H],[NULL]
            // Ex. 3) '1','3','0', [20.H], [20.H], [20.H], [20.H], [20.H], [NULL]
            // Example when subsecond data is unknown:
            // Ex. 4) [20.H], [20.H], [20.H], [20.H], [20.H], [20.H], [20.H], [20.H], [NULL]
        },{
            "ExposureTime",
            // Exposure time, given in seconds (sec).
            EXIFTAG_EXPOSURETIME,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "FNumber",
            // The F number.
            EXIFTAG_FNUMBER,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },
        {
            "ExifIfd",
            // Exif IFD offset tag
            TIFFTAG_EXIFIFD,
            TIFF_LONG
            // Count = 1
            // Default = None
            // A pointer to the Exif IFD. Exif IFD has the same
            // structure as that of the IFD specified in
            // TIFF. Ordinarily, however, it does not contain image
            // data as in the case of TIFF.
        },
        {
            "GPSIfd",
            // GPS IFD offset tag
            TIFFTAG_GPSIFD,
            TIFF_LONG
            // Count = 1
            // Default = None
            // A pointer to the GPS Info IFD.
        },
        {
            "InteroperabilityIfd",
            // EXIF Interoperability IFD offset tag
            TIFFTAG_INTEROPERABILITYIFD,
            TIFF_LONG
            // Count = 1
            // Default = None
            // A pointer to the EXIF Interoperability IFD.
        },
        {
            "ExposureProgram",
            // The class of the program used by the camera to set exposure when the picture is taken. The tag values are as
            // follows.
            EXIFTAG_EXPOSUREPROGRAM,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = Not defined
            // 1 = Manual
            // 2 = Normal program
            // 3 = Aperture priority
            // 4 = Shutter priority
            // 5 = Creative program (biased toward depth of field)
            // 6 = Action program (biased toward fast shutter speed)
            // 7 = Portrait mode (for closeup photos with the background out of focus)
            // 8 = Landscape mode (for landscape photos with the background in focus)
            // Other = reserved
        },{
            "SpectralSensitivity",
            // Indicates the spectral sensitivity of each channel of the camera used. The tag value is an ASCII string compatible
            // with the standard developed by the ASTM Technical committee.
            EXIFTAG_SPECTRALSENSITIVITY,
            TIFF_ASCII
            // Count = Any
            // Default = none
        },{
            "ISOSpeedRatings",
            // Indicates the ISO Speed and ISO Latitude of the camera or input device as specified in ISO 12232.
            EXIFTAG_PHOTOGRAPHICSENSITIVITY,
            TIFF_SHORT
            // Count = Any
            // Default = none
            // Also called "PhotographicSensitivity" in EXIF 2.3.
            // Can be several different values, depending on value of
            // SensitivityType tag.

        },{
            "OECF",
            // Indicates the Opto-Electric Conversion Function (OECF) specified in ISO 14524. OECF is the relationship between
            // the camera optical input and the image values.
            EXIFTAG_OECF,
            TIFF_UNDEFINED
            // Count = ANY
            // Default = none
            // When this tag records an OECF of m rows and n columns, the values are as in Figure 10.
            // Length Type Meaning
            // 2 SHORT Columns = n
            // 2 SHORT Rows = m
            // Any ASCII 0th column item name (NULL terminated)
            // : : :
            // Any ASCII n-1th column item name (NULL terminated)
            // 8 SRATIONAL OECF value [0,0]
            // : : :
            // 8 SRATIONAL OECF value [n-1,0]
            // 8 SRATIONAL OECF value [0,m-1]
            // : : :
            // 8 SRATIONAL OECF value [n-1,m-1]
            // Figure 10 OECF Description
            // Table 9 gives a simple example.
            // Table 9 Example of Exposure and RGB Output Level
            // Camera log Aperture R Output Level G Output Level B Output Level
            // -3.0 10.2 12.4 8.9
            // -2.0 48.1 47.5 48.3
            // -1.0 150.2 152.0 149.8
        },
        {
            "SensitivityType",
            // Indicates which of the parameters of ISO12232 is the PhotographicSensitivity tag.
            EXIFTAG_SENSITIVITYTYPE,
            TIFF_SHORT
            // Count = 1
            // Default = none
            // 0 : Unknown
            // 1 : Standard output sensitivity (SOS)
            // 2 : Recommended Exposure Index (REI)
            // 3 : ISO speed
            // 4 : SOS and REI
            // 5 : SOS and ISO
            // 6 : REI and ISO
            // 7 : SOS, REI, and ISO
        },
        {
            "ShutterSpeedValue",
            // Shutter speed. The unit is the APEX (Additive System of Photographic Exposure) setting (see Annex C).
            EXIFTAG_SHUTTERSPEEDVALUE,
            TIFF_SRATIONAL
            // Count = 1
            // Default = none
        },{
            "ApertureValue",
            // The lens aperture. The unit is the APEX value.
            EXIFTAG_APERTUREVALUE,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "BrightnessValue",
            // The value of brightness. The unit is the APEX value. Ordinarily it is given in the range of -99.99 to 99.99. Note that
            // if the numerator of the recorded value is FFFFFFFF.H, Unknown shall be indicated.
            EXIFTAG_BRIGHTNESSVALUE,
            TIFF_SRATIONAL
            // Count = 1
            // Default = none
        },{
            "ExposureBiasValue",
            // The exposure bias. The unit is the APEX value. Ordinarily it is given in the range of –99.99 to 99.99.
            EXIFTAG_EXPOSUREBIASVALUE,
            TIFF_SRATIONAL
            // Count = 1
            // Default = none
        },{
            "MaxApertureValue",
            // The smallest F number of the lens. The unit is the APEX value. Ordinarily it is given in the range of 00.00 to 99.99,
            // but it is not limited to this range.
            EXIFTAG_MAXAPERTUREVALUE,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "SubjectDistance",
            // The distance to the subject, given in meters. Note that if the numerator of the recorded value is FFFFFFFF.H,
            // Infinity shall be indicated; and if the numerator is 0, Distance unknown shall be indicated.
            EXIFTAG_SUBJECTDISTANCE,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "MeteringMode",
            // The metering mode.
            EXIFTAG_METERINGMODE,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = unknown
            // 1 = Average
            // 2 = CenterWeightedAverage
            // 3 = Spot
            // 4 = MultiSpot
            // 5 = Pattern
            // 6 = Partial
            // Other = reserved
            // 255 = other
        },{
            "LightSource",
            // The kind of light source.
            EXIFTAG_LIGHTSOURCE,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = unknown
            // 1 = Daylight
            // 2 = Fluorescent
            // 3 = Tungsten (incandescent light)
            // 4 = Flash
            // 9 = Fine weather
            // 10 = Cloudy weather
            // 11 = Shade
            // 12 = Daylight fluorescent (D 5700 – 7100K)
            // 13 = Day white fluorescent (N 4600 – 5400K)
            // 14 = Cool white fluorescent (W 3900 – 4500K)
            // 15 = White fluorescent (WW 3200 – 3700K)
            // 17 = Standard light A
            // 18 = Standard light B
            // 19 = Standard light C
            // 20 = D55
            // 21 = D65
            // 22 = D75
            // 23 = D50
            // 24 = ISO studio tungsten
            // 255 = other light source
            // Other = reserved
        },{
            "Flash",
            // This tag indicates the status of flash when the image was shot. Bit 0 indicates the flash firing status, bits 1 and 2
            // indicate the flash return status, bits 3 and 4 indicate the flash mode, bit 5 indicates whether the flash function is
            // present, and bit 6 indicates "red eye" mode (see Figure 11).
            EXIFTAG_FLASH,
            TIFF_SHORT
            // Count = 1
            // Values for bit 0 indicating whether the flash fired.
            // 0b = Flash did not fire.
            // 1b = Flash fired.
            // Values for bits 1 and 2 indicating the status of returned light.
            // 00b = No strobe return detection function
            // 01b = reserved
            // 10b = Strobe return light not detected.
            // 11b = Strobe return light detected.
            // Values for bits 3 and 4 indicating the camera's flash mode.
            // 00b = unknown
            // 01b = Compulsory flash firing
            // 10b = Compulsory flash suppression
            // 11b = Auto mode
            // Values for bit 5 indicating the presence of a flash function.
            // 0b = Flash function present
            // 1b = No flash function
            // Values for bit 6 indicating the camera's red-eye mode.
            // 0b = No red-eye reduction mode or unknown
            // 1b = Red-eye reduction supported
        },{
            "SubjectArea",
            // This tag indicates the location and area of the main subject in the overall scene.
            EXIFTAG_SUBJECTAREA,
            TIFF_SHORT
            // Count = 2 or 3 or 4
            // Default = none
            // The subject location and area are defined by Count values as follows.
            // Count = 2 Indicates the location of the main subject as coordinates. The first value is the X coordinate and the
            // second is the Y coordinate.
            // Count = 3 The area of the main subject is given as a circle. The circular area is expressed as center coordinates
            // and diameter. The first value is the center X coordinate, the second is the center Y coordinate, and the
            // third is the diameter. (See Figure 12.)
            // Count = 4 The area of the main subject is given as a rectangle. The rectangular area is expressed as center
            // coordinates and area dimensions. The first value is the center X coordinate, the second is the center Y
            // coordinate, the third is the width of the area, and the fourth is the height of the area. (See Figure 13.)
            // Note that the coordinate values, width, and height are expressed in relation to the upper left as origin, prior to
            // rotation processing as per the Rotation tag.
        },{
            "FocalLength",
            // The actual focal length of the lens, in mm. Conversion is not made to the focal length of a 35 mm film camera.
            EXIFTAG_FOCALLENGTH,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "FlashEnergy",
            // Indicates the strobe energy at the time the image is captured, as measured in Beam Candle Power Seconds
            // (BCPS).
            EXIFTAG_FLASHENERGY,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "SpatialFrequencyResponse",
            // This tag records the camera or input device spatial frequency table and SFR values in the direction of image width,
            // image height, and diagonal direction, as specified in ISO 12233.
            EXIFTAG_SPATIALFREQUENCYRESPONSE,
            TIFF_UNDEFINED
            // Count = ANY
            // Default = none
            // When the spatial frequency response for m rows and n columns is recorded, the values are as shown in Figure 14.
            // Length Type Meaning
            // 2 SHORT Columns = n
            // 2 SHORT Rows = m
            // Any ASCII 0th column item name (NULL terminated)
            // : : :
            // Any ASCII n-1th column item name (NULL terminated)
            // 8 RATIONAL SFR value [0,0]
            // : : :
            // 8 RATIONAL SFR value [n-1,0]
            // 8 RATIONAL SFR value [0,m-1]
            // : : :
            // 8 RATIONAL SFR value [n-1,m-1]
            // Figure 14 Spatial Frequency Response Description
            // Table 10 gives a simple example.
            // Table 10 Example of Spatial Frequency Response
            // Spatial Frequency (lw/ph) Along Image Width Along Image Height
            // 0.1 1.00 1.00
            // 0.2 0.90 0.95
            // 0.3 0.80 0.85
        },{
            "FocalPlaneXResolution",
            // Indicates the number of pixels in the image width (X) direction per FocalPlaneResolutionUnit on the camera focal
            // plane.
            EXIFTAG_FOCALPLANEXRESOLUTION,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "FocalPlaneYResolution",
            // Indicates the number of pixels in the image height (Y) direction per FocalPlaneResolutionUnit on the camera focal
            // plane.
            EXIFTAG_FOCALPLANEYRESOLUTION,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "FocalPlaneResolutionUnit",
            // Indicates the unit for measuring FocalPlaneXResolution and FocalPlaneYResolution. This value is the same as the
            // ResolutionUnit.
            EXIFTAG_FOCALPLANERESOLUTIONUNIT,
            TIFF_SHORT
            // Count = 1
            // Default = 2 (inch)
            // Note on use of tags concerning focal plane resolution
            // These tags record the actual focal plane resolutions of the main image which is written as a file after processing
            // instead of the pixel resolution of the image sensor in the camera. It should be noted carefully that the data from
            // the image sensor is resampled.
            // These tags are used at the same time as a FocalLength tag when the angle of field of the recorded image is to
            // be calculated precisely.
        },{
            "SubjectLocation",
            // Indicates the location of the main subject in the scene. The value of this tag represents the pixel at the center of the
            // main subject relative to the left edge, prior to rotation processing as per the Rotation tag. The first value indicates
            // the X column number and second indicates the Y row number.
            EXIFTAG_SUBJECTLOCATION,
            TIFF_SHORT
            // Count = 2
            // Default = none
            // When a camera records the main subject location, it is recommended that the SubjectArea tag be used instead of this tag.
        },{
            "ExposureIndex",
            // Indicates the exposure index selected on the camera or input device at the time the image is captured.
            EXIFTAG_EXPOSUREINDEX,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "SensingMethod",
            // Indicates the image sensor type on the camera or input device. The values are as follows.
            EXIFTAG_SENSINGMETHOD,
            TIFF_SHORT
            // Count = 1
            // Default = none
            // 1 = Not defined
            // 2 = One-chip color area sensor
            // 3 = Two-chip color area sensor
            // 4 = Three-chip color area sensor
            // 5 = Color sequential area sensor
            // 7 = Trilinear sensor
            // 8 = Color sequential linear sensor
            // Other = reserved
        },{
            "FileSource",
            // Indicates the image source. If a DSC recorded the image, this tag value of this tag always be set to 3, indicating
            // that the image was recorded on a DSC.
            EXIFTAG_FILESOURCE,
            TIFF_UNDEFINED
            // Count = 1
            // Default = 3
            // 3 = DSC
            // Other = reserved
        },{
            "SceneType",
            // Indicates the type of scene. If a DSC recorded the image, this tag value shall always be set to 1, indicating that the
            // image was directly photographed.
            EXIFTAG_SCENETYPE,
            TIFF_UNDEFINED
            // Count = 1
            // Default = 1
            // 1 = A directly photographed image
            // Other = reserved
        },{
            "CFAPatternEXIF",
            // Indicates the color filter array (CFA) geometric pattern of the image sensor when a one-chip color area sensor is
            // used. It does not apply to all sensing methods.
            EXIFTAG_CFAPATTERN,
            TIFF_UNDEFINED
            // Count = ANY
            // Default = none
            // Figure 15 shows how a CFA pattern is recorded for a one-chip color area sensor when the color filter array is
            // repeated in m x n (vertical x lateral) pixel units.
            // Length Type Meaning
            // 2 SHORT Horizontal repeat pixel unit = n
            // 2 SHORT Vertical repeat pixel unit = m
            // 1 BYTE CFA value [0.0]
            // : : :
            // 1 BYTE CFA value [n-1.0]
            // 1 BYTE CFA value [0.m-1]
            // : : :
            // 1 BYTE CFA value [n-1.m-1]
            // Figure 15 CFA Pattern Description
            // The relation of color filter color to CFA value is shown in Table 11.
            // Table 11 Color Filter Color and CFA Value
            // Filter Color CFA Value
            // RED 00.H
            // GREEN 01.H
            // BLUE 02.H
            // CYAN 03.H
            // MAGENTA 04.H
            // YELLOW 05.H
            // WHITE 06.H
        },{
            "CustomRendered",
            // This tag indicates the use of special processing on image data, such as rendering geared to output. When special
            // processing is performed, the reader is expected to disable or minimize any further processing.
            EXIFTAG_CUSTOMRENDERED,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = Normal process
            // 1 = Custom process
            // Other = reserved
        },{
            "ExposureMode",
            // This tag indicates the exposure mode set when the image was shot. In auto-bracketing mode, the camera shoots a
            // series of frames of the same scene at different exposure settings.
            EXIFTAG_EXPOSUREMODE,
            TIFF_SHORT
            // Count = 1
            // Default = none
            // 0 = Auto exposure
            // 1 = Manual exposure
            // 2 = Auto bracket
            // Other = reserved
        },{
            "WhiteBalance",
            // This tag indicates the white balance mode set when the image was shot.
            EXIFTAG_WHITEBALANCE,
            TIFF_SHORT
            // Count = 1
            // Default = none
            // 0 = Auto white balance
            // 1 = Manual white balance
            // Other = reserved
        },{
            "DigitalZoomRatio",
            // This tag indicates the digital zoom ratio when the image was shot. If the numerator of the recorded value is 0, this
            // indicates that digital zoom was not used.
            EXIFTAG_DIGITALZOOMRATIO,
            TIFF_RATIONAL
            // Count = 1
            // Default = none
        },{
            "FocalLengthIn35mmFilm",
            // This tag indicates the equivalent focal length assuming a 35mm film camera, in mm. A value of 0 means the focal
            // length is unknown. Note that this tag differs from the FocalLength tag.
            EXIFTAG_FOCALLENGTHIN35MMFILM,
            TIFF_SHORT
            // Count = 1
            // Default = none
        },{
            "This tag indicates the type of scene that was shot. It can also be used to record the mode in which the image was",
            // shot. Note that this differs from the scene type (SceneType) tag.
            EXIFTAG_SCENECAPTURETYPE,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = Standard
            // 1 = Landscape
            // 2 = Portrait
            // 3 = Night scene
            // Other = reserved
        },{
            "GainControl",
            // This tag indicates the degree of overall image gain adjustment.
            EXIFTAG_GAINCONTROL,
            TIFF_SHORT
            // Count = 1
            // Default = none
            // 0 = None
            // 1 = Low gain up
            // 2 = High gain up
            // 3 = Low gain down
            // 4 = High gain down
            // Other = reserved
        },{
            "Contrast",
            // This tag indicates the direction of contrast processing applied by the camera when the image was shot.
            EXIFTAG_CONTRAST,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = Normal
            // 1 = Soft
            // 2 = Hard
            // Other = reserved
        },{
            "Saturation",
            // This tag indicates the direction of saturation processing applied by the camera when the image was shot.
            EXIFTAG_SATURATION,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = Normal
            // 1 = Low saturation
            // 2 = High saturation
            // Other = reserved
        },{
            "Sharpness",
            // This tag indicates the direction of sharpness processing applied by the camera when the image was shot.
            EXIFTAG_SHARPNESS,
            TIFF_SHORT
            // Count = 1
            // Default = 0
            // 0 = Normal
            // 1 = Soft
            // 2 = Hard
            // Other = reserved
        },{
            "DeviceSettingDescription",
            // This tag indicates information on the picture-taking conditions of a particular camera model. The tag is used only to
            // indicate the picture-taking conditions in the reader.
            EXIFTAG_DEVICESETTINGDESCRIPTION,
            TIFF_UNDEFINED
            // Count = Any
            // Default = none
            // The information is recorded in the format shown in Figure 17. The data is recorded in Unicode using SHORT type
            // for the number of display rows and columns and UNDEFINED type for the camera settings. The Unicode (UCS-2)
            // string including Signature is NULL terminated. The specifics of the Unicode string are as given in ISO/IEC 10464-1.
            // Length Type Meaning
            // 2 SHORT Display columns
            // 2 SHORT Display rows
            // Any UNDEFINED Camera setting-1
            // Any UNDEFINED Camera setting-2
            // : : :
            // Any UNDEFINED Camera setting-n
            // Figure 17 Format used to record picture-taking conditions
        },{
            "SubjectDistanceRange",
            // This tag indicates the distance to the subject.
            EXIFTAG_SUBJECTDISTANCERANGE,
            TIFF_SHORT
            // Count = 1
            // Default = none
            // 0 = unknown
            // 1 = Macro
            // 2 = Close view
            // 3 = Distant view
            // Other = reserved
        },{
            "ImageUniqueID",
            // This tag indicates an identifier assigned uniquely to each image. It is recorded as an ASCII string equivalent to
            // hexadecimal notation and 128-bit fixed length.
            EXIFTAG_IMAGEUNIQUEID,
            TIFF_ASCII
            // Count = 33
            // Default = none
        }
    };


    TiffEntryInfo const* tiffEntryLookup(uint16_t tag) {
        static std::map<uint16_t, TiffEntryInfo const*> entriesByTag;
        static std::mutex mutex{};
        static bool initialized{ false };

        // Build map if not yet initialized. Annoying thread-safety mutex
        if (!initialized)
        {
            auto lock = std::scoped_lock{ mutex };

            if (!initialized)
            {
                for (unsigned int i = 0; i < sizeof(tiffEntryTypes) / sizeof(TiffEntryInfo); i++)
                {
                    entriesByTag[tiffEntryTypes[i].tag] = tiffEntryTypes + i;
                }
                initialized = true;
            }
        }

        std::map<uint16_t, TiffEntryInfo const*>::iterator it = entriesByTag.find(tag);
        if (it == entriesByTag.end()) { return NULL; }
        return it->second;
    }

    // The lookup by entry name is done using basic c char arrays, to avoid having to make duplicate strings of the whole big tiffEntryTypes table.

    // Need to compare const char* as strings, not pointers.
    struct TiffNameLess {
        bool operator()(const char* l, const char* r) const {
            return strcmp(l, r) < 0;
        }
    };

    TiffEntryInfo const* tiffEntryLookup(const std::string& entryName) {
        static std::map<const char*, TiffEntryInfo const*, TiffNameLess> entriesByName;
        static std::mutex mutex{};
        static bool initialized{ false };

        // Build map if not yet initialized. Annoying thread-safety mutex
        if (!initialized)
        {
            auto lock = std::scoped_lock{ mutex };

            if (!initialized)
            {
                for (unsigned int i = 0; i < sizeof(tiffEntryTypes) / sizeof(TiffEntryInfo); i++)
                {
                    entriesByName[tiffEntryTypes[i].name] = tiffEntryTypes + i;
                }
                initialized = true;
            }
        }
        std::map<const char*, TiffEntryInfo const*, TiffNameLess>::iterator it = entriesByName.find(entryName.c_str());
        if (it == entriesByName.end()) { return NULL; }
        return it->second;
    }

}

