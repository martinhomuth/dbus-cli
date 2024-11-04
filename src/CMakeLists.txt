set(SOURCE_FILES
  main.c
  terminal.c
  test.c
  util.c
)

add_executable(dbus-cli ${SOURCE_FILES})
target_link_libraries(
  dbus-cli PRIVATE project_options project_warnings
)

target_include_directories(dbus-cli PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_splint(dbus-cli main.c)
