ADD_LIBRARY(multialn
    ${PROJECT_SOURCE_DIR}/include/MultialnConstants.h
    SequenceDetails.cpp
    ${PROJECT_SOURCE_DIR}/include/SequenceDetails.h
    AlignmentBlock.cpp
    ${PROJECT_SOURCE_DIR}/include/AlignmentBlock.h
)
TARGET_LINK_LIBRARIES(multialn ${LIBCDS_LIBRARIES})
