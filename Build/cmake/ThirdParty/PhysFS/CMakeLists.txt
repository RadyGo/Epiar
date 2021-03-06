# PhysicsFS CMake build file
cmake_minimum_required (VERSION 2.6)
PROJECT(PhysicsFS)

if (NOT PhysFS_SOURCE_NAME)
	set (PhysFS_SOURCE_NAME "physfs-2.0.0" CACHE STRING "PhysFS source name")
endif (NOT PhysFS_SOURCE_NAME)

set(PHYSFS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/${PhysFS_SOURCE_NAME}" CACHE STRING "PhysFS include directory" FORCE)
set(PHYSFS_LIBRARY physfs-static CACHE STRING "PhysFS libraries" FORCE)

# PHYSFS CMAKE
SET(PHYSFS_VERSION 2.0.0)

# Increment this if/when we break backwards compatibility.
SET(PHYSFS_SOVERSION 1)

# I hate that they define "WIN32" ... we're about to move to Win64...I hope!
IF(WIN32 AND NOT WINDOWS)
    SET(WINDOWS TRUE)
ENDIF(WIN32 AND NOT WINDOWS)

# Bleh, let's do it for "APPLE" too.
IF(APPLE AND NOT MACOSX)
    SET(MACOSX TRUE)
ENDIF(APPLE AND NOT MACOSX)

INCLUDE(CheckIncludeFile)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckCSourceCompiles)

INCLUDE_DIRECTORIES(${PhysFS_SOURCE_NAME})
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(platform)
#INCLUDE_DIRECTORIES(archivers)

IF(MACOSX)
    # Fallback to older OS X on PowerPC to support wider range of systems...
    IF(CMAKE_OSX_ARCHITECTURES MATCHES ppc)
        ADD_DEFINITIONS(-DMAC_OS_X_VERSION_MIN_REQUIRED=1020)
        SET(OTHER_LDFLAGS ${OTHER_LDFLAGS} " -mmacosx-version-min=10.2")
    ENDIF(CMAKE_OSX_ARCHITECTURES MATCHES ppc)

    # Need these everywhere...
    ADD_DEFINITIONS(-fno-common)
    SET(OTHER_LDFLAGS ${OTHER_LDFLAGS} " -framework Carbon -framework IOKit")
ENDIF(MACOSX)

# Add some gcc-specific command lines.
IF(CMAKE_COMPILER_IS_GNUCC)
    # Always build with debug symbols...you can strip it later.
    ADD_DEFINITIONS(-g -pipe -Werror -fsigned-char)

    # Stupid BeOS generates warnings in the system headers.
    IF(NOT BEOS)
        ADD_DEFINITIONS(-Wall)
    ENDIF(NOT BEOS)

    CHECK_C_SOURCE_COMPILES("
        #if ((defined(__GNUC__)) && (__GNUC__ >= 4))
        int main(int argc, char **argv) { int is_gcc4 = 1; return 0; }
        #else
        #error This is not gcc4.
        #endif
    " PHYSFS_IS_GCC4)

    IF(PHYSFS_IS_GCC4)
        ADD_DEFINITIONS(-fvisibility=hidden)
    ENDIF(PHYSFS_IS_GCC4)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(MSVC)
    # VS.NET 8.0 got really really anal about strcpy, etc, which even if we
    #  cleaned up our code, zlib, etc still use...so disable the warning.
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS=1)
ENDIF(MSVC)

# Basic chunks of source code ...

SET(LZMA_SRCS
    ${PhysFS_SOURCE_NAME}/lzma/C/7zCrc.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Archive/7z/7zBuffer.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Archive/7z/7zDecode.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Archive/7z/7zExtract.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Archive/7z/7zHeader.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Archive/7z/7zIn.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Archive/7z/7zItem.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Archive/7z/7zMethodID.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Compress/Branch/BranchX86.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Compress/Branch/BranchX86_2.c
    ${PhysFS_SOURCE_NAME}/lzma/C/Compress/Lzma/LzmaDecode.c
)

IF(BEOS)
    # We add this explicitly, since we don't want CMake to think this
    #  is a C++ project unless we're on BeOS.
    SET(PHYSFS_BEOS_SRCS ${PhysFS_SOURCE_NAME}/platform/beos.cpp)
    SET(OPTIONAL_LIBRARY_LIBS ${OPTIONAL_LIBRARY_LIBS} be root)
ENDIF(BEOS)

# Almost everything is "compiled" here, but things that don't apply to the
#  build are #ifdef'd out. This is to make it easy to embed PhysicsFS into
#  another project or bring up a new build system: just compile all the source
#  code and #define the things you want.
SET(PHYSFS_SRCS
	${PhysFS_SOURCE_NAME}/physfs.c
    ${PhysFS_SOURCE_NAME}/physfs_byteorder.c
    ${PhysFS_SOURCE_NAME}/physfs_unicode.c
    ${PhysFS_SOURCE_NAME}/platform/os2.c
    ${PhysFS_SOURCE_NAME}/platform/pocketpc.c
    ${PhysFS_SOURCE_NAME}/platform/posix.c
    ${PhysFS_SOURCE_NAME}/platform/unix.c
    ${PhysFS_SOURCE_NAME}/platform/macosx.c
    ${PhysFS_SOURCE_NAME}/platform/windows.c
    ${PhysFS_SOURCE_NAME}/archivers/dir.c
    ${PhysFS_SOURCE_NAME}/archivers/grp.c
    ${PhysFS_SOURCE_NAME}/archivers/hog.c
    ${PhysFS_SOURCE_NAME}/archivers/lzma.c
    ${PhysFS_SOURCE_NAME}/archivers/mvl.c
    ${PhysFS_SOURCE_NAME}/archivers/qpak.c
    ${PhysFS_SOURCE_NAME}/archivers/wad.c
    ${PhysFS_SOURCE_NAME}/archivers/zip.c
    ${PHYSFS_BEOS_SRCS}
)


# platform layers ...

IF(UNIX)
    IF(BEOS)
        SET(PHYSFS_HAVE_CDROM_SUPPORT TRUE)
        SET(PHYSFS_HAVE_THREAD_SUPPORT TRUE)
        SET(HAVE_PTHREAD_H TRUE)
    ELSE(BEOS)
        # !!! FIXME
        #  AC_DEFINE([PHYSFS_HAVE_LLSEEK], 1, [define if we have llseek])
        CHECK_INCLUDE_FILE(sys/ucred.h HAVE_UCRED_H)
        IF(HAVE_UCRED_H)
            ADD_DEFINITIONS(-DPHYSFS_HAVE_SYS_UCRED_H=1)
            SET(PHYSFS_HAVE_CDROM_SUPPORT TRUE)
        ENDIF(HAVE_UCRED_H)

        CHECK_INCLUDE_FILE(mntent.h HAVE_MNTENT_H)
        IF(HAVE_MNTENT_H)
            ADD_DEFINITIONS(-DPHYSFS_HAVE_MNTENT_H=1)
            SET(PHYSFS_HAVE_CDROM_SUPPORT TRUE)
        ENDIF(HAVE_MNTENT_H)

        CHECK_INCLUDE_FILE(pthread.h HAVE_PTHREAD_H)
        IF(HAVE_PTHREAD_H)
            SET(PHYSFS_HAVE_THREAD_SUPPORT TRUE)
        ELSE(HAVE_PTHREAD_H)
            ADD_DEFINITIONS(-DPHYSFS_NO_PTHREADS_SUPPORT=1)
        ENDIF(HAVE_PTHREAD_H)
    ENDIF(BEOS)
ENDIF(UNIX)

IF(WINDOWS)
    SET(PHYSFS_HAVE_CDROM_SUPPORT TRUE)
    SET(PHYSFS_HAVE_THREAD_SUPPORT TRUE)
ENDIF(WINDOWS)

IF(NOT PHYSFS_HAVE_CDROM_SUPPORT)
    ADD_DEFINITIONS(-DPHYSFS_NO_CDROM_SUPPORT=1)
    MESSAGE(WARNING " ***")
    MESSAGE(WARNING " *** There is no CD-ROM support in this build!")
    MESSAGE(WARNING " *** PhysicsFS will just pretend there are no discs.")
    MESSAGE(WARNING " *** This may be fine, depending on how PhysicsFS is used,")
    MESSAGE(WARNING " ***   but is this what you REALLY wanted?")
    MESSAGE(WARNING " *** (Maybe fix CMakeLists.txt, or write a platform driver?)")
    MESSAGE(WARNING " ***")
ENDIF(NOT PHYSFS_HAVE_CDROM_SUPPORT)

IF(PHYSFS_HAVE_THREAD_SUPPORT)
    ADD_DEFINITIONS(-D_REENTRANT -D_THREAD_SAFE)
ELSE(PHYSFS_HAVE_THREAD_SUPPORT)
    MESSAGE(WARNING " ***")
    MESSAGE(WARNING " *** There is no thread support in this build!")
    MESSAGE(WARNING " *** PhysicsFS will NOT be reentrant!")
    MESSAGE(WARNING " *** This may be fine, depending on how PhysicsFS is used,")
    MESSAGE(WARNING " ***   but is this what you REALLY wanted?")
    MESSAGE(WARNING " *** (Maybe fix CMakeLists.txt, or write a platform driver?)")
    MESSAGE(WARNING " ***")
ENDIF(PHYSFS_HAVE_THREAD_SUPPORT)

CHECK_INCLUDE_FILE(assert.h HAVE_ASSERT_H)
IF(HAVE_ASSERT_H)
    ADD_DEFINITIONS(-DHAVE_ASSERT_H=1)
ENDIF(HAVE_ASSERT_H)



# Archivers ...

OPTION(PHYSFS_ARCHIVE_ZIP "Enable ZIP support" TRUE)
IF(PHYSFS_ARCHIVE_ZIP)
    ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_ZIP=1)
ENDIF(PHYSFS_ARCHIVE_ZIP)

OPTION(PHYSFS_ARCHIVE_7Z "Enable 7zip support" TRUE)
IF(PHYSFS_ARCHIVE_7Z)
    ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_7Z=1)
    # !!! FIXME: rename to 7z.c?
    SET(PHYSFS_SRCS ${PHYSFS_SRCS} ${LZMA_SRCS})
ENDIF(PHYSFS_ARCHIVE_7Z)

OPTION(PHYSFS_ARCHIVE_GRP "Enable Build Engine GRP support" TRUE)
IF(PHYSFS_ARCHIVE_GRP)
    ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_GRP=1)
ENDIF(PHYSFS_ARCHIVE_GRP)

OPTION(PHYSFS_ARCHIVE_WAD "Enable Doom WAD support" TRUE)
IF(PHYSFS_ARCHIVE_WAD)
    ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_WAD=1)
ENDIF(PHYSFS_ARCHIVE_WAD)

OPTION(PHYSFS_ARCHIVE_HOG "Enable Descent I/II HOG support" TRUE)
IF(PHYSFS_ARCHIVE_HOG)
    ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_HOG=1)
ENDIF(PHYSFS_ARCHIVE_HOG)

OPTION(PHYSFS_ARCHIVE_MVL "Enable Descent I/II MVL support" TRUE)
IF(PHYSFS_ARCHIVE_MVL)
    ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_MVL=1)
ENDIF(PHYSFS_ARCHIVE_MVL)

OPTION(PHYSFS_ARCHIVE_QPAK "Enable Quake I/II QPAK support" TRUE)
IF(PHYSFS_ARCHIVE_QPAK)
    ADD_DEFINITIONS(-DPHYSFS_SUPPORTS_QPAK=1)
ENDIF(PHYSFS_ARCHIVE_QPAK)


# See if some archiver required zlib, and see about using system version.

OPTION(PHYSFS_BUILD_STATIC "Build static library" TRUE)
IF(PHYSFS_BUILD_STATIC)
    ADD_LIBRARY(physfs-static STATIC ${PHYSFS_SRCS})
    SET_TARGET_PROPERTIES(physfs-static PROPERTIES OUTPUT_NAME "physfs")
    SET(PHYSFS_LIB_TARGET physfs-static)
    SET(PHYSFS_INSTALL_TARGETS ${PHYSFS_INSTALL_TARGETS} ";physfs-static")
ENDIF(PHYSFS_BUILD_STATIC)

OPTION(PHYSFS_BUILD_SHARED "Build shared library" TRUE)
IF(PHYSFS_BUILD_SHARED)
    ADD_LIBRARY(physfs SHARED ${PHYSFS_SRCS})
    SET_TARGET_PROPERTIES(physfs PROPERTIES VERSION ${PHYSFS_VERSION})
    SET_TARGET_PROPERTIES(physfs PROPERTIES SOVERSION ${PHYSFS_SOVERSION})
    TARGET_LINK_LIBRARIES(physfs ${OPTIONAL_LIBRARY_LIBS} ${OTHER_LDFLAGS})
    SET(PHYSFS_LIB_TARGET physfs)
    SET(PHYSFS_INSTALL_TARGETS ${PHYSFS_INSTALL_TARGETS} ";physfs")
ENDIF(PHYSFS_BUILD_SHARED)

IF(NOT PHYSFS_BUILD_SHARED AND NOT PHYSFS_BUILD_STATIC)
    MESSAGE(FATAL "Both shared and static libraries are disabled!")
ENDIF(NOT PHYSFS_BUILD_SHARED AND NOT PHYSFS_BUILD_STATIC)

# CMake FAQ says I need this...
IF(PHYSFS_BUILD_SHARED AND PHYSFS_BUILD_STATIC)
    SET_TARGET_PROPERTIES(physfs PROPERTIES CLEAN_DIRECT_OUTPUT 1)
    SET_TARGET_PROPERTIES(physfs-static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
ENDIF(PHYSFS_BUILD_SHARED AND PHYSFS_BUILD_STATIC)

IF(UNIX)
    ADD_CUSTOM_TARGET(dist ./extras/makedist.sh ${PHYSFS_VERSION} COMMENT "Building source tarball")
ENDIF(UNIX)

MACRO(MESSAGE_BOOL_OPTION _NAME _VALUE)
    IF(${_VALUE})
        MESSAGE(STATUS "  ${_NAME}: enabled")
    ELSE(${_VALUE})
        MESSAGE(STATUS "  ${_NAME}: disabled")
    ENDIF(${_VALUE})
ENDMACRO(MESSAGE_BOOL_OPTION)

MESSAGE(STATUS "PhysicsFS will build with the following options:")
MESSAGE_BOOL_OPTION("ZIP support" PHYSFS_ARCHIVE_ZIP)
MESSAGE_BOOL_OPTION("7zip support" PHYSFS_ARCHIVE_7Z)
MESSAGE_BOOL_OPTION("GRP support" PHYSFS_ARCHIVE_GRP)
MESSAGE_BOOL_OPTION("WAD support" PHYSFS_ARCHIVE_WAD)
MESSAGE_BOOL_OPTION("HOG support" PHYSFS_ARCHIVE_HOG)
MESSAGE_BOOL_OPTION("MVL support" PHYSFS_ARCHIVE_MVL)
MESSAGE_BOOL_OPTION("QPAK support" PHYSFS_ARCHIVE_QPAK)
MESSAGE_BOOL_OPTION("CD-ROM drive support" PHYSFS_HAVE_CDROM_SUPPORT)
MESSAGE_BOOL_OPTION("Thread safety" PHYSFS_HAVE_THREAD_SUPPORT)
MESSAGE_BOOL_OPTION("Build static library" PHYSFS_BUILD_STATIC)
MESSAGE_BOOL_OPTION("Build shared library" PHYSFS_BUILD_SHARED)
MESSAGE_BOOL_OPTION("Build wxWidgets test program" PHYSFS_BUILD_WX_TEST)
MESSAGE_BOOL_OPTION("Build stdio test program" PHYSFS_BUILD_TEST)
IF(PHYSFS_BUILD_TEST)
    MESSAGE_BOOL_OPTION("  Use readline in test program" HAVE_SYSTEM_READLINE)
ENDIF(PHYSFS_BUILD_TEST)

# end of CMakeLists.txt for PHYSFS

if (NOT PhysFS_STATICLIB_OUTPUT_PATH)
	set(PhysFS_STATICLIB_OUTPUT_PATH "." CACHE STRING
		"PhysFS static lib output directory")
endif (NOT PhysFS_STATICLIB_OUTPUT_PATH)
option(PHYSFS_RESET_CONFIG "Reset PHYSFS Configuration" TRUE)
if (PHYSFS_RESET_CONFIG)
	set (PHYSFS_BUILD_SHARED FALSE CACHE BOOL "build shared library" FORCE)
	set (LIBRARY_OUTPUT_PATH "${PhysFS_STATICLIB_OUTPUT_PATH}" CACHE STRING
		"physfs library output directory" FORCE)
	set (PHYSFS_ARCHIVE_7Z TRUE CACHE BOOL "" FORCE)
	set (PHYSFS_ARCHIVE_GRP FALSE CACHE BOOL "" FORCE)
	set (PHYSFS_ARCHIVE_HOG FALSE CACHE BOOL "" FORCE)
	set (PHYSFS_ARCHIVE_MVL FALSE CACHE BOOL "" FORCE)
	set (PHYSFS_ARCHIVE_QPAK FALSE CACHE BOOL "" FORCE)
	set (PHYSFS_ARCHIVE_WAD FALSE CACHE BOOL "" FORCE)
	set (PHYSFS_ARCHIVE_ZIP TRUE CACHE BOOL "" FORCE)
	set (PHYSFS_RESET_CONFIG FALSE CACHE BOOL "Reset PHYSFS Config" FORCE)
endif (PHYSFS_RESET_CONFIG)
