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
            SequenceDetails *seq1, *seq2, *seq3;

            virtual void SetUp()
            {
                block = new AlignmentBlock();

                // These correspond to the following alignment:
                // reference:   11111111000000000000001111111111
                // informant1:  11111100000000001111111100000000
                // informant2:  00001111111111110000001111111111
                seq1 = GenerateSequenceDetails(GetParam(), 47, 147, false,
                        kReferenceSequenceId,
                        "11111111000000000000001111111111");
                seq2 = GenerateSequenceDetails(GetParam(), 47, 470, false,
                        2, "11111100000000001111111100000000");
                seq3 = GenerateSequenceDetails(GetParam(), 47, 747, false,
                        3, "00001111111111110000001111111111");
            }

            virtual void TearDown()
            {
                delete block;
                delete seq1;
                delete seq2;
                delete seq3;
            }
    };

    TEST_P(AlignmentBlockTest, Creation)
    {
        EXPECT_THROW(block->getReferenceSequence(), SequenceDoesNotExist);
        block->addSequence(*seq2);
        EXPECT_NO_THROW(block->getSequence(2));
        EXPECT_THROW(block->getReferenceSequence(), SequenceDoesNotExist);
        EXPECT_THROW(block->mapPositionToInformant(47, 2),
                SequenceDoesNotExist);
        block->addSequence(*seq1);
        EXPECT_NO_THROW(block->getReferenceSequence());
        EXPECT_NO_THROW(block->mapPositionToInformant(47, 2));
        EXPECT_THROW(block->mapPositionToInformant(51, 3),
                SequenceDoesNotExist);
        block->addSequence(*seq3);
        EXPECT_NO_THROW(block->mapPositionToInformant(51, 3));
    }

    TEST_P(AlignmentBlockTest, SingleMaps)
    {
        block->addSequence(*seq1);
        block->addSequence(*seq2);
        block->addSequence(*seq3);

        // The first 4 positions are in informant1 only with no shifts.
        EXPECT_EQ(48, block->mapPositionToInformant(48, 2));
        EXPECT_EQ(49, block->mapPositionToInformant(49, 2,
                    INTERVAL_BEGIN));
        EXPECT_EQ(50, block->mapPositionToInformant(50, 2,
                    INTERVAL_END));

        // Mapping positions 53 and 54 to informant1 depends on the chosen
        // interval boundary.
        EXPECT_EQ(52, block->mapPositionToInformant(53, 2,
                    INTERVAL_END));
        EXPECT_EQ(53, block->mapPositionToInformant(54, 2,
                    INTERVAL_BEGIN));
        // INTERVAL_BEGIN is the default
        EXPECT_EQ(53, block->mapPositionToInformant(53, 2));

        // Positions 55, 56 in reference are shifted by 4 in both
        // informants.
        EXPECT_EQ(59, block->mapPositionToInformant(55, 2));
        EXPECT_EQ(60, block->mapPositionToInformant(56, 2));
        EXPECT_EQ(59, block->mapPositionToInformant(55, 3));
        EXPECT_EQ(60, block->mapPositionToInformant(56, 3));

        // Finally, positions 57 through 64 are shifted by 4 in
        // informant2.
        EXPECT_EQ(61, block->mapPositionToInformant(57, 3));
        EXPECT_EQ(68, block->mapPositionToInformant(64, 3));
    }

    TEST_P(AlignmentBlockTest, MultiMaps)
    {
        block->addSequence(*seq1);
        block->addSequence(*seq2);

        // The result of mapPositionToAll should be a map containing only
        // informant1.
        const AlignmentBlock::Mapping *m;
        ASSERT_NO_THROW(m = block->mapPositionToAll(48));
        EXPECT_EQ(1, m->size());
        EXPECT_EQ(1, m->count(2));
        EXPECT_EQ(0, m->count(3));
        ASSERT_TRUE(m->end() != m->find(2));
        EXPECT_TRUE(m->end() == m->find(3));
        EXPECT_EQ(48, m->find(2)->second);
        delete m;

        block->addSequence(*seq3);

        // Position 55 has clear mapping to both informants.
        ASSERT_NO_THROW(m = block->mapPositionToAll(55));
        EXPECT_EQ(2, m->size());
        EXPECT_EQ(1, m->count(2));
        EXPECT_EQ(1, m->count(3));
        ASSERT_TRUE(m->end() != m->find(2));
        ASSERT_TRUE(m->end() != m->find(3));
        EXPECT_EQ(59, m->find(2)->second);
        EXPECT_EQ(59, m->find(3)->second);
        delete m;

        // Position 60 at the beginning of an interval can only be mapped
        // to informant2 since at this point informant1 contains a
        // trailing sequence of zeroes.
        ASSERT_NO_THROW(m = block->mapPositionToAll(60, INTERVAL_BEGIN));
        EXPECT_EQ(1, m->size());
        EXPECT_EQ(0, m->count(2));
        EXPECT_EQ(1, m->count(3));
        ASSERT_TRUE(m->end() == m->find(2));
        EXPECT_TRUE(m->end() != m->find(3));
        EXPECT_EQ(64, m->find(3)->second);
        delete m;

        // Similarly, at the end of an interval, position 48 has no
        // mapping in informant2.
        ASSERT_NO_THROW(m = block->mapPositionToAll(48, INTERVAL_END));
        EXPECT_EQ(1, m->size());
        EXPECT_EQ(1, m->count(2));
        EXPECT_EQ(0, m->count(3));
        ASSERT_TRUE(m->end() != m->find(2));
        EXPECT_TRUE(m->end() == m->find(3));
        EXPECT_EQ(48, m->find(2)->second);
        delete m;
    }

    TEST(AlignmentBlockStaticTest, Comparison)
    {
        AlignmentBlock *a = new AlignmentBlock();
        AlignmentBlock *b = new AlignmentBlock();
        SequenceDetails *seq1 = GenerateSequenceDetails(&fact_rg2, 47, 147, false,
                kReferenceSequenceId, "001011");
        SequenceDetails *seq2 = GenerateSequenceDetails(&fact_rg2, 74, 470, false,
                kReferenceSequenceId, "110100");

        a->addSequence(*seq1);
        b->addSequence(*seq2);

        delete seq1;
        delete seq2;

        EXPECT_TRUE(AlignmentBlock::compareReferencePosition(a, b));
        EXPECT_FALSE(AlignmentBlock::compareReferencePosition(b, a));

        delete a;
        delete b;
    }

    INSTANTIATE_BITSEQ_TEST_P(AlignmentBlockTest)

}  // namespace
