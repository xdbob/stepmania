file(MAKE_DIRECTORY "${SM_SRC_DIR}/generated")

if(MSVC)
  # Add the mapconv program of the past to allow for debugging information.
  include("CMakeProject-mapconv.cmake")
  if (WITH_IRC_POST_HOOK)
    include("CMakeProject-irc.cmake")
  endif()
  if (WITH_TEXTURE_GENERATOR OR WITH_FULL_RELEASE)
    include("CMakeProject-texture.cmake")
  endif()
endif()

# Keep the module path local for easier grabbing.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Main project is below.
if(UNIX OR LINUX)
include(GNUInstallDirs)
endif()

include(CMakeData-arch.cmake)
include(CMakeData-rage.cmake)
include(CMakeData-os.cmake)
include(CMakeData-actor.cmake)
include(CMakeData-screen.cmake)
include(CMakeData-data.cmake)
include(CMakeData-gtk.cmake)
include(CMakeData-file-types.cmake)
include(CMakeData-globals.cmake)
include(CMakeData-singletons.cmake)

list(APPEND SMDATA_ALL_FILES_SRC
  ${SMDATA_GLOBAL_FILES_SRC}
  ${SMDATA_GLOBAL_SINGLETON_SRC}
  ${SMDATA_ALL_ACTORS_SRC}
  ${SMDATA_ALL_ARCH_SRC}
  ${SMDATA_ALL_DATA_SRC}
  ${SMDATA_ALL_RAGE_SRC}
  ${SMDATA_ALL_SCREENS_SRC}
  ${SMDATA_OS_SRC}
  ${SMDATA_FILE_TYPES_SRC}
)
list(APPEND SMDATA_ALL_FILES_HPP
  ${SMDATA_GLOBAL_FILES_HPP}
  ${SMDATA_GLOBAL_SINGLETON_HPP}
  ${SMDATA_ALL_ACTORS_HPP}
  ${SMDATA_ALL_ARCH_HPP}
  ${SMDATA_ALL_DATA_HPP}
  ${SMDATA_ALL_RAGE_HPP}
  ${SMDATA_ALL_SCREENS_HPP}
  ${SMDATA_OS_HPP}
  ${SMDATA_FILE_TYPES_HPP}
)

if(NOT APPLE)
  list(APPEND SMDATA_ALL_FILES_SRC "Main.cpp")
  source_group("" FILES "Main.cpp")
endif()

if(MSVC OR APPLE)
  set(SM_NAME_RELEASE "StepMania")
  set(SM_NAME_DEBUG "StepMania-debug")
  set(SM_NAME_MINSIZEREL "StepMania-min-size")
  set(SM_NAME_RELWITHDEBINFO "StepMania-release-symbols")
else()
  set(SM_NAME_RELEASE "stepmania")
  set(SM_NAME_DEBUG "stepmania-debug")
  set(SM_NAME_MINSIZEREL "stepmania-min-size")
  set(SM_NAME_RELWITHDEBINFO "stepmania-release-symbols")
endif()

# Configure generated files here.
configure_file("${SM_XCODE_DIR}/Info.plist.in.xml" "${SM_XCODE_DIR}/Info.StepMania.plist")
configure_file("${SM_XCODE_DIR}/plistHelper.in.hpp" "${SM_XCODE_DIR}/plistHelper.hpp")

# TODO: Make this actually be data and not an executable.
if(APPLE)
  set(MACOSX_BUNDLE_BUNDLE_NAME ${SM_EXE_NAME})
  add_executable("${SM_EXE_NAME}" MACOSX_BUNDLE ${SMDATA_ALL_FILES_SRC} ${SMDATA_ALL_FILES_HPP})
  set_target_properties("${SM_EXE_NAME}" PROPERTIES
    MACOSX_BUNDLE TRUE
  )
else()
  if (MSVC)
    foreach ( sm_src_file ${SMDATA_ALL_FILES_SRC} )
      get_filename_component(sm_src_raw "${sm_src_file}" NAME_WE)
      if( ${sm_src_raw} MATCHES "global" )
        set_source_files_properties("${sm_src_raw}.cpp" PROPERTIES COMPILE_FLAGS "/Ycglobal.h")
      elseif(NOT (${sm_src_raw} MATCHES "verstub"))
        set_source_files_properties("${sm_src_raw}.cpp" PROPERTIES COMPILE_FLAGS "/Yuglobal.h")
      endif()
    endforeach()
  endif()
  add_executable("${SM_EXE_NAME}" ${SMDATA_ALL_FILES_SRC} ${SMDATA_ALL_FILES_HPP})
endif()

set_target_properties("${SM_EXE_NAME}" PROPERTIES
  OUTPUT_NAME "${SM_NAME_RELEASE}"
  RELEASE_OUTPUT_NAME "${SM_NAME_RELEASE}"
  DEBUG_OUTPUT_NAME "${SM_NAME_DEBUG}"
  MINSIZEREL_OUTPUT_NAME "${SM_NAME_MINSIZEREL}"
  RELWITHDEBINFO_OUTPUT_NAME "${SM_NAME_RELWITHDEBINFO}"
)

if (WITH_PORTABLE_TOMCRYPT)
  sm_add_compile_definition("${SM_EXE_NAME}" LTC_NO_ASM)
elseif (WITH_NO_ROLC_TOMCRYPT OR APPLE)
  sm_add_compile_definition("${SM_EXE_NAME}" LTC_NO_ROLC)
endif()

sm_add_compile_definition("${SM_EXE_NAME}" CMAKE_POWERED)

if (NOT WITH_NETWORKING)
  sm_add_compile_definition("${SM_EXE_NAME}" WITHOUT_NETWORKING)
endif()

# Compilation flags per project here.
sm_add_compile_definition("${SM_EXE_NAME}" $<$<CONFIG:Debug>:DEBUG>)
sm_add_compile_definition("${SM_EXE_NAME}" $<$<CONFIG:Release>:RELEASE>)
sm_add_compile_definition("${SM_EXE_NAME}" $<$<CONFIG:MinSizeRel>:MINSIZEREL>)
sm_add_compile_definition("${SM_EXE_NAME}" $<$<CONFIG:RelWithDebInfo>:RELWITHDEBINFO>)

set(SM_COMPILE_FLAGS "")

if(WITH_SSE2)
  if(MSVC)
    set(SM_COMPILE_FLAGS "${SM_COMPILE_FLAGS} /arch:SSE2")
  else()
    set(SM_COMPILE_FLAGS "${SM_COMPILE_FLAGS} -msse2")
  endif()
endif()

if(WITH_LTO)
  if(MSVC)
    set(SM_COMPILE_FLAGS "${SM_COMPILE_FLAGS} /GL")
  else()
    set(SM_COMPILE_FLAGS "${SM_COMPILE_FLAGS} -flto")
  endif()
endif()

if(MSVC)
  # TODO: Find a way to do this cleanly for non MSVC users.
  set(SM_COMPILE_FLAGS "${SM_COMPILE_FLAGS} /MP2")
endif()

set_target_properties("${SM_EXE_NAME}" PROPERTIES
  COMPILE_FLAGS "${SM_COMPILE_FLAGS}"
)

set_target_properties("${SM_EXE_NAME}" PROPERTIES
  OUTPUT_NAME_DEBUG "${SM_NAME_DEBUG}"
  OUTPUT_NAME_MINSIZEREL "${SM_NAME_MINSIZEREL}"
  OUTPUT_NAME_RELWITHDEBINFO "${SM_NAME_RELWITHDEBINFO}"
)

list(APPEND SM_WINDOWS_PROGRAM_DLLS
  "${SM_PROGRAM_DIR}/avcodec-55.dll"
  "${SM_PROGRAM_DIR}/avformat-55.dll"
  "${SM_PROGRAM_DIR}/avutil-52.dll"
  "${SM_PROGRAM_DIR}/parallel_lights_io.dll"
  "${SM_PROGRAM_DIR}/swscale-2.dll"
)

if(WIN32)
  sm_add_compile_definition("${SM_EXE_NAME}" WINDOWS)
  sm_add_compile_definition("${SM_EXE_NAME}" _WINDOWS) # TODO: Remove this potential duplicate.
  sm_add_compile_definition("${SM_EXE_NAME}" _CRT_SECURE_NO_WARNINGS)
  sm_add_compile_definition("${SM_EXE_NAME}" _WINSOCK_DEPRECATED_NO_WARNINGS)
  sm_add_compile_definition("${SM_EXE_NAME}" GLEW_STATIC)
  
  set_target_properties("${SM_EXE_NAME}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SM_PROGRAM_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SM_PROGRAM_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SM_PROGRAM_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${SM_PROGRAM_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${SM_PROGRAM_DIR}"
  )
  
  if(MSVC)
    # Allow for getting a virtualdub stack trace.
    add_custom_command(TARGET "${SM_EXE_NAME}"
      POST_BUILD
      COMMAND "mapconv" "${SM_PROGRAM_DIR}/$<$<CONFIG:DEBUG>:${SM_NAME_DEBUG}>$<$<CONFIG:MINSIZEREL>:${SM_NAME_MINSIZEREL}>$<$<CONFIG:RELWITHDEBINFO>:${SM_NAME_RELWITHDEBINFO}>$<$<CONFIG:RELEASE>:${SM_NAME_RELEASE}>.map" "${SM_PROGRAM_DIR}/$<$<CONFIG:DEBUG>:${SM_NAME_DEBUG}>$<$<CONFIG:MINSIZEREL>:${SM_NAME_MINSIZEREL}>$<$<CONFIG:RELWITHDEBINFO>:${SM_NAME_RELWITHDEBINFO}>$<$<CONFIG:RELEASE>:${SM_NAME_RELEASE}>.vdi"
      COMMENT "Generating file to allow for easier stack traces."
    )
  endif()
elseif(APPLE)
  set_target_properties("${SM_EXE_NAME}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${SM_ROOT_DIR}"
    MACOSX_BUNDLE_INFO_PLIST "${SM_XCODE_DIR}/Info.StepMania.plist"
    XCODE_ATTRIBUTE_INFOPLIST_FILE "${SM_XCODE_DIR}/Info.StepMania.plist"
    XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS "YES"
    XCODE_ATTRIBUTE_INFOPLIST_PREPROCESSOR_DEFINITIONS[variant=Release] "RELEASE"
    XCODE_ATTRIBUTE_INFOPLIST_PREPROCESSOR_DEFINITIONS[variant=Debug] "DEBUG"
    XCODE_ATTRIBUTE_INFOPLIST_PREPROCESSOR_DEFINITIONS[variant=MinSizeRel] "MINSIZEREL"
    XCODE_ATTRIBUTE_INFOPLIST_PREPROCESSOR_DEFINITIONS[variant=RelWithDebInfo] "RELWITHDEBINFO"
    XCODE_ATTRIBUTE_INFOPLIST_PREFIX_HEADER "${SM_XCODE_DIR}/plistHelper.hpp"
    XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/archutils/Darwin/StepMania.pch"
    XCODE_ATTRIBUTE_LIBRARY_SEARCH_PATHS "${SM_XCODE_DIR}/Libraries"
  )

  sm_add_compile_definition("${SM_EXE_NAME}" _XOPEN_SOURCE)

  if (${HAS_FFMPEG})
    sm_add_compile_definition("${SM_EXE_NAME}" HAVE_FFMPEG)
  endif()

  if (CMAKE_BUILD_TYPE EQUAL "Debug")
    set(SM_APP_RELEASE_NAME "${SM_NAME_DEBUG}")
  elseif(CMAKE_BUILD_TYPE EQUAL "MinSizeRel")
    set(SM_APP_RELEASE_NAME "${SM_NAME_MINSIZEREL}")
  elseif(CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
    set(SM_APP_RELEASE_NAME "${SM_NAME_RELWITHDEBINFO}")
  else()
    set(SM_APP_RELEASE_NAME "${SM_NAME_RELEASE}")
  endif()
  if (WITH_FULL_RELEASE)
    string(CONCAT SM_NAME_VERSION "${SM_EXE_NAME}" "-" "${SM_VERSION_TRADITIONAL}")
    set(SM_DMG_VERSION "${SM_VERSION_TRADITIONAL}")
  else()
    string(CONCAT SM_NAME_VERSION "${SM_EXE_NAME}" "-" "${SM_VERSION_FULL}")
    set(SM_DMG_VERSION "${SM_VERSION_FULL}")
  endif()
  set(SM_DMG_RELEASE_NAME "${SM_NAME_VERSION}-mac.dmg")

  add_custom_target(dmg
    COMMAND
      pushd ${SM_ROOT_DIR}\;
      mkdir dmgtmpdir\;
      cd dmgtmpdir\;
      mkdir ${SM_NAME_VERSION}\;
      cd ${SM_NAME_VERSION}\;
      mkdir ${SM_NAME_VERSION}\;
      cd ${SM_NAME_VERSION}\;
      cp -r "${SM_ROOT_DIR}/Announcers" .\;
      cp -r "${SM_ROOT_DIR}/BackgroundEffects" .\;
      cp -r "${SM_ROOT_DIR}/BackgroundTransitions" .\;
      cp -r "${SM_ROOT_DIR}/BGAnimations" .\;
      cp -r "${SM_ROOT_DIR}/Characters" .\;
      cp -r "${SM_ROOT_DIR}/Courses" .\;
      cp -r "${SM_ROOT_DIR}/Data" .\;
      cp -r "${SM_ROOT_DIR}/Docs" .\;
      cp -r "${SM_ROOT_DIR}/Manual" .\;
      cp -r "${SM_ROOT_DIR}/NoteSkins" .\;
      cp -r "${SM_ROOT_DIR}/Scripts" .\;
      cp -r "${SM_ROOT_DIR}/Songs" .\;
      cp -r "${SM_ROOT_DIR}/Themes" .\;
      cp -r "${SM_ROOT_DIR}/${SM_APP_RELEASE_NAME}.app" .\;
      cd ${SM_ROOT_DIR}\;
      hdiutil create "${SM_DMG_RELEASE_NAME}"
        -volname "StepMania ${SM_DMG_VERSION}"
        -srcfolder "dmgtmpdir/${SM_NAME_VERSION}/"
        -ov\;
      rm -rf dmgtmpdir\;
      popd\;
  )

  # Add the ability to copy the resource file.
  add_custom_command(TARGET "${SM_EXE_NAME}"
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:StepMania>/../Resources"
    COMMAND ${CMAKE_COMMAND} -E copy "${SM_XCODE_DIR}/smicon.icns" "$<TARGET_FILE_DIR:StepMania>/../Resources/"
    COMMAND ${CMAKE_COMMAND} -E copy "${SM_XCODE_DIR}/Hardware.plist" "$<TARGET_FILE_DIR:StepMania>/../Resources/"
  )
else() # Linux
  set_target_properties("${SM_EXE_NAME}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${SM_ROOT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${SM_ROOT_DIR}"
  )

  if(${WITH_CRASH_HANDLER})
    sm_add_compile_definition("${SM_EXE_NAME}" CRASH_HANDLER)
    if(LINUX)
      # used only in the ArchHooks_Unix.cpp file
      sm_add_compile_definition("${SM_EXE_NAME}" BACKTRACE_METHOD_X86_LINUX)
      sm_add_compile_definition("${SM_EXE_NAME}" BACKTRACE_METHOD_TEXT="x86 custom backtrace")
      sm_add_compile_definition("${SM_EXE_NAME}" BACKTRACE_LOOKUP_METHOD_TEXT="backtrace_symbols")
      if (${DL_FOUND})
        sm_add_compile_definition("${SM_EXE_NAME}" BACKTRACE_LOOKUP_METHOD_DLADDR)
      else()
        sm_add_compile_definition("${SM_EXE_NAME}" BACKTRACE_LOOKUP_METHOD_BACKTRACE_SYMBOLS)
      endif()
    endif()
  endif()
  if (${HAS_PTHREAD})
    sm_add_compile_definition("${SM_EXE_NAME}" HAVE_LIBPTHREAD)

    message("Host processor is ${CMAKE_SYSTEM_PROCESSOR}")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      message("Host processor is 64bit")
      sm_add_compile_definition("${SM_EXE_NAME}" CPU_X86_64)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
      message("Host processor is 32bit")
      sm_add_compile_definition("${SM_EXE_NAME}" CPU_X86)
    else()
      message("Unrecognized host processor type")
    endif()
  endif()
  if (${HAS_FFMPEG})
    sm_add_compile_definition("${SM_EXE_NAME}" HAVE_FFMPEG)
  endif()
  if (BSD)
    sm_add_compile_definition("${SM_EXE_NAME}" BSD)
  endif()

  if (GTK2_FOUND)
    sm_add_compile_definition("${SM_EXE_NAME}" HAVE_GTK)
  endif()

  sm_add_compile_definition("${SM_EXE_NAME}" UNIX)
  if("${CMAKE_SYSTEM}" MATCHES "Linux")
    sm_add_compile_definition("${SM_EXE_NAME}" LINUX)
  endif()
endif()

set_property(TARGET "${SM_EXE_NAME}" PROPERTY FOLDER "Internal Libraries")

list(APPEND SMDATA_LINK_LIB
  "lua-5.1"
  "tomcrypt"
  "tommath"
  "zlib"
  "jsoncpp"
  "png"
  "GLEW"
  "jpeg"
)
include(../extern/CMakeProject-mmmagic.cmake)

if (HAS_MP3)
  list(FIND SMDATA_LINK_LIB "jsoncpp" JSON_INDEX)
  if (WIN32 OR MACOSX)
    list(INSERT SMDATA_LINK_LIB ${JSON_INDEX} "mad")
  else()
    list(INSERT SMDATA_LINK_LIB ${JSON_INDEX} ${LIBMAD_LIBRARY})
  endif()
endif()

if (NOT SYSTEM_PCRE_FOUND)
  list(APPEND SMDATA_LINK_LIB "pcre")
endif()

if (WIN32)
  
  list(APPEND SMDATA_LINK_LIB
    # The misc libraries are here.
    "${LIB_SWSCALE}"
    "${LIB_AVCODEC}"
    "${LIB_AVFORMAT}"
    "${LIB_AVUTIL}"
  )
  
  if (WITH_OGG)
    list(APPEND SMDATA_LINK_LIB
      "ogg"
      "vorbis"
      "vorbisfile"
    )
  endif()
  
  list(APPEND SMDATA_LINK_LIB
    "dbghelp.lib"
    "setupapi.lib"
    "hid.lib"
  )
  
  get_filename_component(DIRECTX_LIBRARY_DIR "${DIRECTX_LIBRARIES}" DIRECTORY)
  
  sm_add_link_flag("${SM_EXE_NAME}" "/LIBPATH:\"${DIRECTX_LIBRARY_DIR}\"")
  sm_add_link_flag("${SM_EXE_NAME}" "/LIBPATH:\"${SM_EXTERN_DIR}/ffmpeg/lib\"")
  sm_add_link_flag("${SM_EXE_NAME}" "/LIBPATH:\"${SM_SRC_DIR}/archutils/Win32/ddk\"")
  sm_add_link_flag("${SM_EXE_NAME}" "/ERRORREPORT:SEND")
  sm_add_link_flag("${SM_EXE_NAME}" "/MAPINFO:EXPORTS")
  sm_add_link_flag("${SM_EXE_NAME}" "/SAFESEH:NO")
  sm_add_link_flag("${SM_EXE_NAME}" "/NOLOGO")
  sm_add_link_flag("${SM_EXE_NAME}" "/MAP")
  sm_add_link_flag("${SM_EXE_NAME}" "/NODEFAULTLIB:wininet.lib")
  sm_add_link_flag("${SM_EXE_NAME}" "/NODEFAULTLIB:msimg32.lib")
  sm_add_link_flag("${SM_EXE_NAME}" "/NODEFAULTLIB:libci.lib")
  set_target_properties("${SM_EXE_NAME}" PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt.lib")
  set_target_properties("${SM_EXE_NAME}" PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  set_target_properties("${SM_EXE_NAME}" PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
  
elseif(APPLE)
  # The following were removed from SMDATA_LINK_LIB to "match" StepMania's pbxproj.
#    ${MAC_FRAME_APPKIT}
#    ${MAC_FRAME_FOUNDATION}
#    "ffmpeg"

  list(INSERT SMDATA_LINK_LIB 0
    ${MAC_FRAME_ACCELERATE}
    ${MAC_FRAME_CARBON}
    ${MAC_FRAME_COCOA}
    ${MAC_FRAME_IOKIT}
    ${MAC_FRAME_QUICKTIME}
    ${MAC_FRAME_OPENGL}
    ${MAC_FRAME_COREFOUNDATION}
    ${MAC_FRAME_AUDIOTOOLBOX}
    ${MAC_FRAME_AUDIOUNIT}
    ${MAC_FRAME_COREAUDIO}
    ${MAC_FRAME_CORESERVICES}
    "ogg"
    "theora"
    "vorbis"
  )
  list(APPEND SMDATA_LINK_LIB
    "${BZIP2_LIBRARY_RELEASE}"
    "${ICONV_LIBRARIES}"
    "pcre"
  )

  if(HAS_FFMPEG)
    list(APPEND SMDATA_LINK_LIB
      "${SM_FFMPEG_ROOT}/libavformat/libavformat.a"
      "${SM_FFMPEG_ROOT}/libavcodec/libavcodec.a"
      "${SM_FFMPEG_ROOT}/libswscale/libswscale.a"
      "${SM_FFMPEG_ROOT}/libavutil/libavutil.a"
    )
  endif()
else() # Unix / Linux
  # TODO: Remember to find and locate the zip archive files.
  list(FIND SMDATA_LINK_LIB "jpeg" JPEG_INDEX)
  sm_list_replace(SMDATA_LINK_LIB ${JPEG_INDEX} "${JPEG_LIBRARY}")
  if (HAS_FFMPEG)
    if(WITH_SYSTEM_FFMPEG)
      list(APPEND SMDATA_LINK_LIB
        "${FFMPEG_avformat_LIBRARY}"
        "${FFMPEG_avcodec_LIBRARY}"
        "${FFMPEG_swscale_LIBRARY}"
        "${FFMPEG_avutil_LIBRARY}"
      )
    else()
      list(APPEND SMDATA_LINK_LIB
        "${SM_FFMPEG_ROOT}/libavformat/libavformat.a"
        "${SM_FFMPEG_ROOT}/libavcodec/libavcodec.a"
        "${SM_FFMPEG_ROOT}/libswscale/libswscale.a"
        "${SM_FFMPEG_ROOT}/libavutil/libavutil.a"
      )
    endif()

    if(VA_FOUND)
      list(APPEND SMDATA_LINK_LIB ${VA_LIBRARY})
    endif()
  endif()

  if (${DL_FOUND})
    list(APPEND SMDATA_LINK_LIB ${DL_LIBRARIES})
  endif()

  if(HAS_OGG)
    list(APPEND SMDATA_LINK_LIB
      "${VORBISFILE_LIBRARY}"
      "${VORBIS_LIBRARY}"
      "${OGG_LIBRARY}"
    )
  endif()

  list(APPEND SMDATA_LINK_LIB
    "${OPENGL_LIBRARY}"
  )

  if(GTK2_FOUND)
    list(APPEND SMDATA_LINK_LIB
      "${GTK2_LIBRARIES}"
    )
  endif()

  list(APPEND SMDATA_LINK_LIB
    "${BZIP2_LIBRARIES}"
    "${ZLIB_LIBRARIES}"
    "${CMAKE_THREAD_LIBS_INIT}"
  )

  if (HAS_ALSA)
    list(APPEND SMDATA_LINK_LIB ${ALSA_LIBRARIES})
  endif()

  if (HAS_JACK)
    list(APPEND SMDATA_LINK_LIB ${JACK_LIBRARIES})
  endif()

  if (HAS_OSS)
    # No mention of OSS libraries.
    sm_add_compile_definition("${SM_EXE_NAME}" "HAVE_OSS_VERSION=1")
  endif()

  if (HAS_PULSE)
    list(APPEND SMDATA_LINK_LIB ${PULSEAUDIO_LIBRARY})
    # PACKAGE_NAME and PACKAGE_VERSION are only used in this scenario. Why is not clear.
    # TODO: Remove this silliness.
    sm_add_compile_definition("${SM_EXE_NAME}" PACKAGE_NAME="StepMania")
    set(PACKAGE_VERSION "${SM_VERSION_MAJOR}.${SM_VERSION_MINOR}")
    sm_add_compile_definition("${SM_EXE_NAME}" PACKAGE_VERSION="${PACKAGE_VERSION}")
  endif()

  if(X11_FOUND)
    list(APPEND SMDATA_LINK_LIB ${X11_LIBRARIES})
  endif()

  if(PCRE_FOUND)
    list(APPEND SMDATA_LINK_LIB ${PCRE_LIBRARY})
  endif()

  list(APPEND SMDATA_LINK_LIB
    ${XRANDR_LIBRARIES}
  )

  list(REMOVE_ITEM SMDATA_LINK_LIB "zlib")
endif()

target_link_libraries("${SM_EXE_NAME}" ${SMDATA_LINK_LIB})
target_include_directories("${SM_EXE_NAME}" PRIVATE ${CMAKE_BINARY_DIR})

list(APPEND SM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  "${SM_SRC_DIR}/generated"
)
if(NOT APPLE)
  if(MSVC)
  else()
    list(APPEND SM_INCLUDE_DIRS
      "${JPEG_INCLUDE_DIR}"
    )
    if (HAS_FFMPEG)
      if (WITH_SYSTEM_FFMPEG)
        list(APPEND SM_INCLUDE_DIRS "${FFMPEG_INCLUDE_DIR}")
      else()
        add_dependencies("${SM_EXE_NAME}" "ffmpeg")
        list(APPEND SM_INCLUDE_DIRS
          "${SM_FFMPEG_SRC_DIR}"
          "${SM_FFMPEG_ROOT}"
        )
      endif()
    endif()
    if (HAS_GTK2)
      list(APPEND SM_INCLUDE_DIRS "${GTK2_INCLUDE_DIRS}")
    endif()
    if (X11_FOUND)
      list(APPEND SM_INCLUDE_DIRS "${X11_INCLUDE_DIR}")
    endif()
    if (PCRE_FOUND)
      list(APPEND SM_INCLUDE_DIRS "${PCRE_INCLUDE_DIR}")
    endif()
    if (DL_FOUND)
      list(APPEND SM_INCLUDE_DIRS "${DL_INCLUDE_DIR}")
    endif()
  endif()
else()
  list(APPEND SM_INCLUDE_DIRS
    "archutils/Unix"
  )
  if(HAS_FFMPEG)
    add_dependencies("${SM_EXE_NAME}" "ffmpeg")
    list(APPEND SM_INCLUDE_DIRS
      "${SM_FFMPEG_SRC_DIR}"
      "${SM_FFMPEG_ROOT}"
    )
  endif()
endif()

target_include_directories("${SM_EXE_NAME}" PUBLIC ${SM_INCLUDE_DIRS})

if (NOT DEFINED SM_INSTALL_DESTINATION)
  if(WIN32)
    set(SM_INSTALL_DESTINATION "." CACHE PATH "Installation directory")
  elseif(APPLE)
    set(SM_INSTALL_DESTINATION "stepmania-5.0" CACHE PATH "Installation directory")
  else()
    set(SM_INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/stepmania" CACHE PATH "Installation directory")
  endif()
endif()

if (NOT DEFINED SM_LIBDIR_DESTINATION)
  if(UNIX OR LINUX)
    set(SM_LIBDIR_DESTINATION "${CMAKE_INSTALL_LIBDIR}/stepmania" CACHE PATH "Libraries directory")
  else()
    set(SM_LIBDIR_DESTINATION "${SM_INSTALL_DESTINATION}" CACHE PATH "Libraries directory")
  endif()
endif()

configure_file(config.in.h "${CMAKE_BINARY_DIR}/config.h" @ONLY)

if(NOT APPLE)
  if(WIN32)
    set(SM_FULL_INSTALLATION_PATH_LIST "${SM_INSTALL_DESTINATION}" "Program")
    sm_join("${SM_FULL_INSTALLATION_PATH_LIST}" "/" SM_FULL_INSTALLATION_PATH)
    # Hardcoding the values for now since the foreach loop is not working as intended.
    install(TARGETS "${SM_EXE_NAME}" DESTINATION "${SM_FULL_INSTALLATION_PATH}")
    if (WITH_FULL_RELEASE OR WITH_TEXTURE_GENERATOR)
      install(FILES "${SM_PROGRAM_DIR}/Texture Font Generator.exe" DESTINATION "${SM_FULL_INSTALLATION_PATH}")
    endif()
    install(FILES "${SM_PROGRAM_DIR}/avcodec-55.dll" DESTINATION "${SM_FULL_INSTALLATION_PATH}")
    install(FILES "${SM_PROGRAM_DIR}/avformat-55.dll" DESTINATION "${SM_FULL_INSTALLATION_PATH}")
    install(FILES "${SM_PROGRAM_DIR}/avutil-52.dll" DESTINATION "${SM_FULL_INSTALLATION_PATH}")
    install(FILES "${SM_PROGRAM_DIR}/parallel_lights_io.dll" DESTINATION "${SM_FULL_INSTALLATION_PATH}")
    install(FILES "${SM_PROGRAM_DIR}/swscale-2.dll" DESTINATION "${SM_FULL_INSTALLATION_PATH}")
    install(FILES "${SM_PROGRAM_DIR}/StepMania.vdi" DESTINATION "${SM_FULL_INSTALLATION_PATH}")

    # foreach(SM_WINDOW_DLL "${SM_WINDOWS_PROGRAM_DLLS}")
    #   install(FILES "${SM_WINDOW_DLL}" DESTINATION "${SM_INSTALL_DESTINATION}")
    # endforeach()
  else()
    install(TARGETS "${SM_EXE_NAME}" DESTINATION "${CMAKE_INSTALL_BINDIR}")
  endif()
  if (UNIX OR LINUX)
    install(FILES "${SM_ROOT_DIR}/GtkModule.so" LIBRARY DESTINATION "${SM_LIBDIR_DESTINATION}" OPTIONAL)
  endif()
  install(DIRECTORY "${SM_ROOT_DIR}/Announcers" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/BGAnimations" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/Themes" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/Characters" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/Scripts" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/Courses" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/BackgroundEffects" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/Data" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/BackgroundTransitions" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/Docs" DESTINATION "${SM_INSTALL_DESTINATION}")
  install(DIRECTORY "${SM_ROOT_DIR}/NoteSkins" DESTINATION "${SM_INSTALL_DESTINATION}")
  if (NOT SM_INSTALL_DESTINATION)
    install(DIRECTORY "${SM_ROOT_DIR}/Songs/StepMania 5" DESTINATION "Songs")
    install(FILES "${SM_ROOT_DIR}/Songs/instructions.txt" DESTINATION "Songs")
  else()
    install(DIRECTORY "${SM_ROOT_DIR}/Songs/StepMania 5" DESTINATION "${SM_INSTALL_DESTINATION}/Songs")
    install(FILES "${SM_ROOT_DIR}/Songs/instructions.txt" DESTINATION "${SM_INSTALL_DESTINATION}/Songs")
  endif()
endif()

