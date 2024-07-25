# cmake-format: off
set(examples_0030.binary_serialization_usual
  leb128.cc
  leb128_input.cc
  lec559_lebe.cc
)

set(examples_0030.binary_serialization_wasm
  wasm.cc
)
# cmake-format: on

set(examples_to_compile ${examples_0030.binary_serialization_usual})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
  list(APPEND examples_to_compile ${examples_0030.binary_serialization_wasm})
endif()

apply_to_sources_examples(examples_to_compile create_exe_default_example)
