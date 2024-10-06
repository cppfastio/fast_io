cmake_minimum_required(VERSION 3.15)

project(fast_io C CXX)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_I18N_DLLS "build i18n runtime dlls" OFF)
option(BUILD_TESTING "build tests" OFF)
option(GEN_TESTS "generate cmake files for test" OFF)

include(GNUInstallDirs)
include(CheckCXXCompilerFlag)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include TYPE INCLUDE)
install(FILES ${CMAKE_SOURCE_DIR}/man/*.3 DESTINATION ${CMAKE_INSTALL_MANDIR}/man3)

include_directories(include)

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif()

if(BUILD_I18N_DLLS)

set(CMAKE_SHARED_MODULE_PREFIX "fast_io_i18n.locale.")
file(GLOB localefiles ${CMAKE_SOURCE_DIR}/src/i18n_data/locale/*.cc)

function(compilelocaleencoding encoding localelist)

if(${encoding} STREQUAL "UTF-8")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(compilerencodingtoggle "/utf-8")
		CHECK_CXX_COMPILER_FLAG(${compilerencodingtoggle} COMPILER_SUPPORT_EXECUTION_CHARSET_${encoding})
	else()
		set(compilerencodingtoggle "")
		set(COMPILER_SUPPORT_EXECUTION_CHARSET_UTF-8 1)
	endif()
else()
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(compilerencodingtoggle "/execution-charset:${encoding}")
	else()
		set(compilerencodingtoggle "-fexec-charset=${encoding}")
	endif()
	CHECK_CXX_COMPILER_FLAG(${compilerencodingtoggle} COMPILER_SUPPORT_EXECUTION_CHARSET_${encoding})
endif()

if(I18N_INSTALL_DIR)
else()
	set(I18N_INSTALL_DIR lib)
endif()

if(COMPILER_SUPPORT_EXECUTION_CHARSET_${encoding})
	if(localelist)
		foreach(filename ${localelist})
			string(REPLACE "@" "_" purfilename ${filename})
			add_library(${purfilename}.${encoding} MODULE "${CMAKE_SOURCE_DIR}/src/i18n_data/locale/${localelist}.cc")
			set_target_properties(${purfilename}.${encoding} PROPERTIES OUTPUT_NAME ${filename}.${encoding})
			if (NOT ${compilerencodingtoggle} STREQUAL "")
				target_compile_options(${purfilename}.${encoding} PRIVATE ${compilerencodingtoggle})
			endif()
			target_compile_definitions(${purfilename}.${encoding} PUBLIC
				FAST_IO_LOCALE_ENCODING="${encoding}"
				FAST_IO_LOCALE_LENCODING=L"${encoding}"
				FAST_IO_LOCALE_u8ENCODING=u8"${encoding}"
				FAST_IO_LOCALE_uENCODING=u"${encoding}"
				FAST_IO_LOCALE_UENCODING=U"${encoding}")
			install(TARGETS ${purfilename}.${encoding}
				LIBRARY DESTINATION ${I18N_INSTALL_DIR})
		endforeach()

	else()
		foreach(filepath ${localefiles})
			get_filename_component(filename ${filepath} NAME_WE)
			string(REPLACE "@" "_" purfilename ${filename})
			add_library(${purfilename}.${encoding} MODULE ${filepath})
			set_target_properties(${purfilename}.${encoding} PROPERTIES OUTPUT_NAME ${filename}.${encoding})
			if (NOT ${compilerencodingtoggle} STREQUAL "")
				target_compile_options(${purfilename}.${encoding} PRIVATE ${compilerencodingtoggle})
			endif()
			target_compile_definitions(${purfilename}.${encoding} PUBLIC
				FAST_IO_LOCALE_ENCODING="${encoding}"
				FAST_IO_LOCALE_LENCODING=L"${encoding}"
				FAST_IO_LOCALE_u8ENCODING=u8"${encoding}"
				FAST_IO_LOCALE_uENCODING=u"${encoding}"
				FAST_IO_LOCALE_UENCODING=U"${encoding}")
			install(TARGETS ${purfilename}.${encoding}
				LIBRARY DESTINATION ${I18N_INSTALL_DIR})
		endforeach()
	endif()
else()
	message("Compiler does not support \"${compilerencodingtoggle}\"; locale under ${encoding} execution charset will not build.")
endif()
endfunction()

compilelocaleencoding("UTF-8" false)
compilelocaleencoding("GB18030" false)
if(I18N_LOCALE_ENCODINGS)
foreach(encoding ${I18N_LOCALE_ENCODINGS})
	if (NOT proj STREQUAL "UTF-8" AND NOT encoding STREQUAL "GB18030")
		if(I18N_LOCALE_LIST)
			compilelocaleencoding(${encoding} ${I18N_LOCALE_LIST})
		else()
			compilelocaleencoding(${encoding} false)
		endif()
	endif()
endforeach()
else()
endif()

endif()

if(${GEN_TESTS})
include_directories(include)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_EXTENSION OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin" OR CMAKE_SYSTEM_NAME STREQUAL "Msys")
link_libraries(ntdll)
endif()
add_executable(gentests gentests.cc)
add_custom_target(run_gentests
    COMMAND gentests
    DEPENDS gentests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
endif()

if(${BUILD_TESTING})
include(CTest)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_EXTENSION OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin" OR CMAKE_SYSTEM_NAME STREQUAL "Msys")
link_libraries(ntdll)
endif()
include(${CMAKE_CURRENT_SOURCE_DIR}/benchmark/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
endif()
