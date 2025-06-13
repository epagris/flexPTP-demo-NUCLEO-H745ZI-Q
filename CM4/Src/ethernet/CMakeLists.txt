if (ETH_STACK STREQUAL "ETHERLIB")
    set(ETH_STACK_SRC 
        ethernet_etherlib.c
    )
elseif(ETH_STACK STREQUAL "LWIP")
    set(ETH_STACK_SRC 
        ethernet_lwip.c
    )
else()
    message("No Ethernet stack was defined!")
endif()

if (ETH_STACK)
    message("Ethernet stack: " ${ETH_STACK})
endif()

target_sources(
    ${CM4_TARGET}
    PUBLIC
    
    ${ETH_STACK_SRC}
)