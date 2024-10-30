
FetchContent_GetProperties(cmocka_extensions)

add_executable(run_tests
	testmain.c
	${PROJECT_SOURCE_DIR}/src/test.c
  )

target_link_libraries(run_tests PRIVATE cmocka_extensions)
target_include_directories(run_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${cmocka_extensions_SOURCE_DIR}/src/cmocka_extensions/include/cmocka_extensions
)

enable_testing()

add_test(NAME test COMMAND run_tests)


