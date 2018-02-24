
#
# Sample: Constants
#

from lcms import *


print "D50=", cmsD50_XYZ()
print "D50=", cmsD50_xyY()

Lab = cmsCIELab(70, -3, 45)

print
print Lab

LCh = cmsCIELCh(0, 0, 0)
cmsLab2LCh(LCh, Lab)
print LCh

Lab2 = cmsCIELab(70, 3, 45)

print
print Lab, Lab2
print "dE (Lab)  =", cmsDeltaE(Lab, Lab2)
print "dE (CIE94)=", cmsCIE94DeltaE(Lab, Lab2)
print "dE (BFD)  = ", cmsBFDdeltaE(Lab, Lab2)
print "dE (CMC)  = ", cmsCMCdeltaE(Lab, Lab2)


