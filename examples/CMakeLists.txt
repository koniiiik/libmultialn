ADD_EXECUTABLE(random_positions
    random_positions.cpp
)
TARGET_LINK_LIBRARIES(random_positions multialn)

ADD_EXECUTABLE(bitseq_assess
    bitseq_assess.cpp
)
TARGET_LINK_LIBRARIES(bitseq_assess multialn)
