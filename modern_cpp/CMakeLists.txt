#=============================================================================== 
set(if_have_func_foo
    if_have_func_foo.h
)

add_executable(if_have_func_foo ${if_have_func_foo})
set_source_files_properties(if_have_func_foo.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(functional_programming
    functional_programming.h
)

add_executable(functional_programming ${functional_programming})
set_source_files_properties(functional_programming.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(std_bind
    std_bind.h
)

add_executable(std_bind ${std_bind})
set_source_files_properties(std_bind.h PROPERTIES LANGUAGE CXX)
#===============================================================================