FIND_PACKAGE( Doxygen )

IF( DOXYGEN_FOUND )
  IF( DOXYGEN_DOT_FOUND )
    CONFIGURE_FILE( Doxyfile.txt.in
      ${CTK_BINARY_DIR}/Documentation/Doxyfile.txt
    )

    ADD_CUSTOM_TARGET( doc ALL 
      ${DOXYGEN_EXECUTABLE} ${CTK_BINARY_DIR}/Documentation/Doxyfile.txt 
      DEPENDS ${CTK_BINARY_DIR}/Documentation/Doxyfile.txt
    ) 
  ELSE( DOXYGEN_DOT_FOUND )
    MESSAGE( SEND_ERROR
      "Warning: Doxygen not found - Documentation will not be created"
      )
  ENDIF( DOXYGEN_DOT_FOUND )
ENDIF( DOXYGEN_FOUND )