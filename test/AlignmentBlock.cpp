#include <string>
#include <gtest/gtest.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"


namespace
{

    // The fixture for testing class AlignmentBlock.
    class AlignmentBlockTest: public ::testing::Test
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
                seq1 = GenerateSequence<uint32_t>(47, 147, false, 0xFFC000FF);
                seq2 = GenerateSequence<uint32_t>(47, 470, false, 0x00FF003F);
                seq3 = GenerateSequence<uint32_t>(47, 747, false, 0xFFC0FFFF);
            }

            virtual void TearDown()
            {
                delete block;
            }
    };

    TEST_F(AlignmentBlockTest, TestCreation)
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

    TEST_F(AlignmentBlockTest, TestSingleMaps)
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

    TEST_F(AlignmentBlockTest, TestMultiMap)
    {
        block->addSequence("reference", seq1);
        block->addSequence("informant1", seq2);

        // The result of mapPositionToAll should be a map containing only
        // informant1.
        const AlignmentBlock::Mapping * m = block->mapPositionToAll(48);
        ASSERT_EQ(1, m->size());
        ASSERT_EQ(1, m->count("informant1"));
        ASSERT_EQ(0, m->count("informant2"));
        ASSERT_TRUE(m->end() != m->find("informant1"));
        ASSERT_TRUE(m->end() == m->find("informant2"));
        ASSERT_EQ(48, m->find("informant1")->second);

        block->addSequence("informant2", seq3);

        m = block->mapPositionToAll(55);
        ASSERT_EQ(2, m->size());
        ASSERT_EQ(1, m->count("informant1"));
        ASSERT_EQ(1, m->count("informant2"));
        ASSERT_TRUE(m->end() != m->find("informant1"));
        ASSERT_TRUE(m->end() != m->find("informant2"));
        ASSERT_EQ(59, m->find("informant1")->second);
        ASSERT_EQ(63, m->find("informant2")->second);
    }

}  // namespace
