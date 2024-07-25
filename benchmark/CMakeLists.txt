# include(${CMAKE_CURRENT_LIST_DIR}/../cmake/get_subdirs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/apply.cmake)

function(apply_to_sources_benchmark sources_list_name command)
  apply_for_every_compilation_unit(${sources_list_name} benchmark_${PROJECT_NAME} ${command})
endfunction()

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/create_exe_default.cmake)

function(create_exe_default_benchmarks name source)
  cmake_language(CALL create_exe_default "BENCHMARKS" ${name} ${source})
endfunction()

function(link_qt_core name source)
  if(${QT5_FOUND})
    target_link_libraries(${name} PRIVATE Qt5::Core)
  else()
    target_link_libraries(${name} PRIVATE Qt6::Core)
  endif()
endfunction()

# cmake-format: off

# file(
#   GLOB_RECURSE files
#   RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
#   ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
# )
# foreach(file ${files})
#   message(DEBUG "Benchmarks *.cc: ${file}")
# endforeach()

set(simple_benchmarks
    # 0001.refterm/filebuf_file.cc
    0001.refterm/fpipe.cc
    # 0001.refterm/fstream.cc
    0001.refterm/iostream.cc
    0001.refterm/out_buf.cc
    0001.refterm/stdio.cc
    0001.refterm/stdio_hack.cc
    0002.file_io_tsc/1mtime/fstream.cc
    0002.file_io_tsc/1mtime/iobuf_file.cc
    0002.file_io_tsc/1mtime/stdio.cc
    0002.file_io_tsc/c_file_unlocked_tsc.cc
    0002.file_io_tsc/iobuf_file_tsc.cc
    0002.file_io_tsc/longspaces/fstream.cc
    0002.file_io_tsc/longspaces/iobuf_file.cc
    0002.file_io_tsc/longspaces/iobuf_file_verify.cc
    0002.file_io_tsc/longspaces/stdio.cc
    0002.file_io_tsc/longspaces/u32iobuf_file.cc
    0002.file_io_tsc/longspaces/u32utf8_file.cc
    0002.file_io_tsc/random/c_file.cc
    0002.file_io_tsc/random/filebuf_file.cc
    0002.file_io_tsc/random/iobuf_file.cc
    0002.file_io_tsc/random/stdio.cc
    0002.file_io_tsc/random/stream.cc
    0002.file_io_tsc/stdio_tsc.cc
    0002.file_io_tsc/time/c_file.cc
    0002.file_io_tsc/time/c_file_unlocked.cc
    0002.file_io_tsc/time/c_file_unlocked_text.cc
    0002.file_io_tsc/time/fast_io_llvm_raw_fd_ostream_file.cc
    0002.file_io_tsc/time/fast_io_llvm_raw_fd_ostream_file_l10n.cc
    0002.file_io_tsc/time/filebuf_file.cc
    0002.file_io_tsc/time/fstream.cc
    0002.file_io_tsc/time/fstream_std_format.cc
    0002.file_io_tsc/time/fstreamtext.cc
    0002.file_io_tsc/time/iobuf_file.cc
    0002.file_io_tsc/time/iobuf_file_be32.cc
    0002.file_io_tsc/time/iobuf_file_bin.cc
    0002.file_io_tsc/time/iobuf_file_concatln.cc
    0002.file_io_tsc/time/iobuf_file_l10n.cc
    0002.file_io_tsc/time/iobuf_file_le32.cc
    0002.file_io_tsc/time/iobuf_file_leb128.cc
    0002.file_io_tsc/time/iobuf_file_mutex.cc
    0002.file_io_tsc/time/iobuf_file_mutex_big.cc
    0002.file_io_tsc/time/iobuf_file_std_format.cc
    0002.file_io_tsc/time/iobuf_file_std_format_locale.cc
    0002.file_io_tsc/time/iobuf_file_unsafe_rt_fprint.cc
    0002.file_io_tsc/time/iobuf_file_unsafe_rt_fprint_l10n.cc
    0002.file_io_tsc/time/iobuf_io_file.cc
    0002.file_io_tsc/time/iobuf_io_file_eol.cc
    0002.file_io_tsc/time/iobuf_io_file_eol_dbg.cc
    0002.file_io_tsc/time/iobuf_io_file_lf_cr.cc
    0002.file_io_tsc/time/iobuf_io_file_lf_crlf.cc
    0002.file_io_tsc/time/llvm_raw_fd_ostream.cc
    0002.file_io_tsc/time/stdio.cc
    0002.file_io_tsc/time/stdiotext.cc
    0002.file_io_tsc/time/u32iobuf_file.cc
    0002.file_io_tsc/time/wiobuf_file.cc
    0003.file_io_hex/fstream.cc
    0003.file_io_hex/fstream_locale.cc
    0003.file_io_hex/iobuf_file.cc
    0003.file_io_hex/iobuf_file_addrvw.cc
    0003.file_io_hex/iobuf_file_l10n.cc
    0004.file_io_floating/charconv_general.cc
    0004.file_io_floating/charconv_general_float.cc
    0004.file_io_floating/charconv_hexfloat.cc
    0004.file_io_floating/charconv_hexfloat_float.cc
    0004.file_io_floating/decimal.cc
    0004.file_io_floating/decimal_float.cc
    0004.file_io_floating/decimal_l10n.cc
    0004.file_io_floating/general.cc
    0004.file_io_floating/general_float.cc
    0004.file_io_floating/general_l10n.cc
    0004.file_io_floating/hexfloat.cc
    0004.file_io_floating/hexfloat_float.cc
    0005.time/fast_io.cc
    # 0005.time/glibc_nl_langinfo.cc
    0005.time/iso8601.cc
    # 0005.time/put_time.cc
    0006.random/password/ibuf_white_hole_engine.cc
    0006.random/password/mt19937_64.cc
    0006.random/password/random_device.cc
    # 0007.concat/concat_cstring.cc
    # 0007.concat/concat_hstring.cc
    # 0007.concat/concat_qstring.cc
    0007.concat/concat_std.cc
    0007.concat/concat_string.cc
    # 0007.concat/concat_vs_format.cc
    # 0007.concat/to_hstring.cc
    0008/seperate/charconv.cc
    0008/seperate/charconv_no_c_str.cc
    # 0008/seperate/dragonbox.cc
    # 0008/seperate/dragonbox_no_c_str.cc
    0008/seperate/fast_io.cc
    0008/seperate/fast_io_direct.cc
    0008/seperate/fast_io_no_c_str.cc
    0008/seperate_short/charconv_no_c_str.cc
    # 0008/seperate_short/dragonbox_no_c_str.cc
    0008/seperate_short/fast_io_no_c_str.cc
    0009.concatstring/concat.cc
    0009.concatstring/format.cc
    0009.concatstring/stringstream.cc
    0009.concatstring/tostring.cc
    0010.line/contiguous_line_scanner.cc
    0010.line/getline.cc
    0010.line/line_get.cc
    0010.line/line_get_fbf.cc
    0010.line/line_scanner.cc
    0010.line/random_data.cc
    # 0010.line/scan_qstring.cc
    # 0010.line/scan_qstring_u16utf8.cc
    0010.line/scan_strlike.cc
    0010.line/scan_u16string_u16utf8.cc
    # 0010.line/scan_winrt_hstring.cc
    0011.containers/list/0000.algos/fast_io.cc
    0011.containers/list/0000.algos/std.cc
    0011.containers/list/0002.multi_push_back/fast_io.cc
    0011.containers/list/0002.multi_push_back/std.cc
    # 0011.containers/list/0002.multi_push_back/std_mimalloc.cc
    0011.containers/vector/0000.initialization/fast_io.cc
    0011.containers/vector/0000.initialization/std.cc
    0011.containers/vector/0001.push_back/fast_io.cc
    0011.containers/vector/0001.push_back/fast_io_unchecked.cc
    0011.containers/vector/0001.push_back/std.cc
    0011.containers/vector/0002.multi_push_back/fast_io.cc
    0011.containers/vector/0002.multi_push_back/std.cc
    # 0011.containers/vector/0002.multi_push_back/std_mimalloc.cc
    # 0012.isa/sha512.cc
    0013.bitset/filebuf_file.cc
    0013.bitset/ostream.cc
    0013.bitset/u8c_file.cc
    0013.bitset/u8c_file_unlocked.cc
    0013.bitset/u8obuf_file.cc
    0014.codecvt/u16utf8_file.cc
    0014.codecvt/u32utf8_file.cc
    0014.codecvt/u8iobuf_file.cc
    0015.string/istream.cc
    0015.string/random_data.cc
    0015.string/scan.cc
    0015.string/scan_fbf.cc
    # 0015.string/scan_qstring.cc
    # 0015.string/scan_qstring_u16utf8.cc
    0015.string/scan_strlike.cc
    0015.string/scan_u16string_u16utf8.cc
    # 0015.string/scan_winrt_hstring.cc
    0016.endl/filebuf_file.cc
    0016.endl/ofstream.cc
    0016.endl/ofstream_endl.cc
    0016.endl/u8c_file_unlocked.cc
    0016.endl/u8native_file.cc
    0016.endl/u8obuf_file.cc
    # 0017.syscalls/nt_file.cc
    # 0017.syscalls/posix_file.cc
    # 0017.syscalls/win32_file.cc
    0018.get_fd/cout.cc
    0018.get_fd/coutfp.cc
)

set(benchmarks_windows_only
    0001.refterm/filebuf_file.cc
    0001.refterm/fstream.cc
    0007.concat/concat_cstring.cc
    0007.concat/to_hstring.cc
    0010.line/scan_winrt_hstring.cc
    0015.string/scan_winrt_hstring.cc
    0017.syscalls/nt_file.cc
    0017.syscalls/win32_file.cc
)

set(benchmarks_linux_only
    0005.time/glibc_nl_langinfo.cc
    0005.time/put_time.cc
    0017.syscalls/posix_file.cc
)

set(benchmarks_qt_only
    0007.concat/concat_qstring.cc
    0010.line/scan_qstring.cc
    0010.line/scan_qstring_u16utf8.cc
    0015.string/scan_qstring.cc
    0015.string/scan_qstring_u16utf8.cc
)

set(benchmarks_dragonbox_only
    0008/seperate/dragonbox.cc
    0008/seperate/dragonbox_no_c_str.cc
    0008/seperate_short/dragonbox_no_c_str.cc
)

set(benchmarks_mimalloc_only
    0011.containers/list/0002.multi_push_back/std_mimalloc.cc
    0011.containers/vector/0002.multi_push_back/std_mimalloc.cc
)
# cmake-format: on

set(benchmarks_to_compile ${simple_benchmarks})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows"
   OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin"
   OR CMAKE_SYSTEM_NAME STREQUAL "Msys"
)
  list(APPEND benchmarks_to_compile ${benchmarks_windows_only})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND benchmarks_to_compile ${benchmarks_linux_only})
endif()

find_package(Qt5 COMPONENTS Core)
find_package(Qt6 COMPONENTS Core)
if(${QT5_FOUND} OR ${QT6_FOUND})
  list(APPEND benchmarks_to_compile ${benchmarks_qt_only})
endif()

list(APPEND benchmarks_to_compile ${benchmarks_dragonbox_only})
list(APPEND benchmarks_to_compile ${benchmarks_mimalloc_only})

apply_to_sources_benchmark(benchmarks_to_compile create_exe_default_benchmarks)

if(${QT5_FOUND} OR ${QT6_FOUND})
  apply_to_sources_benchmark(benchmarks_qt_only link_qt_core)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/packages/dragonbox/add_dragonbox.cmake)

function(link_dragonbox name source)
  target_link_libraries(${name} PRIVATE dragonbox::dragonbox)
endfunction()

apply_to_sources_benchmark(benchmarks_dragonbox_only link_dragonbox)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/packages/mimalloc/add_mimalloc.cmake)

function(link_mimalloc name source)
  target_link_libraries(${name} PRIVATE mimalloc_static)
endfunction()

apply_to_sources_benchmark(benchmarks_mimalloc_only link_mimalloc)
