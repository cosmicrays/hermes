add_library(kiss STATIC
    src/logger.cpp
    src/path.cpp
    src/string.cpp
    src/uuid.cpp
)

target_include_directories(kiss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(kiss PROPERTIES POSITION_INDEPENDENT_CODE ON)

# testing
if(ENABLE_TESTING)
    add_executable(test_uuid test/test_uuid.cpp)
    target_link_libraries(test_uuid PRIVATE kiss gtest_main Threads::Threads)
    add_test(NAME test_uuid COMMAND test_uuid)
endif()
