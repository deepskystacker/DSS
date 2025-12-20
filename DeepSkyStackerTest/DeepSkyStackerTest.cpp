#include "pch.h"
#if defined(Q_OS_WIN) && !defined(NDEBUG) && __has_include(<vld.h>)
//
// Visual Leak Detector
//
#include <vld.h>
#endif

#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

TEST_CASE("1: All test cases reside in other .cpp files (empty)", "[multi-file:1]") {
}
