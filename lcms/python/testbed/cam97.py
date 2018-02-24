
#
# Sample: CIECAM97s appearance model
#

from lcms import *


Wt = cmsCIEXYZ()

Wt.X = 95.05
Wt.Y = 100
Wt.Z = 108.88

vc = cmsViewingConditions(Wt, 20, 20, AVG_SURROUND, 0.997)

cam = cmsCIECAM97sInit(vc)

JCh = cmsJCh()
XYZ = cmsCIEXYZ()

XYZ.X = 19.01
XYZ.Y = 20
XYZ.Z = 21.78

print XYZ

cmsCIECAM97sForward(cam,  XYZ, JCh)

print JCh

cmsCIECAM97sReverse(cam, JCh, XYZ)
print XYZ

cmsCIECAM97sDone(cam)


