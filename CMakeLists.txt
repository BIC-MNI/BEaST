SET(BEAST_PACKAGE_VERSION_MAJOR 1)
SET(BEAST_PACKAGE_VERSION_MINOR 90)
SET(BEAST_PACKAGE_VERSION_PATCH 00)

IF(NOT MINC_TOOLKIT_BUILD)
  PROJECT(mincbeast)
  ENABLE_TESTING()
  INCLUDE(CTest)

  # Packaging defines
  SET(CPACK_PACKAGE_VERSION_MAJOR ${BEAST_PACKAGE_VERSION_MAJOR})
  SET(CPACK_PACKAGE_VERSION_MINOR ${BEAST_PACKAGE_VERSION_MINOR})
  SET(CPACK_PACKAGE_VERSION_PATCH ${BEAST_PACKAGE_VERSION_PATCH})
  
  INCLUDE(CPack)
  CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
  
  if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
  endif(COMMAND cmake_policy)
  
  FIND_PACKAGE( LIBMINC )

  IF(LIBMINC_FOUND)
    FIND_PACKAGE( HDF5 )
    FIND_PACKAGE( NETCDF )
    FIND_PACKAGE( ZLIB )
  ENDIF(LIBMINC_FOUND)

  
  OPTION(USE_NIFTI      "Use niftiio library" ON)
  OPTION(USE_DMALLOC    "Use dmalloc library to debug memory leaks" OFF)

  IF(USE_NIFTI)
    SET(NIFTI_ROOT "/usr/local/bic" CACHE PATH "niftiio library base")
  ENDIF(USE_NIFTI)

  OPTION(MT_USE_OPENMP  "Use OpenMP multi-threading" OFF)
  OPTION(MT_SPARSE_BEAST  "Build sparse BEAST (experimental)" OFF)

  IF(MT_SPARSE_BEAST)
    FIND_PACKAGE(OpenBLAS )
  ENDIF(MT_SPARSE_BEAST)
ELSE()
    IF(BLAS_FOUND)
      SET(OpenBLAS_INCLUDE_DIR ${OpenBLAS_INCLUDE_DIRS} CACHE PATH "OpenBLAS Include directory")
      SET(OpenBLAS_LIBRARY     ${OpenBLAS_LIBRARIES}    CACHE PATH "OpenBLAS library ")
    ENDIF()
ENDIF()


IF(BLAS_FOUND AND MT_SPARSE_BEAST)
  SET(BUILD_SPAMS ON)
  ADD_DEFINITIONS(-DHAVE_OPENBLAS -DUSE_SPAMS)
ELSE()
  SET(BUILD_SPAMS OFF)
ENDIF()

SET(PACKAGE "mincbeast")

SET(PACKAGE_BUGREPORT "eskild@gmail.com")

SET(PACKAGE_NAME "mincbeast")
SET(PACKAGE_VERSION "${BEAST_PACKAGE_VERSION_MAJOR}.${BEAST_PACKAGE_VERSION_MINOR}.${BEAST_PACKAGE_VERSION_PATCH}")
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

#setup variables only needed inside scripts
SET(version ${PACKAGE_VERSION})
SET(long_version ${PACKAGE_STRING})
SET(VERSION "${PACKAGE_VERSION}")

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

# check for prereqs
# INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFiles)

# CHECK_FUNCTION_EXISTS(finite   HAVE_FINITE)
# CHECK_FUNCTION_EXISTS(isfinite HAVE_ISFINITE)

CHECK_INCLUDE_FILES(float.h     HAVE_FLOAT_H)
CHECK_INCLUDE_FILES(sys/stat.h  HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(values.h    HAVE_VALUES_H)
CHECK_INCLUDE_FILES(unistd.h    HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(dirent.h    HAVE_DIRENT_H)
CHECK_INCLUDE_FILES(memory.h    HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(malloc.h    HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(stdlib.h    HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(fcntl.h     HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(vfork.h     HAVE_VFORK_H)
CHECK_INCLUDE_FILES(inttypes.h  HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(string.h    HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h   HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(math.h      HAVE_MATH_H)

SET(BEAST_LIBRARY_PREFIX "${CMAKE_INSTALL_PREFIX}/share/beast-library-1.0")

if (MT_USE_OPENMP)
    FIND_PACKAGE(OpenMP)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}   ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else()
        message(WARNING "OpenMP NOT found")
    endif()
endif()


# others
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR})

IF(BUILD_SPAMS)
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include 
        ${CMAKE_SOURCE_DIR}/spams-2.6.1 
        ${CMAKE_SOURCE_DIR}/spams-2.6.1/spams/linalg
        ${CMAKE_SOURCE_DIR}/spams-2.6.1/spams/decomp
        ${CMAKE_SOURCE_DIR}/spams-2.6.1/spams/prox
        ${CMAKE_SOURCE_DIR}/spams-2.6.1/spams/dictLearn/
        )

    SET(SPAMS_SRC
        spams-2.6.1/spams/linalg/misc.h
        spams-2.6.1/spams/linalg/cblas_defvar.h
        spams-2.6.1/spams/linalg/cblas_alt_template.h
        spams-2.6.1/spams/linalg/utils.h
        spams-2.6.1/spams/linalg/linalg.h
        spams-2.6.1/spams/linalg/mexutils.h
        spams-2.6.1/spams/linalg/list.h
        spams-2.6.1/spams/linalg/cblas_template.h
        spams-2.6.1/spams/decomp/decomp.h
        spams-2.6.1/spams/decomp/lsqsplx.h
        spams-2.6.1/spams/decomp/projsplx.h
        spams-2.6.1/spams/dictLearn/dicts.h
        spams-2.6.1/spams/dictLearn/arch.h
        spams-2.6.1/spams/prox/fista.h
        spams-2.6.1/spams/prox/surrogate.h
        spams-2.6.1/spams/prox/project.h
        spams-2.6.1/spams/prox/svm.h
        spams-2.6.1/spams/prox/mexgrouputils.h
        spams-2.6.1/spams/prox/groups-graph.h
        spams-2.6.1/spams.h
    )
    
ENDIF(BUILD_SPAMS)

SET(BEAST_SRC 
  alloc.cpp
  beast_lib.cpp
  distance_patch.cpp
  moments.cpp
  nlm_utils.cpp
  nlmfilter.cpp
  nlmsegFuzzy.cpp
  nlmsegSparse.cpp
  label.cpp
  ParseArgv.c
  beast_config.cpp
  mincbeast_v1.cpp
  mincbeast_v2.cpp
)

IF(BUILD_SPAMS)
    message("Building SPAMS")
    INCLUDE_DIRECTORIES(${OpenBLAS_INCLUDE_DIRS})

    SET(BEAST_SRC ${BEAST_SRC} ${SPAMS_SRC})
    # test SPAMS
    ADD_EXECUTABLE(spams_test
        spams_test.cpp
    )
    message("OpenBLAS_LIBRARY=${OpenBLAS_LIBRARY}")

    TARGET_LINK_LIBRARIES( spams_test
                  ${OpenBLAS_LIBRARIES} )
ENDIF(BUILD_SPAMS)

IF(LIBMINC_FOUND)
  INCLUDE( ${LIBMINC_USE_FILE} )

  IF(LIBMINC_FOUND)
    FIND_PACKAGE( HDF5 )
    FIND_PACKAGE( NETCDF )
    FIND_PACKAGE( ZLIB )
  ENDIF(LIBMINC_FOUND)

  LINK_DIRECTORIES(
    ${HDF5_LINK_DIRECTORIES}
    ${NETCDF_LINK_DIRECTORIES}
    ${ZLIB_LINK_DIRECTORIES}
  )
  
  INCLUDE_DIRECTORIES( 
    ${HDF5_INCLUDE_DIRS}
    ${NETCDF_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
  )

  SET(BEAST_SRC ${BEAST_SRC} mincio.cpp)
  ADD_DEFINITIONS( -DHAVE_MINC )
ENDIF(LIBMINC_FOUND)


IF(USE_NIFTI)
  ADD_DEFINITIONS( -DHAVE_NIFTI )
  LINK_LIBRARIES(niftiio znz z m)
  SET(BEAST_SRC ${BEAST_SRC} niftiio.cpp)
  LINK_DIRECTORIES(
    "${NIFTI_ROOT}/lib${LIB_SUFFIX}"
  )
  INCLUDE_DIRECTORIES( 
    "${NIFTI_ROOT}/include/nifti"
  )
ENDIF(USE_NIFTI)


IF(USE_DMALLOC)
  ADD_DEFINITIONS( -DUSE_DMALLOC -DDMALLOC_FUNC_CHECK  )
  LINK_LIBRARIES(dmalloc)
ENDIF(USE_DMALLOC)

ADD_LIBRARY(beast  STATIC 
  ${BEAST_SRC}
)

TARGET_LINK_LIBRARIES(beast 
    ${VOLUME_IO_LIBRARIES} 
    ${LIBMINC_LIBRARIES} )

IF(BUILD_SPAMS)
  TARGET_LINK_LIBRARIES( beast
                  ${OpenBLAS_LIBRARIES} )
ENDIF(BUILD_SPAMS)

ADD_EXECUTABLE(mincbeast
  mincbeast.cpp
)

TARGET_LINK_LIBRARIES(mincbeast beast m)

IF(LIBMINC_FOUND AND USE_NIFTI)
  ADD_EXECUTABLE(minc2nifti
    minc2nifti.cpp
    )
  TARGET_LINK_LIBRARIES(minc2nifti beast)
ENDIF(LIBMINC_FOUND AND USE_NIFTI)

ADD_SUBDIRECTORY (scripts)

INSTALL(TARGETS 
        mincbeast
        #mincbeast-v2
        RUNTIME DESTINATION bin )

IF(BUILD_TESTING)
   ADD_SUBDIRECTORY( Testing )
ENDIF(BUILD_TESTING)

# VF: now this will be in a separate project
# INSTALL(DIRECTORY free_database/
#         DESTINATION share/mincbeast_free )
# TODO download and create average models
