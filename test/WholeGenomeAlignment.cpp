#include <gtest/gtest.h>
#include <string>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


using std::string;

namespace
{
    class WholeGenomeAlignmentTest: public BitSequenceParamTest
    {
        protected:

            WholeGenomeAlignment *al;

            virtual void SetUp()
            {
                AlignmentBlockStorage *storage = new
                    BinSearchAlignmentBlockStorage;

                al = new WholeGenomeAlignment("reference", storage);

                AlignmentBlock *block;
                SequenceDetails *seq;

                block = new AlignmentBlock(&al->get_reference());
                seq = GenerateSequenceDetails(GetParam(), 20, 240, false,
                        "111110000011111");
                block->addSequence("reference", seq);
                seq = GenerateSequenceDetails(GetParam(), 10, 150, false,
                        "111111100000111");
                block->addSequence("forwardinf", seq);
                seq = GenerateSequenceDetails(GetParam(), 150, 180, true,
                        "000111111111100");
                block->addSequence("reverseinf", seq);
                al->addBlock(block);

                block = new AlignmentBlock(&al->get_reference());
                seq = GenerateSequenceDetails(GetParam(), 30, 240, false,
                        "111110000011111");
                block->addSequence("reference", seq);
                seq = GenerateSequenceDetails(GetParam(), 30, 150, false,
                        "111111100000111");
                block->addSequence("forwardinf", seq);
                seq = GenerateSequenceDetails(GetParam(), 50, 180, true,
                        "000111111111100");
                block->addSequence("reverseinf", seq);
                al->addBlock(block);
            }

            virtual void TearDown()
            {
                delete al;
            }
    };

    TEST_P(WholeGenomeAlignmentTest, SinglePositions)
    {
        EXPECT_EQ(13, al->mapPositionToInformant(23, "forwardinf"));
        EXPECT_EQ(33, al->mapPositionToInformant(33, "forwardinf"));

        EXPECT_THROW(al->mapPositionToInformant(10, "forwardinf"),
                OutOfSequence);
        EXPECT_THROW(al->mapPositionToInformant(23, "nonexistent"),
                SequenceDoesNotExist);
    }

    INSTANTIATE_BITSEQ_TEST_P(WholeGenomeAlignmentTest);

}  // namespace
