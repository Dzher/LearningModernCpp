set(timer_sync
    timer_sync.cpp
)

set(timer_async
    timer_async.cpp
)

add_executable(timer_sync ${timer_sync})
if(WIN32)
    target_link_libraries(timer_sync PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(timer_sync PRIVATE Threads::Threads)
endif()

add_executable(timer_async ${timer_async})
if(WIN32)
    target_link_libraries(timer_async PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(timer_async PRIVATE Threads::Threads)
endif()