
#
# Sample: White point from temperature
#

from lcms import *


Temp = input("Temperature §K? ")

WhitexyY = cmsCIExyY(0, 0, 1)

cmsWhitePointFromTemp(Temp, WhitexyY)

WhiteXYZ = cmsCIEXYZ(0, 0, 0)
cmsxyY2XYZ(WhiteXYZ, WhitexyY)

print WhitexyY
print WhiteXYZ

Lab = cmsCIELab(0, 0, 0)

cmsXYZ2Lab(None, Lab, WhiteXYZ)
print "D50", Lab



