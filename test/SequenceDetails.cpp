#include <gtest/gtest.h>
#include <SequenceDetails.h>
#include <BitSequence.h>


namespace
{

    // The fixture for testing class Foo.
    class SequenceDetailsTest : public ::testing::Test
    {
        protected:

            SequenceDetails * forward;

            virtual void SetUp()
            {
                // equals 11011110101011011011111011101111; libcds stores
                // the sequence little-endian, which means the bit string
                // stored is 11110111011111011011010101111011
                uint32_t a = 0xDEADBEEF;
                cds_static::BitSequence *seq = new cds_static::BitSequenceRRR(&a, 32);
                forward = new SequenceDetails(47, seq->countOnes(), false, 94, seq);
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
        EXPECT_EQ(0, forward->sequenceToAlignment(47));
        EXPECT_EQ(5, forward->sequenceToAlignment(51));
        EXPECT_EQ(10, forward->sequenceToAlignment(55));

        // If we're asking for a position in this alignment containing a
        // 1, we should get the exact position match.
        EXPECT_EQ(51, forward->alignmentToSequenceBegin(5));
        EXPECT_EQ(51, forward->alignmentToSequenceEnd(5));

        // Otherwise the match should be the first position to the left or
        // to the right.
        EXPECT_EQ(51, forward->alignmentToSequenceBegin(4));
        EXPECT_EQ(49, forward->alignmentToSequenceEnd(4));
    }

}  // namespace
