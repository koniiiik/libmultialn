#include <gtest/gtest.h>
#include <string>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <MultialnConstants.h>

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

                seqid_t reference_id = al->requestSequenceId("reference");
                seqid_t forward_id = al->requestSequenceId("forwardinf");
                seqid_t reverse_id = al->requestSequenceId("reverseinf");

                block = new AlignmentBlock();
                seq = GenerateSequenceDetails(GetParam(), 20, 240, false,
                        reference_id, "111110000011111");
                block->addSequence(seq);
                seq = GenerateSequenceDetails(GetParam(), 10, 150, false,
                        forward_id, "111111100000111");
                block->addSequence(seq);
                seq = GenerateSequenceDetails(GetParam(), 150, 180, true,
                        reverse_id, "000111111111100");
                block->addSequence(seq);
                al->addBlock(block);

                block = new AlignmentBlock();
                seq = GenerateSequenceDetails(GetParam(), 30, 240, false,
                        reference_id, "111110000011111");
                block->addSequence(seq);
                seq = GenerateSequenceDetails(GetParam(), 30, 150, false,
                        forward_id, "111111100000111");
                block->addSequence(seq);
                seq = GenerateSequenceDetails(GetParam(), 50, 180, true,
                        reverse_id, "000111111111100");
                block->addSequence(seq);
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

    TEST_P(WholeGenomeAlignmentTest, SequenceId)
    {
        EXPECT_EQ(3, al->countKnownSequences());
        EXPECT_NO_THROW(al->getSequenceId("reference"));
        EXPECT_EQ(kReferenceSequenceId,
                al->requestSequenceId("reference"));

        EXPECT_THROW(al->getSequenceId("extra"), SequenceDoesNotExist);
        seqid_t id;
        EXPECT_NO_THROW(id = al->requestSequenceId("extra"));
        EXPECT_NO_THROW(al->getSequenceId("extra"));
        EXPECT_EQ(id, al->getSequenceId("extra"));
        EXPECT_EQ(id, al->requestSequenceId("extra"));
        EXPECT_EQ(kReferenceSequenceId,
                al->requestSequenceId("reference"));
    }

    INSTANTIATE_BITSEQ_TEST_P(WholeGenomeAlignmentTest);

}  // namespace
