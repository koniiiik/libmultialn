#include <gtest/gtest.h>
#include <string>
#include <vector>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


using std::string;
using std::vector;

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

                seqid_t reference_id = al->requestSequenceId("reference", 240);
                seqid_t forward_id = al->requestSequenceId("forwardinf", 150);
                seqid_t reverse_id = al->requestSequenceId("reverseinf", 180);

                block = new AlignmentBlock();
                seq = GenerateSequenceDetails(GetParam(), 20, 240, false,
                        reference_id, "111110000011111");
                block->addSequence(*seq);
                delete seq;
                seq = GenerateSequenceDetails(GetParam(), 10, 150, false,
                        forward_id, "111111100000111");
                block->addSequence(*seq);
                delete seq;
                seq = GenerateSequenceDetails(GetParam(), 150, 180, true,
                        reverse_id, "000111111111100");
                block->addSequence(*seq);
                delete seq;
                al->addBlock(block);

                block = new AlignmentBlock();
                seq = GenerateSequenceDetails(GetParam(), 30, 240, false,
                        reference_id, "111110000011111");
                block->addSequence(*seq);
                delete seq;
                seq = GenerateSequenceDetails(GetParam(), 30, 150, false,
                        forward_id, "111111100000111");
                block->addSequence(*seq);
                delete seq;
                seq = GenerateSequenceDetails(GetParam(), 50, 180, true,
                        reverse_id, "000111111111100");
                block->addSequence(*seq);
                delete seq;
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

    TEST_P(WholeGenomeAlignmentTest, MultiSinglePositions)
    {
        EXPECT_THROW(al->mapPositionToAll(5), OutOfSequence);
        WholeGenomeAlignment::PositionMapping * mapping;
        EXPECT_NO_THROW(mapping = al->mapPositionToAll(23));
        ASSERT_TRUE(mapping != NULL);
        EXPECT_EQ(2, mapping->size());
        EXPECT_TRUE(mapping->find("forwardinf") != mapping->end());
        EXPECT_EQ(13, (*mapping)["forwardinf"]);
        delete mapping;
    }

    TEST_P(WholeGenomeAlignmentTest, SequenceId)
    {
        EXPECT_EQ(3, al->countKnownSequences());
        EXPECT_NO_THROW(al->getSequenceId("reference"));
        EXPECT_EQ(kReferenceSequenceId,
                al->requestSequenceId("reference", 240));
        EXPECT_EQ(3, al->countKnownSequences());

        EXPECT_THROW(al->getSequenceId("extra"), SequenceDoesNotExist);
        EXPECT_EQ(3, al->countKnownSequences());
        seqid_t id;
        EXPECT_NO_THROW(id = al->requestSequenceId("extra", 740));
        EXPECT_NO_THROW(al->getSequenceId("extra"));
        EXPECT_EQ(4, al->countKnownSequences());
        EXPECT_EQ(id, al->getSequenceId("extra"));
        EXPECT_EQ(id, al->requestSequenceId("extra", 740));
        EXPECT_EQ(kReferenceSequenceId,
                al->requestSequenceId("reference", 240));
        EXPECT_EQ(4, al->countKnownSequences());

        EXPECT_EQ("extra", al->getSequenceName(id));
        EXPECT_EQ("reference", al->getSequenceName(kReferenceSequenceId));

        vector<string> *seqlist;
        EXPECT_NO_THROW(seqlist = al->getSequenceList());
        vector<string> expected(4);
        expected[0] = "reference";
        expected[1] = "forwardinf";
        expected[2] = "reverseinf";
        expected[3] = "extra";
        EXPECT_EQ(expected, *seqlist);
        delete seqlist;
    }

    INSTANTIATE_BITSEQ_TEST_P(WholeGenomeAlignmentTest);

}  // namespace
