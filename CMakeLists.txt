cmake_minimum_required(VERSION 3.15)

project(fast_io C CXX)

include(GNUInstallDirs)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ TYPE INCLUDE)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/man/ DESTINATION ${CMAKE_INSTALL_MANDIR}/man3 PATTERN "*.3")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/fast_io/ DESTINATION ${CMAKE_INSTALL_DATADIR}/fast_io)

# fast_io_i18n — shared locale-blob cache library.
# src/locale/lcblob.cc owns the process-wide locale cache (thread-local
# first, then a mutex-guarded global map); mappings are never unloaded.
# Produces fast_io_i18n.dll on Windows, libfast_io_i18n.so elsewhere.
option(BUILD_I18N_LIB "build the fast_io_i18n locale library" OFF)

if(BUILD_I18N_LIB)
	add_library(fast_io_i18n SHARED src/locale/lcblob.cc)
	target_include_directories(fast_io_i18n PUBLIC ${CMAKE_SOURCE_DIR}/include)
	target_compile_definitions(fast_io_i18n PRIVATE FAST_IO_I18N_BUILDING)
	target_compile_definitions(fast_io_i18n PUBLIC FAST_IO_I18N_SHARED)
	# static twin — same output name as the shared lib where the
	# suffixes differ (libfast_io_i18n.{a,so}); on Windows the import
	# library already owns fast_io_i18n.lib so the static one is
	# fast_io_i18n_static.lib
	add_library(fast_io_i18n_static STATIC src/locale/lcblob.cc)
	target_include_directories(fast_io_i18n_static PUBLIC ${CMAKE_SOURCE_DIR}/include)
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin" OR CMAKE_SYSTEM_NAME STREQUAL "Msys")
		set_target_properties(fast_io_i18n_static PROPERTIES OUTPUT_NAME fast_io_i18n_static)
	else()
		set_target_properties(fast_io_i18n_static PROPERTIES OUTPUT_NAME fast_io_i18n)
	endif()
	install(TARGETS fast_io_i18n fast_io_i18n_static
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# locale tools — the localedef compiler in src/binfmt: fast_io_l10n_gen
# turns glibc localedata files into flat binary locale blobs,
# fast_io_l10n_dump inspects them.
option(BUILD_LOCALE_TOOLS "build locale blob tools (fast_io_l10n_gen, fast_io_l10n_dump)" OFF)

if(BUILD_LOCALE_TOOLS)
	add_executable(fast_io_l10n_gen src/binfmt/locale_emit.cc)
	add_executable(fast_io_l10n_dump src/binfmt/locale_dump.cc)
	foreach(tool fast_io_l10n_gen fast_io_l10n_dump)
		target_include_directories(${tool} PRIVATE ${CMAKE_SOURCE_DIR}/include)
		set_target_properties(${tool} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
		install(TARGETS ${tool} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	endforeach()
endif()

option(ENABLE_TESTS "Enable tests" OFF)

if(${ENABLE_TESTS})
	include(CTest)
	include_directories(include)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
	endif()
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin" OR CMAKE_SYSTEM_NAME STREQUAL "Msys")
		link_libraries(ntdll)
		link_libraries(psapi)
	endif()

	option(TESTS_PREBUILD "prebuild cmake files for test" OFF)

	if(${TESTS_PREBUILD})
		add_executable(tests_prebuild tests/0000.tests_prebuild/gentests.cc)
		add_custom_target(run_tests_prebuild
			COMMAND tests_prebuild
			DEPENDS tests_prebuild
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	else()
		include(${CMAKE_CURRENT_SOURCE_DIR}/benchmark/CMakeLists.txt)
		include(${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt)
		include(${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
	endif()
endif()
