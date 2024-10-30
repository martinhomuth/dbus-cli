
FetchContent_Declare(
  cmocka_extensions
  GIT_REPOSITORY https://github.com/elektrobit/cmocka_extensions
  GIT_TAG cmocka_extensions-0.54.1
  )

FetchContent_MakeAvailable(cmocka_extensions)
message(STATUS "cmocka_extensions made available")

