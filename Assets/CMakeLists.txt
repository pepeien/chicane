##################### Directories #######################
add_subdirectory(Engine)

if (CHICANE_EDITOR)
    add_subdirectory(Editor)
endif()

if(CHICANE_SAMPLE)
    add_subdirectory(Sample)
endif()
