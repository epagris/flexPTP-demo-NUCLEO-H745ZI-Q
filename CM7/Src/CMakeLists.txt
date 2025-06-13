target_sources(${CM7_TARGET} PUBLIC
    main.c
    syscall.c
    sysmem.c
)

add_subdirectory(standard_output)