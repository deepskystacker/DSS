
#
# Sample: Convert from Lab to AdobeRGB,  perceptual intent
#

from lcms import *


print "Enter Lab values"

#
# Create placeholders
#

Lab        = cmsCIELab(0, 0, 0)
LabEncoded = COLORW()
RGB        = COLORB()

Lab.L = input("L?")
Lab.a = input("a?")
Lab.b = input("b?")

# cmsFloat2LabEncoded(LabEncoded.w, Lab)


#
# Open profiles
#

hLab    = cmsCreateLabProfile(None)
hAdobe  = cmsOpenProfileFromFile("AdobeRGB1998.icc", "r")

#
# The transform
#

xform = cmsCreateTransform(hLab, TYPE_Lab_DBL, hAdobe, TYPE_RGB_8, INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC)

cmsDoTransform(xform, Lab, RGB, 1)

#
#  Print results
#

print Lab
print "AdobeRGB = ", RGB[0], RGB[1], RGB[2]


#
# Free all stuff
#

cmsDeleteTransform(xform)
cmsCloseProfile(hAdobe)
cmsCloseProfile(hLab)

