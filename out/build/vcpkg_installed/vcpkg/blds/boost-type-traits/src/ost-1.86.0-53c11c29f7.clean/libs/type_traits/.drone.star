# Use, modification, and distribution are
# subject to the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt)
#
# Copyright Rene Rivera 2020.

# For Drone CI we use the Starlark scripting language to reduce duplication.
# As the yaml syntax for Drone CI is rather limited.
#
#
globalenv={}
linuxglobalimage="cppalliance/droneubuntu1604:1"
windowsglobalimage="cppalliance/dronevs2019"

def main(ctx):
  return [
  linux_cxx("TOOLSET=gcc COMPILER=g++-6 CXXSTD=03,11,14,1z", "g++-6", packages="g++-6", buildtype="boost", image=linuxglobalimage, environment={'TOOLSET': 'gcc', 'COMPILER': 'g++-6', 'CXXSTD': '03,11,14,1z'}, globalenv=globalenv),
  linux_cxx("TOOLSET=gcc COMPILER=g++-6 CXXSTD=03,11,14,1z gnu++", "g++-6", packages="g++-6", buildtype="boost", image=linuxglobalimage, environment={'TOOLSET': 'gcc', 'COMPILER': 'g++-6', 'CXXSTD': '03,11,14,1z', 'CXXSTD_DIALECT': 'cxxstd-dialect=gnu'}, globalenv=globalenv),
  linux_cxx("TOOLSET=gcc COMPILER=g++-7 CXXSTD=03,11,14,17", "g++-7", packages="g++-7", buildtype="boost", image=linuxglobalimage, environment={'TOOLSET': 'gcc', 'COMPILER': 'g++-7', 'CXXSTD': '03,11,14,17'}, globalenv=globalenv),
  linux_cxx("TOOLSET=gcc COMPILER=g++-8 CXXSTD=03,11,14,17", "g++-8", packages="g++-8", buildtype="boost", image=linuxglobalimage, environment={'TOOLSET': 'gcc', 'COMPILER': 'g++-8', 'CXXSTD': '03,11,14,17'}, globalenv=globalenv),
  linux_cxx("TOOLSET=gcc COMPILER=g++-9 CXXSTD=03,11,14,17", "g++-9", packages="g++-9", buildtype="boost", image="cppalliance/droneubuntu1404:1", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++-9', 'CXXSTD': '03,11,14,17,2a'}, globalenv=globalenv),
  linux_cxx("TOOLSET=gcc COMPILER=g++-10 CXXSTD=03,11,14,17,20", "g++-10", packages="g++-10", image="cppalliance/droneubuntu2004:1", buildtype="boost", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++-10', 'CXXSTD': '03,11,14,17,20'}, globalenv=globalenv),
  linux_cxx("TOOLSET=clang COMPILER=clang++-9 CXXSTD=03,11,14,17,2a", "clang++-9", packages="clang-9", llvm_ver="9", buildtype="boost", image="cppalliance/droneubuntu2004:1", environment={'TOOLSET': 'clang', 'COMPILER': 'clang++-9', 'CXXSTD': '03,11,14,17,2a'}, globalenv=globalenv),
  linux_cxx("TOOLSET=clang COMPILER=clang++-10 CXXSTD=03,11,14,17,20", "clang++-10", packages="clang-10", llvm_ver="10", buildtype="boost", image="cppalliance/droneubuntu2004:1", environment={'TOOLSET': 'clang', 'COMPILER': 'clang++-10', 'CXXSTD': '03,11,14,17,20'}, globalenv=globalenv),
  linux_cxx("Ubuntu g++-13", packages="g++-13", buildtype="boost", image="cppalliance/droneubuntu2304:1", environment={'TOOLSET': 'gcc', 'COMPILER': 'g++-13', 'CXXSTD': '14,17,20,23', }, globalenv=globalenv),
  osx_cxx("XCode-11.7 03,11,17,2a", "clang++", packages="", buildtype="boost", xcode_version="11.7", environment={'TOOLSET': 'clang', 'COMPILER': 'clang++', 'CXXSTD': '03,11,14,17,2a', }, globalenv=globalenv),
  osx_cxx("XCode-10.2 03,11,17,2a", "clang++", packages="", buildtype="boost", xcode_version="10.2", environment={'TOOLSET': 'clang', 'COMPILER': 'clang++', 'CXXSTD': '03,11,14,17,2a', }, globalenv=globalenv),
  ]

# from https://github.com/boostorg/boost-ci
load("@boost_ci//ci/drone/:functions.star", "linux_cxx","windows_cxx","osx_cxx","freebsd_cxx")
