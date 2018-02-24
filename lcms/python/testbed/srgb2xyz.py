
#
# Sample: Convert from sRGB to XYZ  perceptual intent
#

from lcms import *


print "Enter sRGB values, 0..255"

#
# Create placeholder for colorant
#

color = COLORB()
color[0] = input("R?")
color[1] = input("G?")
color[2] = input("B?")


#
# Create placeholder for Lab
#

XYZ = cmsCIEXYZ()

#
# Open profiles (does use built-ins)
#

hsRGB = cmsCreate_sRGBProfile()
hXYZ  = cmsCreateXYZProfile()

#
# The transform
#

xform = cmsCreateTransform(hsRGB, TYPE_RGB_8, hXYZ, TYPE_XYZ_DBL, INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC)


cmsDoTransform(xform, color, XYZ, 1)

#
#  Print results
#

print "sRGB = ", color[0], color[1], color[2]
print XYZ

xyY = cmsCIExyY(0, 0, 0)
cmsXYZ2xyY(xyY, XYZ)

print xyY

#
# Free all stuff
#

cmsDeleteTransform(xform)
cmsCloseProfile(hXYZ)
cmsCloseProfile(hsRGB)

