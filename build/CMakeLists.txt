
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


INCLUDE(FindPkgConfig)
SET(CMAKE_SYSTEM_NAME Linux)

PROJECT(kmscube C CXX) 

SET(KMSCUBE_OBJS 
    ../esTransform.c 
    ../kms.cpp 
    ../kmscube.cpp
    )
             
# Needed for openal.pc.in
SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix "\${prefix}")
SET(libdir "\${exec_prefix}/lib${LIB_SUFFIX}")
SET(bindir "\${exec_prefix}/bin")
SET(includedir "\${prefix}/include")
SET(PACKAGE_VERSION "${LIB_VERSION}")

pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(GLESv2 REQUIRED glesv2)
pkg_check_modules(GBM REQUIRED gbm)
pkg_check_modules(DRM REQUIRED libdrm)
pkg_check_modules(UDEV REQUIRED libudev)

SET(KMSCUBE_LIBRARIES 
    ${KMSCUBE_LIBRARIES} 
    ${GLESv2_LDFLAGS} 
    ${EGL_LDFLAGS} 
    ${GBM_LDFLAGS} 
    ${DRM_LDFLAGS}
    ${UDEV_LDFLAGS}) 
SET(KMSCUBE_CFLAGS 
    ${EGL_CFLAGS}
    ${GLESv2_CFLAGS}
    ${GBM_CFLAGS}
    ${DRM_CFLAGS}
    ${UDEV_CFLAGS})
MESSAGE( "${KMSCUBE_LIBRARIES} ${KMSCUBE_CFLAGS}" )

ADD_EXECUTABLE(kmscube ${KMSCUBE_OBJS})
TARGET_LINK_LIBRARIES(kmscube ${KMSCUBE_LIBRARIES} ${LIBNAME})
SET_TARGET_PROPERTIES(kmscube PROPERTIES COMPILE_FLAGS "")

