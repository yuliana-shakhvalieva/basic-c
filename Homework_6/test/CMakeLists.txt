set(TEST_SRC_LIST test_lazy_string.cpp)

add_executable(${PROJECT_NAME}_tests ${TEST_SRC_LIST})
target_link_libraries(${PROJECT_NAME}_tests
    PRIVATE
        ${PROJECT_NAME}_lib
        Catch2::Catch2WithMain
)
target_include_directories(${PROJECT_NAME}_tests
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src"
)

catch_discover_tests(${PROJECT_NAME}_tests)
