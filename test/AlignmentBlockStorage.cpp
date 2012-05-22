#include <string>
#include <gtest/gtest.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <RankAlignmentBlockStorage.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


using std::string;
using ::testing::Test;
using ::testing::Types;

namespace
{

    template <typename T>
    class AlignmentBlockStorageTest: public Test
    {
        protected:
            AlignmentBlockStorage *storage;

            virtual void SetUp()
            {
                storage = new T();

                SequenceDetails *seq1, *seq2, *seq3;
                seq1 = GenerateSequenceDetails(&fact_rg2, 12, 470, false,
                        kReferenceSequenceId, "001011");
                seq2 = GenerateSequenceDetails(&fact_rg2, 15, 470, false,
                        kReferenceSequenceId, "1111111111");
                seq3 = GenerateSequenceDetails(&fact_rg2, 30, 470, false,
                        kReferenceSequenceId, "11111");

                AlignmentBlock *block1, *block2, *block3;

                block1 = new AlignmentBlock();
                block2 = new AlignmentBlock();
                block3 = new AlignmentBlock();

                block1->addSequence(*seq1);
                block2->addSequence(*seq2);
                block3->addSequence(*seq3);

                delete seq1;
                delete seq2;
                delete seq3;

                storage->addBlock(block3);
                storage->addBlock(block1);
                storage->addBlock(block2);
            }

            virtual void TearDown()
            {
                delete storage;
            }
    };

    typedef Types<BinSearchAlignmentBlockStorage,
            RankAlignmentBlockStorage> AlignmentBlockStorageTypes;
    TYPED_TEST_CASE(AlignmentBlockStorageTest, AlignmentBlockStorageTypes);

    TYPED_TEST(AlignmentBlockStorageTest, CorrectSize)
    {
        EXPECT_EQ(3, this->storage->size());
    }

    TYPED_TEST(AlignmentBlockStorageTest, FindsExistingBlocks)
    {
        EXPECT_EQ(12, this->storage->getBlock(12)->getReferenceSequence()->get_start());
        EXPECT_EQ(12, this->storage->getBlock(14)->getReferenceSequence()->get_start());
        EXPECT_EQ(15, this->storage->getBlock(20)->getReferenceSequence()->get_start());
        EXPECT_EQ(30, this->storage->getBlock(32)->getReferenceSequence()->get_start());
    }

    TYPED_TEST(AlignmentBlockStorageTest, ThrowsOnNonexisting)
    {
        EXPECT_THROW(this->storage->getBlock(47), OutOfSequence);
        EXPECT_THROW(this->storage->getBlock(11), OutOfSequence);
        EXPECT_THROW(this->storage->getBlock(25), OutOfSequence);
        EXPECT_THROW(this->storage->getBlock(29), OutOfSequence);
        EXPECT_NO_THROW(this->storage->getBlock(12));
        EXPECT_NO_THROW(this->storage->getBlock(14));
        EXPECT_NO_THROW(this->storage->getBlock(15));
        EXPECT_NO_THROW(this->storage->getBlock(24));
        EXPECT_NO_THROW(this->storage->getBlock(32));
    }

    TYPED_TEST(AlignmentBlockStorageTest, Iterators)
    {
        EXPECT_THROW(this->storage->find(5), OutOfSequence);
        AlignmentBlockStorage::iterator it1, it2;
        EXPECT_NO_THROW(it1 = this->storage->begin());
        EXPECT_NO_THROW(it2 = this->storage->find(15));
        EXPECT_EQ(12, it1->getReferenceSequence()->get_start());
        EXPECT_EQ(12, (*it1).getReferenceSequence()->get_start());
        EXPECT_FALSE(it1 == it2);
        EXPECT_TRUE(it1 != it2);
        it1++;
        EXPECT_EQ(15, it1->getReferenceSequence()->get_start());
        EXPECT_TRUE(it1 == it2);
        EXPECT_FALSE(it1 != it2);
        it1++;
        EXPECT_EQ(30, it1->getReferenceSequence()->get_start());
        EXPECT_TRUE(it1 != this->storage->end());
        EXPECT_FALSE(it1 == this->storage->end());
        ++it1;
        EXPECT_FALSE(it1 != this->storage->end());
        EXPECT_TRUE(it1 == this->storage->end());
    }

} /* namespace */
