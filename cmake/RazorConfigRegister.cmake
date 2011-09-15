MACRO (razor_config_register DesktopFile)
    #
    # Create a dummy symlink from desktop file to RAZOR_CONFIG_MODULES_DIR
    # where are expected apps to be listed in razor-config
    #
    install(FILES ${DesktopFile} DESTINATION share/razor/razor-config)
    # code below does not work with rpm building
    #   it is not symlinked (duplicated files) but it works at least...

#    install(CODE "message(STATUS \"Registering ${DesktopFile} into config framework: ${RAZOR_CONFIG_MODULES_DIR} \")")
#    install(CODE "execute_process(COMMAND mkdir -p ${RAZOR_CONFIG_MODULES_DIR})")
    # this should work but it doesn't. Dunno why and I'm so bored to try it...
    #install(CODE "execute_process(COMMAND \"${CMAKE_COMMAND}\" -E create_symlink \"${DesktopFile}\" \"${RAZOR_CONFIG_MODULES_DIR}\" OUTPUT_VARIABLE cout ERROR_VARIABLE cout)")
#    install(CODE "execute_process(COMMAND ln -s \"${DesktopFile}\" \"${RAZOR_CONFIG_MODULES_DIR}\" OUTPUT_VARIABLE cout ERROR_VARIABLE cout)")
    #install(CODE "message(STATUS \"    result: ${cout}\")")
ENDMACRO (razor_config_register)
