
#
# Sample: Get info on profile
#

from lcms import *


hsRGB = cmsOpenProfileFromFile("sRGB Color Space profile.icm", "r")


print "Product name: ", cmsTakeProductName(hsRGB)
print "Product desc: ", cmsTakeProductDesc(hsRGB)
print "Info: ", cmsTakeProductInfo(hsRGB)

Illuminant = cmsCIEXYZ(0, 0, 0)
cmsTakeIluminant(Illuminant, hsRGB)
print "Illuminant = ", Illuminant

MediaWhite = cmsCIEXYZ(0, 0, 0)
cmsTakeMediaWhitePoint(MediaWhite, hsRGB)
print "Media White = ", MediaWhite

BlackPoint = cmsCIEXYZ(0, 0, 0)
cmsTakeMediaBlackPoint(BlackPoint, hsRGB)
print "Black point = ", BlackPoint


cmsCloseProfile(hsRGB)



