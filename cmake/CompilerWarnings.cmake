function(set_project_warnings project_name)
  option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" TRUE)
  set(COMPILER_WARNINGS
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a
    # parent context
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Wpedantic # warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wnull-dereference # warn if a null dereference is detected
    -Wdouble-promotion # warn if float is implicit promoted to double
    -Wformat=2 # warn on security issues around functions that format output
    -fanalyzer
    # (ie printf)
    )

  if (WARNINGS_AS_ERRORS)
    set(COMPILER_WARNINGS ${COMPILER_WARNINGS} -Werror)
  endif()

  target_compile_options(${project_name} INTERFACE ${COMPILER_WARNINGS})
endfunction()

