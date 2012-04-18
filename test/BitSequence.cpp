#include <string>
#include <gtest/gtest.h>
#include <BitSequence.h>
#include <BitString.h>
#include <BitSequenceFactory.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


using ::testing::TestWithParam;
using ::testing::Values;
using cds_static::BitSequence;
using cds_utils::BitString;

namespace
{

    class BitSequenceTest: public BitSequenceParamTest
    {
        protected:
            BitSequence * seq;

            virtual void SetUp()
            {
                seq = GenerateBitSequence(GetParam(), "000111000111000");
            }

            virtual void TearDown()
            {
                delete seq;
            }
    };

    TEST_P(BitSequenceTest, Access)
    {
        EXPECT_FALSE(seq->access(0));
        EXPECT_FALSE(seq->access(1));
        EXPECT_FALSE(seq->access(2));
        EXPECT_TRUE(seq->access(3));
        EXPECT_TRUE(seq->access(5));
        EXPECT_FALSE(seq->access(6));
        EXPECT_FALSE(seq->access(8));
        EXPECT_TRUE(seq->access(10));
        EXPECT_FALSE(seq->access(13));
    }

    TEST_P(BitSequenceTest, Rank1)
    {
        EXPECT_EQ(0, seq->rank1(0));
        EXPECT_EQ(0, seq->rank1(2));
        EXPECT_EQ(1, seq->rank1(3));
        EXPECT_EQ(3, seq->rank1(5));
        EXPECT_EQ(3, seq->rank1(6));
        EXPECT_EQ(3, seq->rank1(8));
        EXPECT_EQ(4, seq->rank1(9));
        EXPECT_EQ(6, seq->rank1(11));
        EXPECT_EQ(6, seq->rank1(12));
        EXPECT_EQ(6, seq->rank1(14));
    }

    TEST_P(BitSequenceTest, Select1)
    {
        EXPECT_EQ(3, seq->select1(1));
        EXPECT_EQ(4, seq->select1(2));
        EXPECT_EQ(5, seq->select1(3));
        EXPECT_EQ(9, seq->select1(4));
        EXPECT_EQ(11, seq->select1(6));

        // Should return (size_t)-1 but segfaults on BitSequenceRG, have
        // to avoid.
        //EXPECT_EQ((size_t)(-1), seq->select1(0));

        // This is supposed to be equal but in practice they are -1 cast
        // to either uint32_t or uint64_t depending on implementation.
        EXPECT_LE(15, seq->select1(7));
        EXPECT_LE(15, seq->select1(20));
    }

    TEST_P(BitSequenceTest, SelectNext1)
    {
        EXPECT_EQ(3, seq->selectNext1(0));
        EXPECT_EQ(3, seq->selectNext1(1));
        EXPECT_EQ(3, seq->selectNext1(2));
        EXPECT_EQ(3, seq->selectNext1(3));
        EXPECT_EQ(5, seq->selectNext1(5));
        EXPECT_EQ(9, seq->selectNext1(6));

        // This is also suuposed to be equal but in reality it returns -1
        // cast to uint64_t on RRR and SDArray.
        EXPECT_LE(15, seq->selectNext1(12));
    }

    /*
    TEST_P(BitSequenceTest, SelectPrev1)
    {
        // This seems to be completely broken on RRR and SDArray. On RG it
        // starts at the given index as documented, on the other two it
        // skips it right away. Not usable.
        EXPECT_EQ(3, seq->selectPrev1(3));
        EXPECT_EQ(5, seq->selectPrev1(5));
        EXPECT_EQ(5, seq->selectPrev1(6));
        EXPECT_EQ(5, seq->selectPrev1(8));
        EXPECT_EQ(9, seq->selectPrev1(9));
    }
    */

    INSTANTIATE_BITSEQ_TEST_P(
            AllBitSequenceImplementations,
            BitSequenceTest);

}  // namespace
