add_subdirectory(unity)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
enable_testing()

add_subdirectory(out_of_mem)
add_subdirectory(pass_null_abort)
add_subdirectory(pass_null_ignore)
add_subdirectory(usual_behavior)
add_subdirectory(no_crash_on_oob)

add_custom_target(test
  DEPENDS test_vector_out_of_mem test_vector_pass_null_abort test_vector_pass_null_ignore test_vector_usual_behavior test_vector_no_crash_on_oob
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
  COMMAND ctest
)
