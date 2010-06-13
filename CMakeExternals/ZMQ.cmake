#
# ZMQ
#
SET(ZMQ_DEPENDS)
ctkMacroShouldAddExternalProject(ZMQ_LIBRARIES add_project)
IF(${add_project})

  # Sanity checks
  IF(DEFINED ZMQ_DIR AND NOT EXISTS ${ZMQ_DIR})
    MESSAGE(FATAL_ERROR "ZMQ_DIR variable is defined but corresponds to non-existing directory")
  ENDIF()
  
  IF(NOT DEFINED ZMQ_DIR)
    SET(proj ZMQ)
  #   MESSAGE(STATUS "Adding project:${proj}")
    SET(ZMQ_DEPENDS ${proj})
    ExternalProject_Add(${proj}
        GIT_REPOSITORY git://github.com/PatrickCheng/zeromq2.git
        INSTALL_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
          ${ep_common_args}
		  -DBUILD_SHARED_LIBS:BOOL=ON 
		  -DZMQ_BUILD_DEVICES:BOOL=ON
		  -DZMQ_BUILD_PERFORMANCE_TESTS:BOOL=ON
      )
	  SET(ZMQ_DIR ${ep_build_dir}/${proj})
	ENDIF()
ENDIF()
