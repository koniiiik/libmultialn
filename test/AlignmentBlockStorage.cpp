#include <string>
#include <gtest/gtest.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>

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
            string reference;

            virtual void SetUp()
            {
                reference = "reference";
                storage = new BinSearchAlignmentBlockStorage;

                SequenceDetails *seq1, *seq2, *seq3;
                seq1 = GenerateSequenceDetails(&fact_rg2, 12, 470, false,
                        "001011");
                seq2 = GenerateSequenceDetails(&fact_rg2, 15, 470, false,
                        "1111111111");
                seq3 = GenerateSequenceDetails(&fact_rg2, 30, 470, false,
                        "11111");

                AlignmentBlock *block1, *block2, *block3;

                block1 = new AlignmentBlock(&reference);
                block2 = new AlignmentBlock(&reference);
                block3 = new AlignmentBlock(&reference);

                block1->addSequence(reference, seq1);
                block2->addSequence(reference, seq2);
                block3->addSequence(reference, seq3);

                storage->addBlock(block3);
                storage->addBlock(block1);
                storage->addBlock(block2);
            }

            virtual void TearDown()
            {
                delete storage;
            }
    };

    TEST_F(AlignmentBlockStorageTest, FindsExistingBlocks)
    {
        EXPECT_EQ(12, storage->findBlock(12)->getReferenceSequence()->get_start());
        EXPECT_EQ(12, storage->findBlock(14)->getReferenceSequence()->get_start());
        EXPECT_EQ(15, storage->findBlock(20)->getReferenceSequence()->get_start());
        EXPECT_EQ(30, storage->findBlock(32)->getReferenceSequence()->get_start());
    }

} /* namespace */
