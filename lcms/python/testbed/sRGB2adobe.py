
#
# Sample: Convert from sRGB to AdobeRGB,  perceptual intent
#

from lcms import *


print "Enter sRGB values"

#
# Create placeholders
#

RGB        = COLORB()

RGB[0] = input("R?")
RGB[1] = input("G?")
RGB[2] = input("B?")


#
# Open profiles
#

hsRGB   = cmsCreate_sRGBProfile()
hAdobe  = cmsOpenProfileFromFile("AdobeRGB1998.icc", "r")

#
# The transform
#

xform = cmsCreateTransform(hsRGB, TYPE_RGB_8, hAdobe, TYPE_RGB_8, INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC)

cmsDoTransform(xform, RGB, RGB, 1)

#
#  Print results
#

print "AdobeRGB = ", RGB[0], RGB[1], RGB[2]


#
# Free all stuff
#

cmsDeleteTransform(xform)
cmsCloseProfile(hAdobe)
cmsCloseProfile(hsRGB)

