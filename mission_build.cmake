###########################################################
#
# TEC mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TEC configuration
set(TEC_MISSION_CONFIG_FILE_LIST
  tec_fcncodes.h
  tec_interface_cfg.h
  tec_mission_cfg.h
  tec_perfids.h
  tec_msg.h
  tec_msgdefs.h
  tec_msgstruct.h
  tec_tbl.h
  tec_tbldefs.h
  tec_tblstruct.h
  tec_topicids.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, these files come generated from the EDS tool
  set(TEC_CFGFILE_SRC_tec_interface_cfg "tec_eds_designparameters.h")
  set(TEC_CFGFILE_SRC_tec_tbldefs       "tec_eds_typedefs.h")
  set(TEC_CFGFILE_SRC_tec_tblstruct     "tec_eds_typedefs.h")
  set(TEC_CFGFILE_SRC_tec_msgdefs       "tec_eds_typedefs.h")
  set(TEC_CFGFILE_SRC_tec_msgstruct     "tec_eds_typedefs.h")
  set(TEC_CFGFILE_SRC_tec_fcncodes      "tec_eds_cc.h")

endif(CFE_EDS_ENABLED_BUILD)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(TEC_CFGFILE ${TEC_MISSION_CONFIG_FILE_LIST})
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
