include(CheckSymbolExists)

collect (APP_COMMON_SOURCES helper.c)

# If not provided, add default value for resource table
if(NOT DEFINED ${RSC_TABLE})
  set(RSC_TABLE 0x3ed20000 CACHE STRING "")
endif(NOT DEFINED ${RSC_TABLE})

set (_linker_script "${CMAKE_CURRENT_SOURCE_DIR}/linker_remote.ld")
string(REGEX MATCH "SDT" HAS_SYSTEM_DT "${CMAKE_C_FLAGS}")
check_symbol_exists(VERSAL_NET "bspconfig.h" IS_VERSAL_NET)
  if (HAS_SYSTEM_DT)
  if (IS_VERSAL_NET)
    set (_linker_script "${CMAKE_CURRENT_SOURCE_DIR}/linker_sdt_versal_net.ld")
  else()
    set (_linker_script "${CMAKE_CURRENT_SOURCE_DIR}/linker_sdt.ld")
  endif(IS_VERSAL_NET)
endif(HAS_SYSTEM_DT)

set_property (GLOBAL PROPERTY APP_LINKER_OPT "-Wl,--defsym,_rsc_table=${RSC_TABLE} -T\"${_linker_script}\"")
set (_linker_large_text_script "${CMAKE_CURRENT_SOURCE_DIR}/linker_large_text.ld")
set_property (GLOBAL PROPERTY APP_LINKER_LARGE_TEXT_OPT "-Wl,--defsym,_rsc_table=${RSC_TABLE} -T\"${_linker_large_text_script}\"")


