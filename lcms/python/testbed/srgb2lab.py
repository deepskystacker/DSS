
#
# Sample: Convert from sRGB to Lab (D50), perceptual intent
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

Lab        = cmsCIELab()

#
# Open profiles (does use built-ins)
#

hsRGB = cmsCreate_sRGBProfile()
hLab  = cmsCreateLabProfile(None)

#
# The transform
#

xform = cmsCreateTransform(hsRGB, TYPE_RGB_8, hLab, TYPE_Lab_DBL, INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC)


cmsDoTransform(xform, color, Lab, 1)

#
#  Print results
#

print "sRGB = ", color[0], color[1], color[2]
print Lab


#
# Free all stuff
#

cmsDeleteTransform(xform)
cmsCloseProfile(hLab)
cmsCloseProfile(hsRGB)

