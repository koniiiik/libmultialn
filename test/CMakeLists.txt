INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include)

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
TARGET_LINK_LIBRARIES(multialn_test multialn gtest gtest_main)

ADD_TEST(multialn_test multialn_test)
