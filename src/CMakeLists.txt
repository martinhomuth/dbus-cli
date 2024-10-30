add_executable(dbus-cli main.c test.c)
target_link_libraries(
  dbus-cli PRIVATE project_options project_warnings
  )

target_include_directories(dbus-cli PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_splint(dbus-cli main.c)
