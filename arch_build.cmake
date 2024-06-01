###########################################################
#
# TEC platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TEC configuration
set(TEC_PLATFORM_CONFIG_FILE_LIST
  tec_internal_cfg.h
  tec_platform_cfg.h
  tec_perfids.h
  tec_msgids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(TEC_CFGFILE ${TEC_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${TEC_CFGFILE}" NAME_WE)
  if (DEFINED TEC_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${TEC_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${TEC_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${TEC_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
