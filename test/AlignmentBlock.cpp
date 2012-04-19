#include <string>
#include <gtest/gtest.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


namespace
{

    class AlignmentBlockTest: public BitSequenceParamTest
    {
        protected:

            AlignmentBlock *block;
            std::string reference_name;
            SequenceDetails *seq1, *seq2, *seq3;

            virtual void SetUp()
            {
                reference_name = "reference";
                block = new AlignmentBlock(&reference_name);

                // These correspond to the following alignment:
                // reference:   11111111000000000000001111111111
                // informant1:  11111100000000001111111100000000
                // informant2:  11111111111111110000001111111111
                seq1 = GenerateSequenceDetails(GetParam(), 47, 147, false,
                        "11111111000000000000001111111111");
                seq2 = GenerateSequenceDetails(GetParam(), 47, 470, false,
                        "11111100000000001111111100000000");
                seq3 = GenerateSequenceDetails(GetParam(), 47, 747, false,
                        "11111111111111110000001111111111");
            }

            virtual void TearDown()
            {
                delete block;
            }
    };

    TEST_P(AlignmentBlockTest, TestCreation)
    {
        EXPECT_THROW(block->getReferenceSequence(), SequenceDoesNotExist);
        block->addSequence("informant1", seq2);
        EXPECT_EQ(seq2, block->getSequence("informant1"));
        EXPECT_THROW(block->getReferenceSequence(), SequenceDoesNotExist);
        EXPECT_THROW(block->mapPositionToInformant(47, "informant1"),
                SequenceDoesNotExist);
        block->addSequence("reference", seq1);
        EXPECT_NO_THROW(block->mapPositionToInformant(47, "informant1"));
        EXPECT_EQ(block->getReferenceSequence(), seq1);
        EXPECT_THROW(block->mapPositionToInformant(47, "informant2"),
                SequenceDoesNotExist);
        block->addSequence("informant2", seq3);
        EXPECT_NO_THROW(block->mapPositionToInformant(47, "informant2"));
    }

    TEST_P(AlignmentBlockTest, TestSingleMaps)
    {
        block->addSequence("reference", seq1);
        block->addSequence("informant1", seq2);
        block->addSequence("informant2", seq3);

        // The first 6 positions are the same in all sequences, no matter
        // which interval boundary we choose.
        EXPECT_EQ(52, block->mapPositionToInformant(52, "informant1"));
        EXPECT_EQ(50, block->mapPositionToInformant(50, "informant2",
                    INTERVAL_END));

        // Mapping positions 53 and 54 to informant1 depends on the chosen
        // interval boundary.
        EXPECT_EQ(52, block->mapPositionToInformant(53, "informant1",
                    INTERVAL_END));
        EXPECT_EQ(53, block->mapPositionToInformant(54, "informant1",
                    INTERVAL_BEGIN));
        // INTERVAL_BEGIN is the default
        EXPECT_EQ(53, block->mapPositionToInformant(53, "informant1"));

        // Positions 55, 56 in reference are shifted by 4 in informant1
        // and by 8 in informant2.
        EXPECT_EQ(59, block->mapPositionToInformant(55, "informant1"));
        EXPECT_EQ(60, block->mapPositionToInformant(56, "informant1"));
        EXPECT_EQ(63, block->mapPositionToInformant(55, "informant2"));
        EXPECT_EQ(64, block->mapPositionToInformant(56, "informant2"));

        // Finally, positions 57 through 64 are shifted by 8 in
        // informant2.
        EXPECT_EQ(65, block->mapPositionToInformant(57, "informant2"));
        EXPECT_EQ(72, block->mapPositionToInformant(64, "informant2"));
    }

    TEST_P(AlignmentBlockTest, TestMultiMap)
    {
        block->addSequence("reference", seq1);
        block->addSequence("informant1", seq2);

        // The result of mapPositionToAll should be a map containing only
        // informant1.
        const AlignmentBlock::Mapping * m = block->mapPositionToAll(48);
        EXPECT_EQ(1, m->size());
        EXPECT_EQ(1, m->count("informant1"));
        EXPECT_EQ(0, m->count("informant2"));
        ASSERT_TRUE(m->end() != m->find("informant1"));
        EXPECT_TRUE(m->end() == m->find("informant2"));
        EXPECT_EQ(48, m->find("informant1")->second);

        block->addSequence("informant2", seq3);

        m = block->mapPositionToAll(55);
        EXPECT_EQ(2, m->size());
        EXPECT_EQ(1, m->count("informant1"));
        EXPECT_EQ(1, m->count("informant2"));
        ASSERT_TRUE(m->end() != m->find("informant1"));
        ASSERT_TRUE(m->end() != m->find("informant2"));
        EXPECT_EQ(59, m->find("informant1")->second);
        EXPECT_EQ(63, m->find("informant2")->second);
    }

    TEST(AlignmentBlockStaticTest, TestComparison)
    {
        std::string reference_name = "reference";
        AlignmentBlock *a = new AlignmentBlock(&reference_name);
        AlignmentBlock *b = new AlignmentBlock(&reference_name);
        SequenceDetails *seq1 = GenerateSequenceDetails(&fact_rg2, 47, 147, false,
                "001011");
        SequenceDetails *seq2 = GenerateSequenceDetails(&fact_rg2, 74, 470, false,
                "110100");

        a->addSequence(reference_name, seq1);
        b->addSequence(reference_name, seq2);

        EXPECT_TRUE(AlignmentBlock::compareReferencePosition(a, b));
        EXPECT_FALSE(AlignmentBlock::compareReferencePosition(b, a));

        delete a;
        delete b;
    }

    INSTANTIATE_BITSEQ_TEST_P(AlignmentBlockTest)

}  // namespace
