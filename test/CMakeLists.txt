FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

ADD_EXECUTABLE(multialn_test
    SequenceDetails.cpp
    AlignmentBlock.cpp
    BitSequence.cpp
    BitSequenceFactoryDeclarations.h
    BitSequenceFactoryDefinitions.cpp
    SequenceGenerator.h
    SequenceGenerator.cpp
    AlignmentBlockStorage.cpp
    WholeGenomeAlignment.cpp
    MafReader.cpp
)
TARGET_LINK_LIBRARIES(multialn_test multialn ${GTEST_BOTH_LIBRARIES})

ADD_TEST(multialn_test multialn_test)
