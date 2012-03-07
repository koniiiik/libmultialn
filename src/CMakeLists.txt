ADD_LIBRARY(multialn
    SequenceDetails.cpp
    ${PROJECT_SOURCE_DIR}/include/SequenceDetails.h
)
TARGET_LINK_LIBRARIES(multialn ${LIBCDS_LIBRARIES})
