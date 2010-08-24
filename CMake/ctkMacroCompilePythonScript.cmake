

#
# Based on ParaView/VTK/Utilities/vtkTclTest2Py/CMakeLists.txt and
#          ParaView/VTK/Wrapping/Python/CMakeLists.txt
#

MACRO(ctkMacroCompilePythonScript kit_name python_source_files dest_dir)

  FIND_PACKAGE(PythonInterp REQUIRED)
  
  SET(copied_python_files)
  
  FOREACH(file ${python_source_files})
    GET_FILENAME_COMPONENT(file_we "${CMAKE_CURRENT_SOURCE_DIR}/${file}" NAME_WE)
    SET(src "${CMAKE_CURRENT_SOURCE_DIR}/${file_we}.py")
    SET(tgt "${dest_dir}/${file_we}.py")
    SET(copied_python_files ${copied_python_files} ${tgt})
    ADD_CUSTOM_COMMAND(DEPENDS ${src}
                        COMMAND ${CMAKE_COMMAND} -E copy ${src} ${tgt}
                        OUTPUT ${tgt}
                        COMMENT "source copy")
  ENDFOREACH()
  
  ADD_CUSTOM_TARGET(Copy${kit_name}PythonFiles
                  ALL
                  DEPENDS ${copied_python_files})
  
  # Byte compile the Python files.
  SET(compile_all_script "${CMAKE_CURRENT_BINARY_DIR}/compile_all_${kit_name}.py")
  
  # Based on paraview/VTK/Wrapping/Python/compile_all_vtk.py.in
  FILE(WRITE ${compile_all_script} "
\# Auto-generated
import compileall
compileall.compile_dir('${dest_dir}')
file = open('${CMAKE_CURRENT_BINARY_DIR}/${kit_name}_python_compile_complete', 'w')
file.write('Done')
")

  ADD_CUSTOM_COMMAND(
    COMMAND ${PYTHON_EXECUTABLE}
    ARGS  "${compile_all_script}"
    DEPENDS ${copied_python_files}  ${compile_all_script}
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${kit_name}_python_compile_complete"
    )
  
  ADD_CUSTOM_TARGET(${kit_name}_pyc ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${kit_name}_python_compile_complete")


  # Install the slicer python module directory
  #INSTALL(DIRECTORY "${Slicer3_BINARY_DIR}/bin/Python"
  #  DESTINATION "${Slicer3_INSTALL_BIN_DIR}" COMPONENT Runtime
  #  USE_SOURCE_PERMISSIONS)

ENDMACRO()
