# set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SHARED_MODULE_PREFIX "fast_io_i18n.locale.")
file(GLOB localefiles ${CMAKE_CURRENT_LIST_DIR}/i18n_data/locale/*.cc)

include(CheckCXXCompilerFlag)

function(compilelocaleencoding encoding localelist)

  if(${encoding} STREQUAL "UTF-8")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
      set(compilerencodingtoggle "/utf-8")
      check_cxx_compiler_flag(
        ${compilerencodingtoggle} COMPILER_SUPPORT_EXECUTION_CHARSET_${encoding}
      )
    else()
      set(compilerencodingtoggle "")
      set(COMPILER_SUPPORT_EXECUTION_CHARSET_UTF-8 1)
    endif()
  else()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
      set(compilerencodingtoggle "/execution-charset:${encoding}")
    else()
      set(compilerencodingtoggle "-fexec-charset=${encoding}")
    endif()
    check_cxx_compiler_flag(
      ${compilerencodingtoggle} COMPILER_SUPPORT_EXECUTION_CHARSET_${encoding}
    )
  endif()

  if(I18N_INSTALL_DIR)

  else()
    set(I18N_INSTALL_DIR lib)
  endif()

  if(COMPILER_SUPPORT_EXECUTION_CHARSET_${encoding})
    if(localelist)
      foreach(filename ${localelist})
        string(REPLACE "@" "_" purfilename ${filename})
        add_library(
          ${purfilename}.${encoding} MODULE
          "${CMAKE_SOURCE_DIR}/src/i18n_data/locale/${localelist}.cc"
        )
        set_target_properties(${purfilename}.${encoding} PROPERTIES CXX_STANDARD 20)
        set_target_properties(
          ${purfilename}.${encoding} PROPERTIES OUTPUT_NAME ${filename}.${encoding}
        )
        if(NOT ${compilerencodingtoggle} STREQUAL "")
          target_compile_options(${purfilename}.${encoding} PRIVATE ${compilerencodingtoggle})
        endif()
        target_compile_definitions(
          ${purfilename}.${encoding}
          PUBLIC FAST_IO_LOCALE_ENCODING="${encoding}" FAST_IO_LOCALE_LENCODING=L"${encoding}"
                 FAST_IO_LOCALE_u8ENCODING=u8"${encoding}" FAST_IO_LOCALE_uENCODING=u"${encoding}"
                 FAST_IO_LOCALE_UENCODING=U"${encoding}"
        )
        install(TARGETS ${purfilename}.${encoding} LIBRARY DESTINATION ${I18N_INSTALL_DIR})
      endforeach()

    else()
      foreach(filepath ${localefiles})
        get_filename_component(filename ${filepath} NAME_WE)
        string(REPLACE "@" "_" purfilename ${filename})
        add_library(${purfilename}.${encoding} MODULE ${filepath})
        set_target_properties(
          ${purfilename}.${encoding} PROPERTIES OUTPUT_NAME ${filename}.${encoding}
        )
        if(NOT ${compilerencodingtoggle} STREQUAL "")
          target_compile_options(${purfilename}.${encoding} PRIVATE ${compilerencodingtoggle})
        endif()
        target_compile_definitions(
          ${purfilename}.${encoding}
          PUBLIC FAST_IO_LOCALE_ENCODING="${encoding}" FAST_IO_LOCALE_LENCODING=L"${encoding}"
                 FAST_IO_LOCALE_u8ENCODING=u8"${encoding}" FAST_IO_LOCALE_uENCODING=u"${encoding}"
                 FAST_IO_LOCALE_UENCODING=U"${encoding}"
        )
        # install(TARGETS ${purfilename}.${encoding} LIBRARY DESTINATION ${I18N_INSTALL_DIR})
        # install(EXPORT ${purfilename}.${encoding} )
        # install(
        #   EXPORT ${purfilename}.${encoding}
        #   DESTINATION "${${PROJECT_NAME}_INSTALL_CMAKEDIR}"
        #   NAMESPACE ${PROJECT_NAMESPACE}
        #   COMPONENT "${PROJECT_NAME}_Development"
        # )
      endforeach()
    endif()
  else()
    message(
      "Compiler does not support \"${compilerencodingtoggle}\"; locale under ${encoding} execution charset will not build."
    )
  endif()
endfunction()

compilelocaleencoding("UTF-8" false)
compilelocaleencoding("GB18030" false)
if(I18N_LOCALE_ENCODINGS)
  foreach(encoding ${I18N_LOCALE_ENCODINGS})
    if(NOT proj STREQUAL "UTF-8" AND NOT encoding STREQUAL "GB18030")
      if(I18N_LOCALE_LIST)
        compilelocaleencoding(${encoding} ${I18N_LOCALE_LIST})
      else()
        compilelocaleencoding(${encoding} false)
      endif()
    endif()
  endforeach()
else()

endif()
