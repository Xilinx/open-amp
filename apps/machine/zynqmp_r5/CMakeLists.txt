collect (APP_COMMON_SOURCES platform_info.c)
collect (APP_COMMON_SOURCES rsc_table.c)
collect (APP_COMMON_SOURCES zynqmp_r5_a53_rproc.c)

# For now this is unconditionally included.
# Once surrounding tooling accommodates CMake-configure steps for apps
# this will move to be included conditionally based on CMake flags.
collect (APP_COMMON_SOURCES platform_info_zynqmp.c)
collect (APP_COMMON_SOURCES debug_info.c)

collect (APP_INC_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

