#
# Sample: Creates and saves a matrix-shaper profile
#

from lcms import *

Rec709Primaries = cmsCIExyYTRIPLE(cmsCIExyY(0.6400, 0.3300, 1.0),
                                  cmsCIExyY(0.3000, 0.6000, 1.0),
                                  cmsCIExyY(0.1500, 0.0600, 1.0))


Gamma22 = GAMMATABLE(2.2, 4096)
D65 = cmsCIExyY()
cmsWhitePointFromTemp(6504, D65)

hVirtual_sRGB = cmsCreateRGBProfile(D65,
                             Rec709Primaries,
                            (Gamma22, Gamma22, Gamma22));



#
# This should probably be bettered somehow...
#

icSigProfileDescriptionTag  = icTagSignature(0x64657363)
text = StrPointer("Sample sRGB profile")


cmsAddTag(hVirtual_sRGB, icSigProfileDescriptionTag, text)


cmsSaveProfile(hVirtual_sRGB, "Virtual_sRGB.icm")

cmsCloseProfile(hVirtual_sRGB)


