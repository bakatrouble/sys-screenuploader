if (NOT SWITCH)
    cmake_panic("These utils can only be used if you are using the Switch toolchain file.")
endif ()

#############
## ELF2NRO ##
#############
if (NOT ELF2NRO)
    find_program(ELF2NRO elf2nro ${DEVKITPRO}/tools/bin)
    if (ELF2NRO)
        cmake_info("elf2nro: ${ELF2NRO} - found")
    else ()
        cmake_warning("elf2nro - not found")
    endif ()
endif ()

#############
## ELF2KIP ##
#############
if (NOT ELF2KIP)
    find_program(ELF2KIP elf2kip ${DEVKITPRO}/tools/bin)
    if (ELF2KIP)
        cmake_info("elf2kip: ${ELF2KIP} - found")
    else ()
        cmake_warning("elf2kip - not found")
    endif ()
endif ()

#############
## ELF2NSO ##
#############
if (NOT ELF2NSO)
    find_program(ELF2NSO elf2nso ${DEVKITPRO}/tools/bin)
    if (ELF2NSO)
        cmake_info("elf2nso: ${ELF2NSO} - found")
    else ()
        cmake_warning("elf2nso - not found")
    endif ()
endif ()

#############
##  BIN2S  ##
#############
if (NOT BIN2S)
    find_program(BIN2S bin2s ${DEVKITPRO}/tools/bin)
    if (BIN2S)
        cmake_info("bin2s: ${BIN2S} - found")
    else ()
        cmake_warning("bin2s - not found")
    endif ()
endif ()

#############
##  RAW2C  ##
#############
if (NOT RAW2C)
    find_program(RAW2C raw2c ${DEVKITPRO}/tools/bin)
    if (RAW2C)
        cmake_info("raw2c: ${RAW2C} - found")
    else ()
        cmake_warning("raw2c - not found")
    endif ()
endif ()

##################
##  BUILD_PFS0  ##
##################
if (NOT BUILD_PFS0)
    find_program(BUILD_PFS0 build_pfs0 ${DEVKITPRO}/tools/bin)
    if (BUILD_PFS0)
        cmake_info("build_pfs0: ${BUILD_PFS0} - found")
    else ()
        cmake_warning("build_pfs0 - not found")
    endif ()
endif ()

################
##  NACPTOOL  ##
################
if (NOT NACPTOOL)
    find_program(NACPTOOL nacptool ${DEVKITPRO}/tools/bin)
    if (NACPTOOL)
        cmake_info("nacptool: ${NACPTOOL} - found")
    else ()
        cmake_warning("nacptool - not found")
    endif ()
endif ()

################
##  NPDMTOOL  ##
################
if (NOT NPDMTOOL)
    find_program(NPDMTOOL npdmtool ${DEVKITPRO}/tools/bin)
    if (NPDMTOOL)
        cmake_info("npdmtool: ${NPDMTOOL} - found")
    else ()
        cmake_warning("npdmtool - not found")
    endif ()
endif ()

macro(acquire_homebrew_icon target)
    # This basically imitates the behavior of the Makefiles
    # from the switchbrew/switch-examples repository.
    if (EXISTS ${target}.jpg)
        set(APP_ICON ${target}.jpg)
    elseif (EXISTS ${PROJECT_SOURCE_DIR}/assets/icon.jpg)
        set(APP_ICON ${PROJECT_SOURCE_DIR}/assets/icon.jpg)
    elseif (LIBNX)
        set(APP_ICON ${LIBNX}/default_icon.jpg)
    else ()
        cmake_panic("No icon found, please provide one!")
    endif ()
endmacro()

macro(acquire_app_json target)
    if (EXISTS ${PROJECT_SOURCE_DIR}/${target}.json)
        set(APP_JSON ${PROJECT_SOURCE_DIR}/${target}.json)
    elseif (EXISTS ${PROJECT_SOURCE_DIR}/config.json)
        set(APP_JSON ${PROJECT_SOURCE_DIR}/config.json)
    endif()
endmacro()

function(add_nso_target target)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.nso
            COMMAND ${ELF2NSO} ${CMAKE_CURRENT_BINARY_DIR}/${target}.elf ${CMAKE_CURRENT_BINARY_DIR}/${target}.nso
            DEPENDS ${target}.elf
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            VERBATIM)

    if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        add_custom_target(${target}.nso ALL SOURCES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}.nso)
    else ()
        add_custom_target(${target}.nso ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${target}.nso)
    endif ()
endfunction()

function(add_nacp target)
    set(__NACP_COMMAND ${NACPTOOL} --create ${APP_TITLE} ${APP_AUTHOR} ${APP_VERSION} ${CMAKE_CURRENT_BINARY_DIR}/${target})

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}
            COMMAND ${__NACP_COMMAND}
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            VERBATIM)
endfunction()

function(add_npdm_target target)
    if (NOT APP_JSON)
        cmake_panic("APP_JSON was not found!")
    endif()

    set(__NPDM_COMMAND ${NPDMTOOL} ${APP_JSON} ${CMAKE_CURRENT_BINARY_DIR}/${target}.npdm)

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.npdm
        COMMAND ${__NPDM_COMMAND}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        VERBATIM)

    if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        add_custom_target(${target}.npdm ALL SOURCES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}.npdm)
    else ()
        add_custom_target(${target}.npdm ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${target}.npdm)
    endif ()
endfunction()

function(add_kip_target target)
    if (NOT APP_JSON)
        cmake_panic("APP_JSON was not found!")
    endif()

    set(__KIP_COMMAND
            ${ELF2KIP} $<TARGET_FILE:${target}.elf> ${APP_JSON} ${CMAKE_CURRENT_BINARY_DIR}/${target}.kip)

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.kip
            COMMAND ${__KIP_COMMAND}
            DEPENDS ${target}.elf
            VERBATIM)

    if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        add_custom_target(${target}.kip ALL SOURCES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}.kip)
    else ()
        add_custom_target(${target}.kip ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${target}.kip)
    endif ()
endfunction()

function(add_nsp_target target)
    set(__PFS0_COMMAND ${BUILD_PFS0} ${CMAKE_CURRENT_BINARY_DIR}/exefs ${CMAKE_CURRENT_BINARY_DIR}/${target}.nsp)

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.nsp
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/exefs
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${target}.nso ${CMAKE_CURRENT_BINARY_DIR}/exefs/main
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${target}.npdm ${CMAKE_CURRENT_BINARY_DIR}/exefs/main.npdm
            COMMAND ${__PFS0_COMMAND}
            DEPENDS ${target}.elf ${target}.npdm ${target}.nso
            VERBATIM)

    if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        add_custom_target(${target}.nsp ALL SOURCES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}.nsp)
    else ()
        add_custom_target(${target}.nsp ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${target}.nsp)
    endif ()
endfunction()

function(add_nro_target target)
    set(__NRO_COMMAND
            ${ELF2NRO} $<TARGET_FILE:${target}.elf> ${CMAKE_CURRENT_BINARY_DIR}/${target}.nro --nacp=${CMAKE_CURRENT_BINARY_DIR}/${target}.nacp --icon=${APP_ICON})

    if (NOT ${CMAKE_CURRENT_BINARY_DIR}/${target}.nacp)
        add_nacp(${target}.nacp)
    endif ()

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.nro
            COMMAND ${__NRO_COMMAND}
            DEPENDS ${target}.elf ${CMAKE_CURRENT_BINARY_DIR}/${target}.nacp
            VERBATIM)

    if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        add_custom_target(${target}.nro ALL SOURCES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${target}.nro)
    else ()
        add_custom_target(${target}.nro ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${target}.nro)
    endif ()
endfunction()

function(build_switch_binaries target)
    get_filename_component(target_we ${target} NAME_WE)

    if (NOT APP_TITLE)
        if (${ARGC} GREATER 1)
            set(APP_TITLE ${ARGV1})
        else ()
            set(APP_TITLE ${target_we})
        endif ()
    endif ()

    if (NOT APP_AUTHOR)
        if (${ARGC} GREATER 2)
            set(APP_AUTHOR ${ARGV2})
        else ()
            set(APP_AUTHOR "Unspecified Author")
        endif ()
    endif ()

    if (NOT APP_ICON)
        if (${ARGC} GREATER 4)
            set(APP_ICON ${ARGV4})
        else ()
            acquire_homebrew_icon(${target_we})
        endif ()
    endif ()

    if (NOT APP_VERSION)
        if (${ARGC} GREATER 3)
            set(APP_VERSION ${ARGV3})
        else ()
            set(APP_VERSION "1.0.0")
        endif ()
    endif ()

    # Build the binaries
    add_nso_target(${target_we})
    add_nro_target(${target_we})
endfunction()

function (build_switch_nsp target)
    get_filename_component(target_we ${target} NAME_WE)

    if (NOT APP_JSON)
        acquire_app_json(${target_we})
    endif()

    add_nso_target(${target_we})
    add_npdm_target(${target_we})
    add_nsp_target(${target_we})
endfunction()

function (build_switch_sysmodule target)
    get_filename_component(target_we ${target} NAME_WE)

    if (NOT APP_JSON)
        acquire_app_json(${target_we})
    endif()

    add_kip_target(${target_we})
endfunction()

