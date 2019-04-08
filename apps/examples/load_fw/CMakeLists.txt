collector_list (_app_extra_c_flags APP_EXTRA_C_FLAGS)
set (_cflags "${CMAKE_C_FLAGS} ${_app_extra_c_flags}")
set (_fw_dir "${APPS_SHARE_DIR}")

collector_list (_list PROJECT_INC_DIRS)
collector_list (_app_list APP_INC_DIRS)
include_directories (${_list} ${_app_list} ${CMAKE_CURRENT_SOURCE_DIR})

collector_list (_list PROJECT_LIB_DIRS)
collector_list (_app_list APP_LIB_DIRS)
link_directories (${_list} ${_app_list})

get_property (_linker_opt GLOBAL PROPERTY APP_LINKER_OPT)
collector_list (_deps PROJECT_LIB_DEPS)

set (OPENAMP_LIB open_amp)
# set default value to LOAD_FW_TARGET if not provided.
if(NOT LOAD_FW_TARGET)
  if(VERSION_2_PM_CLIENT)
    set(LOAD_FW_TARGET "XPM_NODEIDX_DEV_ACPU_0")
  elseif(VERSION_1_PM_CLIENT)
    set(LOAD_FW_TARGET "NODE_APU_1")
  endif(VERSION_2_PM_CLIENT)
endif(NOT LOAD_FW_TARGET)
add_definitions( -DLOAD_FW_TARGET=${LOAD_FW_TARGET} )

foreach (_app load_fw)
  collector_list (_sources APP_COMMON_SOURCES)
  list (APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/${_app}.c")
  list (APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/mem_image_store.c")
  list (APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/zynqmp_apu_lcm_rproc_example.c")
  if(VERSION_1_PM_CLIENT)
    list (APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/zynqmp_rpu_lcm_rproc_example.c")
  endif(VERSION_1_PM_CLIENT)
  list (APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/platform_info.c")

  if (WITH_STATIC_LIB)
    add_executable (${_app}.out ${_sources})
    set_source_files_properties(${_sources} PROPERTIES COMPILE_FLAGS "${_cflags}")

    target_link_libraries(${_app}.out -Wl,-Map=${_app}.map -Wl,--gc-sections -T"${CMAKE_CURRENT_SOURCE_DIR}/lscript.ld" -Wl,--start-group ${OPENAMP_LIB}-static ${_deps} -Wl,--end-group)

    install (TARGETS ${_app}.out RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  endif (WITH_STATIC_LIB)
endforeach(_app)
