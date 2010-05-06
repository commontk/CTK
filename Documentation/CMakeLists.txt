FIND_PACKAGE( Doxygen )

IF( DOXYGEN_FOUND )
  IF( DOXYGEN_DOT_FOUND )
  
    SET( CTK_USER_DOCUMENTATION "NO" )

    CONFIGURE_FILE( Doxyfile.txt.in
      ${CTK_BINARY_DIR}/Documentation/UserDoxyfile.txt
    )

    ADD_CUSTOM_TARGET( doc 
      ${DOXYGEN_EXECUTABLE} ${CTK_BINARY_DIR}/Documentation/Doxyfile.txt 
      DEPENDS ${CTK_BINARY_DIR}/Documentation/UserDoxyfile.txt
    ) 

    SET( CTK_USER_DOCUMENTATION "YES" )

    CONFIGURE_FILE( Doxyfile.txt.in
      ${CTK_BINARY_DIR}/Documentation/DevDoxyfile.txt
    )

    ADD_CUSTOM_TARGET( doc-dev 
      ${DOXYGEN_EXECUTABLE} ${CTK_BINARY_DIR}/Documentation/Doxyfile.txt 
      DEPENDS ${CTK_BINARY_DIR}/Documentation/DevDoxyfile.txt
    ) 
  ELSE( DOXYGEN_DOT_FOUND )
    MESSAGE( SEND_ERROR
      "Warning: Doxygen not found - Documentation will not be created"
      )
  ENDIF( DOXYGEN_DOT_FOUND )
ENDIF( DOXYGEN_FOUND )
