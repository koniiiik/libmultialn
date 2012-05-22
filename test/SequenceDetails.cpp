#include <gtest/gtest.h>
#include <SequenceDetails.h>
#include <BitSequence.h>
#include <MultialnConstants.h>
#include <BitSequenceFactory.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


using cds_static::BitSequence;

namespace
{

    class SequenceDetailsTest : public BitSequenceParamTest
    {
        protected:

            SequenceDetails *forward, *backward;
            BitSequence *bit_sequence;

            virtual void SetUp()
            {
                forward = new SequenceDetails(47, false, 84,
                        kReferenceSequenceId, 1, 32);
                backward = new SequenceDetails(16, true, 64,
                        kReferenceSequenceId, 33, 32);

                bit_sequence = GenerateBitSequence(GetParam(), "1"
                        "00001111111111110000000011110000"
                        "00001111111111110000000011110000");

                forward->set_sequence(bit_sequence);
                backward->set_sequence(bit_sequence);
            }

            virtual void TearDown()
            {
                delete forward;
                delete backward;
                delete bit_sequence;
            }
    };

    TEST_P(SequenceDetailsTest, StartEndSize)
    {
        EXPECT_EQ(47, forward->get_start());
        EXPECT_EQ(16, forward->get_size());
        EXPECT_EQ(62, forward->get_end());
        EXPECT_EQ(47, backward->get_start());
        EXPECT_EQ(16, backward->get_size());
        EXPECT_EQ(32, backward->get_end());
    }

    TEST_P(SequenceDetailsTest, ForwardLookup)
    {
        // The sequence is zero-based, sequence offset is taken into
        // account.
        EXPECT_EQ(4, forward->sequenceToAlignment(47));
        EXPECT_EQ(24, forward->sequenceToAlignment(59));

        // If we're asking for a position in this alignment containing a
        // 1, we should get the exact position match.
        EXPECT_EQ(48, forward->alignmentToSequence(5, INTERVAL_BEGIN));
        EXPECT_EQ(48, forward->alignmentToSequence(5, INTERVAL_END));

        // Otherwise the match should be the first position to the left or
        // to the right.
        EXPECT_EQ(59, forward->alignmentToSequence(19, INTERVAL_BEGIN));
        EXPECT_EQ(58, forward->alignmentToSequence(19, INTERVAL_END));
    }

    TEST_P(SequenceDetailsTest, BackwardLookup)
    {
        EXPECT_EQ(27, backward->sequenceToAlignment(32));
        EXPECT_EQ(4, backward->sequenceToAlignment(47));

        EXPECT_EQ(42, backward->alignmentToSequence(9));
        EXPECT_EQ(35, backward->alignmentToSequence(18, INTERVAL_BEGIN));
        EXPECT_EQ(36, backward->alignmentToSequence(18, INTERVAL_END));
    }


    TEST_P(SequenceDetailsTest, ExceptionHandling)
    {
        EXPECT_THROW(forward->sequenceToAlignment(42), OutOfSequence);
        EXPECT_THROW(forward->sequenceToAlignment(74), OutOfSequence);
        EXPECT_THROW(forward->sequenceToAlignment(64), OutOfSequence);
        EXPECT_THROW(forward->sequenceToAlignment(63), OutOfSequence);
        EXPECT_NO_THROW(forward->sequenceToAlignment(56));

        EXPECT_THROW(forward->alignmentToSequence(-1), OutOfSequence);
        EXPECT_THROW(forward->alignmentToSequence(35), OutOfSequence);
        EXPECT_NO_THROW(forward->alignmentToSequence(3, INTERVAL_BEGIN));
        EXPECT_NO_THROW(forward->alignmentToSequence(30, INTERVAL_END));

        EXPECT_THROW(forward->alignmentToSequence(3, INTERVAL_END),
                OutOfSequence);
        EXPECT_THROW(forward->alignmentToSequence(30, INTERVAL_BEGIN),
                OutOfSequence);
        EXPECT_THROW(backward->sequenceToAlignment(31), OutOfSequence);
        EXPECT_THROW(backward->sequenceToAlignment(48), OutOfSequence);
    }

    INSTANTIATE_BITSEQ_TEST_P(SequenceDetailsTest);

    TEST(SequenceDetailsTest, Comparison)
    {
        SequenceDetails first(47, false, 84, 1, 5, 47);
        SequenceDetails second(47, false, 84, 2, 4, 52);
        EXPECT_TRUE(SequenceDetails::compareById(first, second));
        EXPECT_FALSE(SequenceDetails::compareById(second, first));
    }

}  // namespace
