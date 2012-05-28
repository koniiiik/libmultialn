ADD_EXECUTABLE(generate_random_positions
    generate_random_positions.cpp
)
TARGET_LINK_LIBRARIES(generate_random_positions multialn)

ADD_EXECUTABLE(map_positions
    map_positions.cpp
    referenced_memory_size.h
    referenced_memory_size.cpp
)
TARGET_LINK_LIBRARIES(map_positions multialn)

ADD_EXECUTABLE(map_regions
    map_regions.cpp
    referenced_memory_size.h
    referenced_memory_size.cpp
)
TARGET_LINK_LIBRARIES(map_regions multialn)

ADD_EXECUTABLE(bitseq_assess
    bitseq_assess.cpp
)
TARGET_LINK_LIBRARIES(bitseq_assess multialn)
