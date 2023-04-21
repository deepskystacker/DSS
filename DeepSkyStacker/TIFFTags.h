#pragma once
/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
**      Based on FCam TIFFTags.h
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
****************************************************************************/#include <string>
#include <vector>
#include <stdint.h>
#include <tiff.h>
namespace TIFF
{
    // TIFF basic structures

    struct TiffEntryInfo {
        const char* name;
        uint16_t tag;
        TIFFDataType type;
    };

    struct TiffRational {
        uint32_t numerator;
        uint32_t denominator;
    };

    struct RawTiffIfdEntry {
        uint16_t tag;
        uint16_t type;
        uint32_t count;
        uint32_t offset;
    };

    // TIFF header constants
    constexpr uint32_t headerBytes = 8;
    constexpr uint16_t littleEndianMarker = TIFF_LITTLEENDIAN;
    constexpr uint16_t bigEndianMarker = TIFF_BIGENDIAN;
    constexpr uint16_t tiffMagicNumber = TIFF_VERSION_CLASSIC;

    // TIFF IFD entry information table and fast lookup functions
    extern const TiffEntryInfo tiffEntryTypes[];
    const TiffEntryInfo* tiffEntryLookup(uint16_t tag);
    const TiffEntryInfo* tiffEntryLookup(const std::string& entryName);

    // Hard-coded tag values for various TIFF tags, for faster parsing/filtering
    constexpr uint16_t        TIFF_TAG_NewSubFileType = TIFFTAG_SUBFILETYPE;
    constexpr uint16_t        TIFF_TAG_ImageWidth = TIFFTAG_IMAGEWIDTH;
    constexpr uint16_t        TIFF_TAG_ImageLength = TIFFTAG_IMAGELENGTH;
    constexpr uint16_t        TIFF_TAG_BitsPerSample = TIFFTAG_BITSPERSAMPLE;
    constexpr uint16_t        TIFF_TAG_Compression = TIFFTAG_COMPRESSION;
    constexpr uint16_t        TIFF_TAG_PhotometricInterpretation = TIFFTAG_PHOTOMETRIC;
    constexpr uint16_t        TIFF_TAG_Make = TIFFTAG_MAKE;
    constexpr uint16_t        TIFF_TAG_Model = TIFFTAG_MODEL;
    constexpr uint16_t        TIFF_TAG_StripOffsets = TIFFTAG_STRIPOFFSETS;
    constexpr uint16_t        TIFF_TAG_Orientation = TIFFTAG_ORIENTATION;
    constexpr uint16_t        TIFF_TAG_SamplesPerPixel = TIFFTAG_SAMPLESPERPIXEL;
    constexpr uint16_t        TIFF_TAG_RowsPerStrip = TIFFTAG_ROWSPERSTRIP;
    constexpr uint16_t        TIFF_TAG_StripByteCounts = TIFFTAG_STRIPBYTECOUNTS;
    constexpr uint16_t        TIFF_TAG_PlanarConfiguration = TIFFTAG_PLANARCONFIG;
    constexpr uint16_t        TIFF_TAG_ResolutionUnit = TIFFTAG_RESOLUTIONUNIT;
    constexpr uint16_t        TIFF_TAG_Software = TIFFTAG_SOFTWARE;
    constexpr uint16_t        TIFF_TAG_DateTime = TIFFTAG_DATETIME;

    constexpr uint16_t        TIFFEP_TAG_CFARepeatPatternDim = TIFFTAG_CFAREPEATPATTERNDIM;
    constexpr uint16_t        TIFFEP_TAG_CFAPattern = TIFFTAG_CFAPATTERN;
    constexpr uint16_t        TIFFEP_TAG_Flash = EXIFTAG_FLASH;
    constexpr uint16_t        TIFFEP_TAG_TIFFEPStandardID = 37398;

    // tags from first TIFF supplement
    constexpr uint16_t        TIFF_TAG_SubIFDs = TIFFTAG_SUBIFD;

    // DNG tags
    constexpr uint16_t        DNG_TAG_DNGVersion = TIFFTAG_DNGVERSION;
    constexpr uint16_t        DNG_TAG_DNGBackwardVersion = TIFFTAG_DNGBACKWARDVERSION;
    constexpr uint16_t        DNG_TAG_UniqueCameraModel = TIFFTAG_UNIQUECAMERAMODEL;
    constexpr uint16_t        DNG_TAG_LocalizedCameraModel = TIFFTAG_LOCALIZEDCAMERAMODEL;
    constexpr uint16_t        DNG_TAG_CFAPlaneColor = TIFFTAG_CFAPLANECOLOR;
    constexpr uint16_t        DNG_TAG_CFALayout = TIFFTAG_CFALAYOUT;
    constexpr uint16_t        DNG_TAG_BlackLevelRepeatDim = TIFFTAG_BLACKLEVELREPEATDIM;
    constexpr uint16_t        DNG_TAG_BlackLevel = TIFFTAG_BLACKLEVEL;
    constexpr uint16_t        DNG_TAG_BlackLevelDeltaH = TIFFTAG_BLACKLEVELDELTAH;
    constexpr uint16_t        DNG_TAG_BlackLevelDeltaV = TIFFTAG_BLACKLEVELDELTAV;
    constexpr uint16_t        DNG_TAG_WhiteLevel = TIFFTAG_WHITELEVEL;
    constexpr uint16_t        DNG_TAG_DefaultScale = TIFFTAG_DEFAULTSCALE;
    constexpr uint16_t        DNG_TAG_DefaultCropOrigin = TIFFTAG_DEFAULTCROPORIGIN;
    constexpr uint16_t        DNG_TAG_DefaultCropSize = TIFFTAG_DEFAULTCROPSIZE;
    constexpr uint16_t        DNG_TAG_ColorMatrix1 = TIFFTAG_COLORMATRIX1;
    constexpr uint16_t        DNG_TAG_ColorMatrix2 = TIFFTAG_COLORMATRIX2;
    constexpr uint16_t        DNG_TAG_CameraCalibration1 = TIFFTAG_CAMERACALIBRATION1;
    constexpr uint16_t        DNG_TAG_CameraCalibration2 = TIFFTAG_CAMERACALIBRATION2;
    constexpr uint16_t        DNG_TAG_ReductionMatrix1 = TIFFTAG_REDUCTIONMATRIX1;
    constexpr uint16_t        DNG_TAG_ReductionMatrix2 = TIFFTAG_REDUCTIONMATRIX2;
    constexpr uint16_t        DNG_TAG_AnalogBalance = TIFFTAG_ANALOGBALANCE;
    constexpr uint16_t        DNG_TAG_AsShotNeutral = TIFFTAG_ASSHOTNEUTRAL;
    constexpr uint16_t        DNG_TAG_AsShotWhiteXY = TIFFTAG_ASSHOTWHITEXY;
    constexpr uint16_t        DNG_TAG_BaselineExposure = TIFFTAG_BASELINEEXPOSURE;
    constexpr uint16_t        DNG_TAG_BaselineNoise = TIFFTAG_BASELINENOISE;
    constexpr uint16_t        DNG_TAG_BaselineSharpness = TIFFTAG_BASELINESHARPNESS;
    constexpr uint16_t        DNG_TAG_BayerGreenSplit = TIFFTAG_BAYERGREENSPLIT;
    constexpr uint16_t        DNG_TAG_LinearResponseLimit = TIFFTAG_LINEARRESPONSELIMIT;
    constexpr uint16_t        DNG_TAG_LensInfo = TIFFTAG_LENSINFO;
    constexpr uint16_t        DNG_TAG_ChromaBlurRadius = TIFFTAG_CHROMABLURRADIUS;
    constexpr uint16_t        DNG_TAG_AntiAliasStrength = TIFFTAG_ANTIALIASSTRENGTH;
    constexpr uint16_t        DNG_TAG_ShadowScale = TIFFTAG_SHADOWSCALE;
    constexpr uint16_t        DNG_TAG_DNGPrivateData = TIFFTAG_DNGPRIVATEDATA;
    constexpr uint16_t        DNG_TAG_MakerNoteSafety = TIFFTAG_MAKERNOTESAFETY;
    constexpr uint16_t        DNG_TAG_CalibrationIlluminant1 = TIFFTAG_CALIBRATIONILLUMINANT1;
    constexpr uint16_t        DNG_TAG_CalibrationIlluminant2 = TIFFTAG_CALIBRATIONILLUMINANT2;
    constexpr uint16_t        DNG_TAG_BestQualityScale = TIFFTAG_BESTQUALITYSCALE;
    constexpr uint16_t        DNG_TAG_ActiveArea = TIFFTAG_ACTIVEAREA;
    constexpr uint16_t        DNG_TAG_MaskedAreas = TIFFTAG_MASKEDAREAS;

    // tags from version 1.3.0.0
    constexpr uint16_t        DNG_TAG_NoiseProfile = 51041;

    // EXIF tags
    constexpr uint16_t        EXIF_TAG_ExifIfd = TIFFTAG_EXIFIFD;
    constexpr uint16_t        EXIF_TAG_ExposureTime = EXIFTAG_EXPOSURETIME;
    constexpr uint16_t        EXIF_TAG_FNumber = EXIFTAG_FNUMBER;
    constexpr uint16_t        EXIF_TAG_FocalLength = EXIFTAG_FOCALLENGTH;
    constexpr uint16_t        EXIF_TAG_SubsecTime = EXIFTAG_SUBSECTIME;
    constexpr uint16_t        EXIF_TAG_ISOSpeedRatings = EXIFTAG_ISOSPEEDRATINGS;
    constexpr uint16_t        EXIF_TAG_SensitivityType = EXIFTAG_SENSITIVITYTYPE;

    // Various values for the above fields

    constexpr uint32_t TIFF_NewSubfileType_FullRAW = 0;
    constexpr uint32_t TIFF_NewSubfileType_MainPreview = FILETYPE_REDUCEDIMAGE;
    constexpr uint32_t TIFF_NewSubfileType_OtherPreview = 0x10001;
    constexpr uint32_t TIFF_NewSubfileType_DEFAULT = 0;

    constexpr uint16_t TIFF_PhotometricInterpretation_WhiteIsZero = PHOTOMETRIC_MINISWHITE;
    constexpr uint16_t TIFF_PhotometricInterpretation_BlackIsZero = PHOTOMETRIC_MINISBLACK;
    constexpr uint16_t TIFF_PhotometricInterpretation_RGB = PHOTOMETRIC_RGB;
    constexpr uint16_t TIFF_PhotometricInterpretation_PaletteRGB = PHOTOMETRIC_PALETTE;
    constexpr uint16_t TIFF_PhotometricInterpretation_TransparencyMask = PHOTOMETRIC_MASK;
    constexpr uint16_t TIFF_PhotometricInterpretation_CMYK = PHOTOMETRIC_SEPARATED;
    constexpr uint16_t TIFF_PhotometricInterpretation_YCbCr = PHOTOMETRIC_YCBCR;
    constexpr uint16_t TIFF_PhotometricInterpretation_CIELAB = PHOTOMETRIC_CIELAB;
    constexpr uint16_t TIFF_PhotometricInterpretation_ICCLAB = PHOTOMETRIC_ICCLAB;
    constexpr uint16_t TIFF_PhotometricInterpretation_ITULAB = PHOTOMETRIC_ITULAB;
    constexpr uint16_t TIFF_PhotometricInterpretation_CFA = PHOTOMETRIC_CFA;
    constexpr uint16_t TIFF_PhotometricInterpretation_LinearRaw = 34892;

    constexpr uint16_t TIFF_Compression_Uncompressed = COMPRESSION_NONE;
    constexpr uint16_t TIFF_Compression_LZW = COMPRESSION_LZW;
    constexpr uint16_t TIFF_Compression_JPEG_old = COMPRESSION_OJPEG;
    constexpr uint16_t TIFF_Compression_JPEG = COMPRESSION_JPEG;
    constexpr uint16_t TIFF_Compression_DEFAULT = TIFF_Compression_Uncompressed;

    // First term is what 0th row represents, the second is what 0th column represents
    constexpr uint16_t TIFF_Orientation_TopLeft = ORIENTATION_TOPLEFT;
    constexpr uint16_t TIFF_Orientation_TopRight = ORIENTATION_TOPRIGHT;
    constexpr uint16_t TIFF_Orientation_BottomRight = ORIENTATION_BOTRIGHT;
    constexpr uint16_t TIFF_Orientation_BottomLeft = ORIENTATION_BOTLEFT;
    constexpr uint16_t TIFF_Orientation_LeftTop = ORIENTATION_LEFTTOP;
    constexpr uint16_t TIFF_Orientation_RightTop = ORIENTATION_RIGHTTOP;
    constexpr uint16_t TIFF_Orientation_RightBottom = ORIENTATION_RIGHTBOT;
    constexpr uint16_t TIFF_Orientation_LeftBottom = ORIENTATION_LEFTBOT;


    constexpr uint16_t TIFF_SamplesPerPixel_DEFAULT = 1;

    constexpr uint32_t TIFF_RowsPerStrip_DEFAULT = 0xFFFFFFFF;

    constexpr char TIFFEP_CFAPattern_RGGB[] = { 00,01,01,02 };
    constexpr char TIFFEP_CFAPattern_BGGR[] = { 02,01,01,00 };
    constexpr char TIFFEP_CFAPattern_GRBG[] = { 01,00,02,01 };
    constexpr char TIFFEP_CFAPattern_GBRG[] = { 01,02,00,01 };

    // Table of calibration illuminant color temperatures
    constexpr unsigned int DNG_CalibrationIlluminant_Values = 26;
    constexpr unsigned int DNG_CalibrationIlluminant_Temp[] = {
        0, // 0 = Unknown
        // Values below from personal preference, less
        // known internet sources.
        6500, // 1 = Daylight
        5000, // 2 = Fluorescent
        3200, // 3 = Tungsten (incandescent light)
        5600, // 4 = Flash
        0,    // 5 = invalid
        0,    // 6 = invalid
        0,    // 7 = invalid
        0,    // 8 = invalid
        6500, // 9 = Fine weather
        6000, // 10 = Cloudy weather
        8000, // 11 = Shade
        // Values below from Wikipedia
        6430, // 12 = Daylight fluorescent (D 5700 - 7100K)
        6350, // 13 = Day white fluorescent (N 4600 - 5400K)
        4230, // 14 = Cool white fluorescent (W 3900 - 4500K)
        3450, // 15 = White fluorescent (WW 3200 - 3700K)
        0,    // 16 = Invalid
        2856, // 17 = Standard light A
        4874, // 18 = Standard light B
        6774, // 19 = Standard light C
        5503, // 20 = D55
        6504, // 21 = D65
        7504, // 22 = D75
        5003, // 23 = D50
        // Value from random internet searching
        3200, // 24 = ISO studio tungsten
        0     // > 25 = Other light source
    };
    constexpr uint16_t DNG_TAG_CalibrationIlluminant_StdA = 17;
    constexpr uint16_t DNG_TAG_CalibrationIlluminant_D65 = 21;

    constexpr uint16_t EXIF_TAG_SensitivityType_Unknown = 0;
    constexpr uint16_t EXIF_TAG_SensitivityType_SOS = 1;
    constexpr uint16_t EXIF_TAG_SensitivityType_REI = 2;
    constexpr uint16_t EXIF_TAG_SensitivityType_ISO = 3;
    constexpr uint16_t EXIF_TAG_SensitivityType_SOS_REI = 4;
    constexpr uint16_t EXIF_TAG_SensitivityType_SOS_ISO = 5;
    constexpr uint16_t EXIF_TAG_SensitivityType_REI_ISO = 6;
    constexpr uint16_t EXIF_TAG_SensitivityType_SOS_REI_ISO = 7;

}

