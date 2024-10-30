option(ENABLE_SPLINT "Enable static code analysis with splint" OFF)
if (ENABLE_SPLINT)
  find_program(SPLINT splint)
  if (SPLINT)
    set(SPLINT_FLAGS ${SPLINT_FLAGS} +posixlib -D__linux__)
    if(CMAKE_COMPILER_IS_GNUCC)
      set(SPLINT_FLAGS ${SPLINT_FLAGS} -D__GNUC__)
    endif()
  else()
    message(SEND_ERROR "splint requested but executable not found")
  endif()
endif()

function(add_splint TARGET)
  if (ENABLE_SPLINT)
    get_directory_property(include_dirs INCLUDE_DIRECTORIES)
    foreach(i ${include_dirs})
      list(APPEND include_flags -I${i})
    endforeach()
    add_custom_target(
      splint-${TARGET}
      COMMAND ${SPLINT} ${SPLINT_FLAGS} ${include_flags} ${ARGN} ${maybe_short_circuit_errors}
      DEPENDS ${ARGN}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
      )
    add_dependencies(${TARGET} splint-${TARGET})
  endif()
endFunction()

