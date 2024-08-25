#========================================================================= 
set(timer_sync
    timer_sync.cpp
)

add_executable(timer_sync ${timer_sync})
if(WIN32)
target_link_libraries(timer_sync PRIVATE ws2_32 mswsock)
else()
find_package(Threads REQUIRED)
target_link_libraries(timer_sync PRIVATE Threads::Threads)
endif()

#========================================================================= 
set(timer_async
    timer_async.cpp
)

add_executable(timer_async ${timer_async})
if(WIN32)
    target_link_libraries(timer_async PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(timer_async PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(tcp_client_sync
tcp_client_sync.cpp
)

add_executable(tcp_client_sync ${tcp_client_sync})
if(WIN32)
    target_link_libraries(tcp_client_sync PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(tcp_client_sync PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(tcp_server_sync
    tcp_server_sync.cpp
)

add_executable(tcp_server_sync ${tcp_server_sync})
if(WIN32)
    target_link_libraries(tcp_server_sync PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(tcp_server_sync PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(tcp_async
    tcp_async.cpp
)

add_executable(tcp_async ${tcp_async})
if(WIN32)
    target_link_libraries(tcp_async PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(tcp_async PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(udp_client_sync
    udp_client_sync.cpp
)

add_executable(udp_client_sync ${udp_client_sync})
if(WIN32)
    target_link_libraries(udp_client_sync PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(udp_client_sync PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(udp_server_sync
    udp_server_sync.cpp
)

add_executable(udp_server_sync ${udp_server_sync})
if(WIN32)
    target_link_libraries(udp_server_sync PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(udp_server_sync PRIVATE Threads::Threads)
endif()
#========================================================================= 