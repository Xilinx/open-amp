collect (APP_COMMON_SOURCES platform_info.c)
collect (APP_COMMON_SOURCES rsc_table.c)
collect (APP_COMMON_SOURCES zynqmp_r5_a53_rproc.c)

# For now this is unconditionally included.
# Once surrounding tooling accommodates CMake-configure steps for apps
# this will move to be included conditionally based on CMake flags.
collect (APP_COMMON_SOURCES platform_info_versal_net.c)
collect (APP_COMMON_SOURCES platform_info_versal.c)
collect (APP_COMMON_SOURCES platform_info_zynqmp.c)
collect (APP_COMMON_SOURCES debug_info.c)

collect (APP_INC_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

find_path(XIL_INCLUDE_DIR NAMES xparameters.h PATHS ${CMAKE_FIND_ROOT_PATH})
collect (PROJECT_INC_DIRS "${XIL_INCLUDE_DIR}")

find_library(LIBXIL_LIB NAMES xil PATHS ${CMAKE_FIND_ROOT_PATH})
get_filename_component(LIBXIL_LIB_DIR ${LIBXIL_LIB} DIRECTORY)
collect(PROJECT_LIB_DIRS ${LIBXIL_LIB_DIR})

# libxil provides AMD-Xilinx BSP APIs to access Xilinx hardware.
collect(PROJECT_LIB_DEPS xil)

# C standard library
collect(PROJECT_LIB_DEPS c)
