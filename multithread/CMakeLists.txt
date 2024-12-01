#========================================================================= 
set(test_condition_variable
    test_condition_variable.h
)

add_executable(test_condition_variable ${test_condition_variable})
set_source_files_properties(test_condition_variable.h PROPERTIES LANGUAGE CXX)
#========================================================================= 
set(3way_async
    3way_async.h
)

add_executable(3way_async ${3way_async})
set_source_files_properties(3way_async.h PROPERTIES LANGUAGE CXX)
#========================================================================= 
set(shared_future
    shared_future.h
)

add_executable(shared_future ${shared_future})
set_source_files_properties(shared_future.h PROPERTIES LANGUAGE CXX)