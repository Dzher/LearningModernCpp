#========================================================================= 
set(timer_sync
    timer/timer_sync.cpp
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
    timer/timer_async.cpp
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
    tcp/tcp_client_sync.cpp
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
    tcp/tcp_server_sync.cpp
)

add_executable(tcp_server_sync ${tcp_server_sync})
if(WIN32)
    target_link_libraries(tcp_server_sync PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(tcp_server_sync PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(tcp_server_async
    tcp/tcp_server_async.cpp
)

add_executable(tcp_server_async ${tcp_server_async})
if(WIN32)
    target_link_libraries(tcp_server_async PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(tcp_server_async PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(udp_client_sync
    udp/udp_client_sync.cpp
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
    udp/udp_server_sync.cpp
)

add_executable(udp_server_sync ${udp_server_sync})
if(WIN32)
    target_link_libraries(udp_server_sync PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(udp_server_sync PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(udp_server_async
    udp/udp_server_async.cpp
)

add_executable(udp_server_async ${udp_server_async})
if(WIN32)
    target_link_libraries(udp_server_async PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(udp_server_async PRIVATE Threads::Threads)
endif()
#========================================================================= 
set(tcp_udp_server_async
    tcp_udp_server_async.cpp
)

add_executable(tcp_udp_server_async ${tcp_udp_server_async})
if(WIN32)
    target_link_libraries(tcp_udp_server_async PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(tcp_udp_server_async PRIVATE Threads::Threads)
endif()
#=========================================================================
set(chat_client
    chat/chat_client.cpp
    chat/chat_message.hpp
)

add_executable(chat_client ${chat_client})
if(WIN32)
    target_link_libraries(chat_client PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(chat_client PRIVATE Threads::Threads)
endif()
#=========================================================================
set(chat_server
    chat/chat_server.cpp
    chat/chat_message.hpp
)

add_executable(chat_server ${chat_server})
if(WIN32)
    target_link_libraries(chat_server PRIVATE ws2_32 mswsock)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(chat_server PRIVATE Threads::Threads)
endif()
#=========================================================================