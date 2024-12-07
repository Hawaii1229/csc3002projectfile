find_package(Qt5 COMPONENTS Sql REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Sql) 