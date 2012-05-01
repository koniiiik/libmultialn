ADD_LIBRARY(multialn
    ${PROJECT_SOURCE_DIR}/include/MultialnConstants.h
    SequenceDetails.cpp
    ${PROJECT_SOURCE_DIR}/include/SequenceDetails.h
    AlignmentBlock.cpp
    ${PROJECT_SOURCE_DIR}/include/AlignmentBlock.h
    AlignmentBlockStorage.cpp
    ${PROJECT_SOURCE_DIR}/include/AlignmentBlockStorage.h
    BinSearchAlignmentBlockStorage.cpp
    ${PROJECT_SOURCE_DIR}/include/BinSearchAlignmentBlockStorage.h
    WholeGenomeAlignment.cpp
    ${PROJECT_SOURCE_DIR}/include/WholeGenomeAlignment.h
    MafReader.cpp
    ${PROJECT_SOURCE_DIR}/include/MafReader.h
)
TARGET_LINK_LIBRARIES(multialn ${LIBCDS_LIBRARIES})
