#include <string>
#include <gtest/gtest.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


using std::string;
using ::testing::Test;

namespace
{

    class AlignmentBlockStorageTest: public Test
    {
        protected:
            AlignmentBlockStorage *storage;

            virtual void SetUp()
            {
                storage = new BinSearchAlignmentBlockStorage;

                SequenceDetails *seq1, *seq2, *seq3;
                seq1 = GenerateSequenceDetails(&fact_rg2, 12, false,
                        kReferenceSequenceId, "001011");
                seq2 = GenerateSequenceDetails(&fact_rg2, 15, false,
                        kReferenceSequenceId, "1111111111");
                seq3 = GenerateSequenceDetails(&fact_rg2, 30, false,
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

    TEST_F(AlignmentBlockStorageTest, CorrectSize)
    {
        EXPECT_EQ(3, storage->size());
    }

    TEST_F(AlignmentBlockStorageTest, FindsExistingBlocks)
    {
        EXPECT_EQ(12, storage->getBlock(12)->getReferenceSequence()->get_start());
        EXPECT_EQ(12, storage->getBlock(14)->getReferenceSequence()->get_start());
        EXPECT_EQ(15, storage->getBlock(20)->getReferenceSequence()->get_start());
        EXPECT_EQ(30, storage->getBlock(32)->getReferenceSequence()->get_start());
    }

    TEST_F(AlignmentBlockStorageTest, ThrowsOnNonexisting)
    {
        EXPECT_THROW(storage->getBlock(47), OutOfSequence);
        EXPECT_THROW(storage->getBlock(11), OutOfSequence);
        EXPECT_THROW(storage->getBlock(25), OutOfSequence);
        EXPECT_THROW(storage->getBlock(29), OutOfSequence);
        EXPECT_NO_THROW(storage->getBlock(12));
        EXPECT_NO_THROW(storage->getBlock(14));
        EXPECT_NO_THROW(storage->getBlock(15));
        EXPECT_NO_THROW(storage->getBlock(24));
        EXPECT_NO_THROW(storage->getBlock(32));
    }

    TEST_F(AlignmentBlockStorageTest, Iterators)
    {
        EXPECT_THROW(storage->find(5), OutOfSequence);
        AlignmentBlockStorage::Iterator *it1, *it2;
        EXPECT_NO_THROW(it1 = storage->first());
        EXPECT_NO_THROW(it2 = storage->find(15));
        EXPECT_EQ(12, (*it1)->getReferenceSequence()->get_start());
        EXPECT_EQ(12, (**it1).getReferenceSequence()->get_start());
        EXPECT_FALSE(*it1 == *it2);
        EXPECT_TRUE(*it1 != *it2);
        (*it1)++;
        EXPECT_EQ(15, (*it1)->getReferenceSequence()->get_start());
        EXPECT_TRUE(*it1 == *it2);
        EXPECT_FALSE(*it1 != *it2);
        (*it1)++;
        EXPECT_EQ(30, (*it1)->getReferenceSequence()->get_start());
        delete it2;
        delete it1;
    }

} /* namespace */
