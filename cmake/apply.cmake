function(apply_for_every_compilation_unit_in_subdir source_dir relative_dir subdir prefix command)
  message(
    DEBUG
    "apply_for_every_compilation_unit_in_subdir was called with next args: source_dir: ${source_dir}  relative_dir: ${relative_dir} subdir: ${subdir} prefix: ${prefix} command: ${command}
  "
  )
  file(
    GLOB_RECURSE sources CONFIGURE_DEPENDS
    RELATIVE "${source_dir}/${subdir}"
    "${relative_dir}/${subdir}/**.cc"
  )
  message(DEBUG "sources ${subdir}: ${sources}")
  foreach(source ${sources})
    string(REGEX REPLACE "/" "_" name ${source})
    string(REGEX REPLACE "(\.cc|\.cpp|\.cxx)" "" name ${name})
    message(DEBUG "Calling next: ${command} ${prefix}_${subdir}_${name} ${subdir}/${source}")
    cmake_language(CALL ${command} ${prefix}_${subdir}_${name} "${subdir}/${source}")
  endforeach()

endfunction()

function(apply_for_every_compilation_unit sources_list prefix command)
  foreach(source ${${sources_list}})
    message(
      DEBUG
      "apply_for_every_compilation_unit was called with next args: source_name: ${source} prefix: ${prefix} command: ${command}"
    )
    string(REGEX REPLACE "/" "_" name ${source})
    string(REGEX REPLACE "(\.cc|\.cpp|\.cxx)" "" name ${name})
    message(DEBUG "Calling next: ${command} ${prefix}_${name} ${source}")
    cmake_language(CALL ${command} ${prefix}_${name} "${source}")
  endforeach()
endfunction()
