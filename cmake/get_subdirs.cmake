function(SUBDIRLIST result_var curdir)
  file(
    GLOB children
    RELATIVE ${curdir}
    ${curdir}/*
  )
  set(result "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND result ${child})
    endif()
  endforeach()
  set(${result_var}
      ${result}
      PARENT_SCOPE
  )
endfunction()

