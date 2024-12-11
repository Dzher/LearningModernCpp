#=============================================================================== 
set(test_condition_variable
    test_condition_variable.h
)

add_executable(test_condition_variable ${test_condition_variable})
set_source_files_properties(test_condition_variable.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(3way_async
    3way_async.h
)

add_executable(3way_async ${3way_async})
set_source_files_properties(3way_async.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(shared_future
    shared_future.h
)

add_executable(shared_future ${shared_future})
set_source_files_properties(shared_future.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(csp_customer_productor
    csp_customer_productor.h
)

add_executable(csp_customer_productor ${csp_customer_productor})
set_source_files_properties(csp_customer_productor.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(spin_lock
    spin_lock.h
)

add_executable(spin_lock ${spin_lock})
set_source_files_properties(spin_lock.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(6_memory_orders
    6_memory_orders.h
)

add_executable(6_memory_orders ${6_memory_orders})
set_source_files_properties(6_memory_orders.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(thread_safe_singleton
    thread_safe_singleton.h
)

add_executable(thread_safe_singleton ${thread_safe_singleton})
set_source_files_properties(thread_safe_singleton.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(sequential_consistent
    sequential_consistent.h
)

add_executable(sequential_consistent ${sequential_consistent})
set_source_files_properties(sequential_consistent.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(acquire_release
    acquire_release.h
)

add_executable(acquire_release ${acquire_release})
set_source_files_properties(acquire_release.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(thread_safe_stack
    thread_safe_stack.h
)

add_executable(thread_safe_stack ${thread_safe_stack})
set_source_files_properties(thread_safe_stack.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(test_semaphore
    test_semaphore.h
)

add_executable(test_semaphore ${test_semaphore})
set_source_files_properties(test_semaphore.h PROPERTIES LANGUAGE CXX)
#===============================================================================
set(memory_fence
    memory_fence.h
)

add_executable(memory_fence ${memory_fence})
set_source_files_properties(memory_fence.h PROPERTIES LANGUAGE CXX)
#===============================================================================