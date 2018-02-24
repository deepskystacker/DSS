#
# Sample: Creates a virtual profile emulating sRGB
#

from lcms import *

#hsRGB = cmsOpenProfileFromFile("sRGB Color Space profile.icm", "r")

Rec709Primaries = cmsCIExyYTRIPLE(cmsCIExyY(0.6400, 0.3300, 1.0),
                                  cmsCIExyY(0.3000, 0.6000, 1.0),
                                  cmsCIExyY(0.1500, 0.0600, 1.0))


Gamma22 = GAMMATABLE(2.2, 4096)

print Gamma22

D65 = cmsCIExyY()
cmsWhitePointFromTemp(6504, D65)

hVirtual_sRGB = cmsCreateRGBProfile(D65,
                             Rec709Primaries,
                            (Gamma22, Gamma22, Gamma22));


hsRGB = cmsCreate_sRGBProfile();

xform = cmsCreateTransform(hsRGB, TYPE_RGB_8, hVirtual_sRGB, TYPE_RGB_8,
                           INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC)

color = COLORB()
color[0] = 155
color[1] = 45
color[2] = 200


print color[0], color[1], color[2]

cmsDoTransform(xform, color, color, 1)

print color[0], color[1], color[2]

cmsDeleteTransform(xform)
cmsCloseProfile(hsRGB)
cmsCloseProfile(hVirtual_sRGB)

