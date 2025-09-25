add_subdirectory(unity)

function(add_vector_test test_name)
  add_executable(${test_name} EXCLUDE_FROM_ALL ${test_name}.c vector_generated.c)
  target_link_libraries(${test_name} PRIVATE unity)
endfunction()

add_vector_test(test_vector_out_of_mem)
add_vector_test(test_vector_pass_null_abort)
add_vector_test(test_vector_pass_null_ignore)
add_vector_test(test_vector_usual_behavior)

enable_testing()

add_test(NAME VectorOutOfMemory COMMAND test_vector_out_of_mem)
add_test(NAME VectorPassNullAbort COMMAND test_vector_pass_null_abort)
add_test(NAME VectorPassNullIgnore COMMAND test_vector_pass_null_ignore)
add_test(NAME VectorUsualBehavior COMMAND test_vector_usual_behavior)

add_custom_target(test
  DEPENDS test_vector_out_of_mem test_vector_pass_null_abort test_vector_pass_null_ignore test_vector_usual_behavior
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
  COMMAND ctest
)
