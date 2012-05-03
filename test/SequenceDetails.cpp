#include <gtest/gtest.h>
#include <SequenceDetails.h>
#include <BitSequence.h>
#include <MultialnConstants.h>
#include <BitSequenceFactory.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


namespace
{

    class SequenceDetailsTest : public BitSequenceParamTest
    {
        protected:

            SequenceDetails *forward;

            virtual void SetUp()
            {
                forward = GenerateSequenceDetails(GetParam(), 47, 84,
                        false, kReferenceSequenceId,
                        "00001111111111110000000011110000");
            }

            virtual void TearDown()
            {
                delete forward;
            }
    };

    // Tests lookups on non-reversed strands
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
    }

    INSTANTIATE_BITSEQ_TEST_P(SequenceDetailsTest);

    TEST(SequenceDetailsTest, Comparison)
    {
        SequenceDetails *first = GenerateSequenceDetails(&fact_rg2, 47,
                84, false, 1, "1100");
        SequenceDetails *second = GenerateSequenceDetails(&fact_rg2, 47,
                84, false, 2, "0011");
        EXPECT_TRUE(SequenceDetails::compareById(first, second));
        EXPECT_FALSE(SequenceDetails::compareById(second, first));

        delete first;
        delete second;
    }

}  // namespace
