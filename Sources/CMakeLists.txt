##################### Directories #######################
add_subdirectory(Engine)

if (CHICANE_PROGRAMS)
    add_subdirectory(Programs)
endif()

if (CHICANE_EDITOR)
    add_subdirectory(Editor)
endif()

if(CHICANE_SAMPLE)
    add_subdirectory(Sample)
endif()
