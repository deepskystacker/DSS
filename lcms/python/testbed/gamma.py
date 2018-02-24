#
# Sample: Gamma tables
#

from lcms import *


#
# Allocate a gamma table
#

Gamma22 = GAMMATABLE(2.2)

print Gamma22

#
# Specify num. of sampled points
#

Gamma22 = GAMMATABLE(2.2, 1024)

print Gamma22

#
# Reverse a gamma table
#

Gamma28 = GAMMATABLE(2.8, 1024)
Rev28   = cmsReverseGamma(1024, Gamma28)
print Rev28

#
# Joint a Gamma 2.8 with inverse of 2.4
#

Joined = cmsJoinGamma(Gamma28, Gamma22)
print Joined

#
# Same, specifying num. of points
#

Joined = cmsJoinGammaEx(Gamma28, Gamma22, 2048)
print Joined

#
#  "Smooth" a curve. Second parameter is smothness lambda
#
cmsSmoothGamma(Joined, 0.8)
print Joined


#
# This should result on a line
#
Straight = cmsJoinGammaEx(Joined, Joined, 2048)
print Straight



