# Boost CMake support infrastructure

This repository hosts the `tools/cmake` Boost submodule, containing
the CMake support infrastructure for Boost.

Note that the officially supported way to build Boost remains
[with `b2`](https://www.boost.org/more/getting_started/index.html).

## Building Boost with CMake

The first thing you need to know is that the
[official Boost releases](https://www.boost.org/users/download/)
can't be built with CMake. Even though the Boost Github repository
contains a `CMakeLists.txt` file, it's removed from the release.

That's because the file and directory layout of Boost releases,
for historical reasons, has all the Boost header files copied
into a single `boost/` directory. These headers are then removed
from the individual library `include/` directories. The CMake
support infrastructure expects the headers to remain in their
respective `libs/<libname>/include` directories, and therefore
does not work on a release archive.

To build Boost with CMake, you will need either a Git clone
of Boost
(`git clone --recurse-submodules https://github.com/boostorg/boost`)
or the alternative archives
[available on Github](https://github.com/boostorg/boost/releases).

Once you have cloned, or downloaded and extracted, Boost, use the
usual procedure of

```
mkdir __build
cd __build
cmake ..
cmake --build .
```

to build it with CMake. To install it, add

```
cmake --build . --target install
```

Under Windows (when using the Visual Studio generator), you can
control whether Debug or Release variants are built by adding
`--config Debug` or `--config Release` to the `cmake --build` lines:

```
cmake --build . --config Debug
```

```
cmake --build . --target install --config Debug
```

The default is Debug. You can build and
install both Debug and Release at the same time, by running the
respective `cmake --build` line twice, once per `--config`:

```
cmake --build . --target install --config Debug
cmake --build . --target install --config Release
```

## Configuration variables

The following variables are supported and can be set either from
the command line as `cmake -DVARIABLE=VALUE ..`, or via `ccmake`
or `cmake-gui`:

* `BOOST_INCLUDE_LIBRARIES`

  A semicolon-separated list of libraries to include into the build (and
  installation.) Defaults
  to empty, which means "all libraries". Example: `filesystem;regex`.

* `BOOST_EXCLUDE_LIBRARIES`

  A semicolon-separated list of libraries to exclude from the build (and
  installation.) This is useful if a library causes an error in the CMake
  configure phase.

* `BOOST_ENABLE_MPI`

  Set to ON if Boost libraries depending on MPI should be built.

* `BOOST_ENABLE_PYTHON`

  Set to ON if Boost libraries depending on Python should be built.

* [`CMAKE_BUILD_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)

  For single-configuration generators such as Makefile and Ninja (the typical
  case under POSIX operating systems), controls the build variant (Debug or
  Release.) The default when building Boost is set to Release.

  For multi-configuration generators such as the Visual Studio generators,
  `CMAKE_BUILD_TYPE` is ignored; the desired configuration is set at build
  (or install) time, with the `--config` option to `cmake --build` and
  `cmake --install`.

  For more information, see
  [the CMake documentation on build configurations](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations).

* [`CMAKE_INSTALL_PREFIX`](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html)

  A standard CMake variable that determines where the headers and libraries
  should be installed. The default when building Boost is set to `C:/Boost`
  under Windows, `/usr/local` otherwise.

* [`CMAKE_INSTALL_INCLUDEDIR`](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)

  Directory in which to install the header files. Can be relative to
  `CMAKE_INSTALL_PREFIX`. Default `include`.

* [`CMAKE_INSTALL_BINDIR`](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)

  Directory in which to install the binary artifacts (executables and Windows
  DLLs.) Can be relative to `CMAKE_INSTALL_PREFIX`. Default `bin`.

* [`CMAKE_INSTALL_LIBDIR`](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)

  Directory in which to install the compiled libraries. Can be relative to
  `CMAKE_INSTALL_PREFIX`. Default `lib`.

* `BOOST_INSTALL_CMAKEDIR`

  Directory in which to install the CMake configuration files. Default `lib/cmake`.

* `BOOST_INSTALL_LAYOUT`

  Boost installation layout. Can be one of `system`, `tagged`, or `versioned`.
  The default is `versioned` under Windows, and `system` otherwise.

  `versioned` produces library names of the form
  `libboost_timer-vc143-mt-gd-x64-1_82.lib`, containing the toolset (compiler)
  name and version, encoded build settings, and the Boost version. (The
  extension is `.lib` under Windows, `.a` or `.so` under Linux, and `.a` or
  `.dylib` under macOS.)

  `tagged` produces library names of the form `libboost_timer-mt-gd-x64.lib`;
  only the build settings are encoded in the name, the toolset and the Boost
  version are not.

  `system` produces library names of the form `libboost_timer.lib` (or
  `libboost_timer.a`, `libboost_timer.so`, `libboost_timer.dylib`.)

* `BOOST_INSTALL_INCLUDE_SUBDIR`

  When `BOOST_INSTALL_LAYOUT` is `versioned`, headers are installed in a
  subdirectory of `CMAKE_INSTALL_INCLUDEDIR` (to enable several Boost releases
  being installed at the same time.) The default for release e.g. 1.81 is
  `/boost-1_81`.)

* `BOOST_RUNTIME_LINK`

  Whether to use the static or the shared C++ runtime libraries under Microsoft
  Visual C++ and compatible compilers. (The available values are `shared` and
  `static` and the default is `shared`.)

* [`BUILD_TESTING`](https://cmake.org/cmake/help/latest/module/CTest.html)

  A standard CMake variable; when ON, tests are configured and built. Defaults
  to OFF.

* [`BUILD_SHARED_LIBS`](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html)

  A standard CMake variable that determines whether to build shared or static
  libraries. Defaults to OFF.

* `BOOST_STAGEDIR`

  The directory in which to place the build outputs. Defaults to the `stage`
  subdirectory of the current CMake binary directory.

  The standard CMake variables
  [`CMAKE_RUNTIME_OUTPUT_DIRECTORY`](https://cmake.org/cmake/help/latest/variable/CMAKE_RUNTIME_OUTPUT_DIRECTORY.html),
  [`CMAKE_LIBRARY_OUTPUT_DIRECTORY`](https://cmake.org/cmake/help/latest/variable/CMAKE_LIBRARY_OUTPUT_DIRECTORY.html),
  and
  [`CMAKE_ARCHIVE_OUTPUT_DIRECTORY`](https://cmake.org/cmake/help/latest/variable/CMAKE_ARCHIVE_OUTPUT_DIRECTORY.html)
  are set by default to `${BOOST_STAGEDIR}/bin`, `${BOOST_STAGEDIR}/lib`, and
  `${BOOST_STAGEDIR}/lib`, respectively.

* [`CMAKE_CXX_VISIBILITY_PRESET`](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_VISIBILITY_PRESET.html)

  C++ symbol visibility (one of `default`, `hidden`, `protected`, `internal`). The default is set to `hidden` to match `b2`.

* [`CMAKE_C_VISIBILITY_PRESET`](https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_VISIBILITY_PRESET.html)

  C symbol visibility (one of `default`, `hidden`, `protected`, `internal`). The default is set to `hidden` to match `b2`.

* [`CMAKE_VISIBILITY_INLINES_HIDDEN`](https://cmake.org/cmake/help/latest/variable/CMAKE_VISIBILITY_INLINES_HIDDEN.html)

  Whether inline functions should have hidden visibility. The default is set to `ON` to match `b2`.

## Library-specific configuration variables

Some Boost libraries provide their own configuration variables, some of which
are given below.

### Context

* `BOOST_CONTEXT_BINARY_FORMAT`

  Allowed values are `elf`, `mach-o`, `pe`, `xcoff`. The default is
  autodetected from the platform.

* `BOOST_CONTEXT_ABI`

  Allowed values are `aapcs`, `eabi`, `ms`, `n32`, `n64`, `o32`, `o64`, `sysv`,
  `x32`. The default is autodetected from the platform.

* `BOOST_CONTEXT_ARCHITECTURE`

  Allowed values are `arm`, `arm64`, `loongarch64`, `mips32`, `mips64`,
  `ppc32`, `ppc64`, `riscv64`, `s390x`, `i386`, `x86_64`, `combined`.
  The default is autodetected from the platform.

* `BOOST_CONTEXT_ASSEMBLER`

  Allowed values are `masm`, `gas`, `armasm`. The default is autodetected from
  the platform.

* `BOOST_CONTEXT_ASM_SUFFIX`

  Allowed values are `.asm` and `.S`. The default is autodetected from the
  platform.

* `BOOST_CONTEXT_IMPLEMENTATION`

  Allowed values are `fcontext`, `ucontext`, `winfib`. Defaults to `fcontext`.

### Fiber

* `BOOST_FIBER_NUMA_TARGET_OS`

  Target OS for the Fiber NUMA support. Can be `aix`, `freebsd`, `hpux`,
  `linux`, `solaris`, `windows`, `none`. Defaults to `windows` under Windows,
  `linux` under Linux, otherwise `none`.

### IOStreams

* `BOOST_IOSTREAMS_ENABLE_ZLIB`

  When ON, enables ZLib support. Defaults to ON when `zlib` is found, OFF
  otherwise.

* `BOOST_IOSTREAMS_ENABLE_BZIP2`

  When ON, enables BZip2 support. Defaults to ON when `libbzip2` is found,
  OFF otherwise.

* `BOOST_IOSTREAMS_ENABLE_LZMA`

  When ON, enables LZMA support. Defaults to ON when `liblzma` is found,
  OFF otherwise.

* `BOOST_IOSTREAMS_ENABLE_ZSTD`

  When ON, enables Zstd support. Defaults to ON when `libzstd` is found,
  OFF otherwise.

### Locale

* `BOOST_LOCALE_ENABLE_ICU`

  When ON, enables the ICU backend. Defaults to ON when ICU is found,
  OFF otherwise.

* `BOOST_LOCALE_ENABLE_ICONV`

  When ON, enables the Iconv backend. Defaults to ON when `iconv` is found,
  OFF otherwise.

* `BOOST_LOCALE_ENABLE_POSIX`

  When ON, enables the POSIX backend. Defaults to ON on POSIX systems,
  OFF otherwise.

* `BOOST_LOCALE_ENABLE_STD`

  When ON, enables the `std::locale` backend. Defaults to ON.

* `BOOST_LOCALE_ENABLE_WINAPI`

  When ON, enables the Windows API backend. Defaults to ON under Windows, OFF
  otherwise.

### Stacktrace

* `BOOST_STACKTRACE_ENABLE_NOOP`

  When ON, builds the `boost_stacktrace_noop` library variant. Defaults to ON.

* `BOOST_STACKTRACE_ENABLE_BACKTRACE`

  When ON, builds the `boost_stacktrace_backtrace` library variant. Defaults
  to ON when `libbacktrace` is found, OFF otherwise.

* `BOOST_STACKTRACE_ENABLE_ADDR2LINE`

  When ON, builds the `boost_stacktrace_addr2line` library variant. Defaults
  to ON, except on Windows.

* `BOOST_STACKTRACE_ENABLE_BASIC`

  When ON, builds the `boost_stacktrace_basic` library variant. Defaults to ON.

* `BOOST_STACKTRACE_ENABLE_WINDBG`

  When ON, builds the `boost_stacktrace_windbg` library variant. Defaults to
  ON under Windows when WinDbg support is autodetected, otherwise OFF.

* `BOOST_STACKTRACE_ENABLE_WINDBG_CACHED`

  When ON, builds the `boost_stacktrace_windbg_cached` library variant.
  Defaults to ON under Windows when WinDbg support is autodetected and when
  `thread_local` is supported, otherwise OFF.

### Thread

* `BOOST_THREAD_THREADAPI`

  Threading API, `pthread` or `win32`. Defaults to `win32` under Windows,
  `pthread` otherwise.

## Testing Boost with CMake

To run the Boost tests with CMake/CTest, first configure as before, but with
`BUILD_TESTING=ON`:

```
mkdir __build
cd __build
cmake -DBUILD_TESTING=ON ..
```

then build the tests:

```
cmake --build . --target tests
```

and then run them:

```
ctest --output-on-failure --no-tests=error
```

Under Windows, you need to select a configuration (Debug or Release):

```
cmake --build . --target tests --config Debug
ctest --output-on-failure --no-tests=error -C Debug
```

To only build the tests for a specific library, and not the entire Boost,
use `BOOST_INCLUDE_LIBRARIES`:

```
cmake -DBUILD_TESTING=ON -DBOOST_INCLUDE_LIBRARIES=timer ..
```

To build and run in parallel using more than one core, use the `-j`
option:

```
cmake --build . --target tests -j 16
ctest --output-on-failure --no-tests=error -j 16
```

A convenience target `check` is provided that first builds the tests and
then invokes `ctest`:

```
cmake --build . --target check
```

but it doesn't support running the tests in parallel.

## Using Boost after building and installing it with CMake

Normally, a Boost installation is used from CMake by means of
`find_package(Boost)`. However, up to and including release 1.81.0, installing
Boost with CMake did not deploy the necessary CMake configuration file for
the `Boost` package, so `find_package(Boost)` did not work. (It also did
not provide the `Boost::boost` and `Boost::headers` targets, on which many
existing `CMakeLists.txt` files rely.)

Instead, the individual Boost libraries needed to be referenced as in
```cmake
find_package(boost_filesystem 1.81 REQUIRED)
```

This has been rectified in Boost 1.82, which installs an umbrella CMake
configuration file for the Boost package (`BoostConfig.cmake`) and
provides the `Boost::boost` and `Boost::headers` compatibility targets.

## Using Boost with `add_subdirectory`

Assuming that your project already has a copy of Boost in a subdirectory,
either deployed as a Git submodule or extracted manually by the user as a
prerequisite, using it is relatively straightforward:

```cmake
add_subdirectory(deps/boost)
```

However, as-is, this will configure all Boost libraries and build them by
default regardless of whether they are used. It's better to use

```cmake
add_subdirectory(deps/boost EXCLUDE_FROM_ALL)
```

so that only the libraries that are referenced by the project are built,
and it's even better to set `BOOST_INCLUDE_LIBRARIES` before the
`add_subdirectory` call to a list of the Boost libraries that need to be
configured:

```cmake
set(BOOST_INCLUDE_LIBRARIES filesystem regex)
add_subdirectory(deps/boost EXCLUDE_FROM_ALL)
```

## Using an individual Boost library with `add_subdirectory`

Boost is a large dependency, and sometimes a project only needs a single
library. It's possible to use `add_subdirectory` with individual Boost
libraries (`https://github.com/boostorg/<libname>`) instead of the entire
superproject or release archive. However, since Boost libraries depend on
each other quite extensively, all library dependencies also need to be
added (again via `add_subdirectory`.)

As an example, this is how one would use Boost.Timer in this manner:

```cmake
set(libs

  timer

  # Primary dependencies

  chrono
  config
  core
  io
  predef
  system
  throw_exception

  # Secondary dependencies

  assert
  integer
  move
  mpl
  ratio
  static_assert
  type_traits
  typeof
  utility
  winapi
  variant2
  preprocessor
  rational
  mp11
)

foreach(lib IN LISTS libs)

  add_subdirectory(deps/boost/${lib} EXCLUDE_FROM_ALL)

endforeach()
```

assuming that the individual libraries have been placed in subdirectories
of `deps/boost`.

(The list of required dependencies above has been produced by running
`boostdep --brief timer`. See
[the documentation of Boostdep](https://boost.org/tools/boostdep).)

## Using Boost with `FetchContent`

`FetchContent` downloads the required dependencies as part of CMake's
project configuration phase. While this is convenient because it doesn't
require the user to acquire the dependencies beforehand, in the case of
Boost it involves an 87 MB download, so you should carefully weigh the
pros and cons of this approach.

That said, here's how one would use Boost with `FetchContent`:

```cmake
include(FetchContent)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_MakeAvailable(Boost)
```

This has the same drawback as the simple `add_subdirectory` call -- all
Boost libraries are configured and built, even if not used by the project.

To configure only some Boost libraries, set `BOOST_INCLUDE_LIBRARIES`
before the `FetchContent_MakeAvailable` call:

```cmake
set(BOOST_INCLUDE_LIBRARIES timer filesystem regex)
FetchContent_MakeAvailable(Boost)
```

To perform the `add_subdirectory` call with the `EXCLUDE_FROM_ALL` option, if you
are using CMake 3.28 or newer, you can simply pass `EXCLUDE_FROM_ALL` to
`FetchContent_Declare`:

```cmake
FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
  URL_MD5 893b5203b862eb9bbd08553e24ff146a
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  EXCLUDE_FROM_ALL
)
```

For earlier versions of CMake, you can replace `FetchContent_MakeAvailable(Boost)` with this:

```cmake
FetchContent_GetProperties(Boost)

if(NOT Boost_POPULATED)

  message(STATUS "Fetching Boost")
  FetchContent_Populate(Boost)

  message(STATUS "Configuring Boost")
  add_subdirectory(${Boost_SOURCE_DIR} ${Boost_BINARY_DIR} EXCLUDE_FROM_ALL)

endif()
```
