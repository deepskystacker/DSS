
#
# Sample: Gamut clamping
#

from lcms import *



Lab = cmsCIELab(80, -200, 50)
print "Original", Lab

#
# Desaturates color to bring it into gamut.
# The gamut boundaries are specified as:
#   -120 <= a <= 120
#   -130 <= b <= 130

cmsClampLab(Lab, 120, -120, 130, -130)

print "Constrained", Lab


