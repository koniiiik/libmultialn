#include <gtest/gtest.h>
#include <SequenceDetails.h>
#include <BitSequence.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"


namespace
{

    // The fixture for testing class Foo.
    class SequenceDetailsTest : public ::testing::Test
    {
        protected:

            SequenceDetails *forward;

            virtual void SetUp()
            {
                // equals 00001111000000001111111111110000; libcds stores
                // the sequence little-endian, which means the bit string
                // stored is 00001111111111110000000011110000
                forward = GenerateSequence<uint32_t>(47, 84, false,
                        0x0F00FFF0);
            }

            virtual void TearDown()
            {
                delete forward;
            }
    };

    // Tests lookups on non-reversed strands
    TEST_F(SequenceDetailsTest, ForwardLookup)
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

    TEST_F(SequenceDetailsTest, ExceptionHandling)
    {
        EXPECT_THROW(forward->sequenceToAlignment(42), OutOfSequence);
        EXPECT_THROW(forward->sequenceToAlignment(74), OutOfSequence);
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

}  // namespace
