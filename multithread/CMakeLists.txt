#========================================================================= 
set(test_condition_variable
    test_condition_variable.h
)

add_executable(test_condition_variable ${test_condition_variable})
set_source_files_properties(test_condition_variable.h PROPERTIES LANGUAGE CXX)