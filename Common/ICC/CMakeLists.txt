set(ICC_CORE_SPECIFIC "")

if(ICC_CORE STREQUAL "CM7")
    set(ICC_CORE_SPECIFIC icc_cm7.c)
elseif(ICC_CORE STREQUAL "CM4")
    set(ICC_CORE_SPECIFIC icc_cm4.c)
endif()


target_sources(${ICC_TARGET} PUBLIC
    ${ICC_CORE_SPECIFIC}
    icc.h
    
    icc_queue.c
    icc_queue.h
)