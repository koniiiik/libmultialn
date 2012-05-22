#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <utility>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"
#include "BitSequenceFactoryDeclarations.h"


using std::string;
using std::vector;
using std::pair;
using cds_static::BitSequence;

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
                BitSequence *bitseq;

                seqid_t reference_id = al->requestSequenceId("reference", 240);
                seqid_t forward_id = al->requestSequenceId("forwardinf", 150);
                seqid_t reverse_id = al->requestSequenceId("reverseinf", 180);

                block = new AlignmentBlock();
                SequenceDetails seq(20, false, 240, reference_id, 1, 15);
                block->addSequence(seq);
                seq = SequenceDetails(10, false, 150, forward_id, 16, 15);
                block->addSequence(seq);
                seq = SequenceDetails(50, true, 180, reverse_id, 31, 15);
                block->addSequence(seq);
                bitseq = GenerateBitSequence(GetParam(), "1"
                        "111110000011111"
                        "111111100000111"
                        "000111111111100");
                block->setBitSequence(bitseq);
                al->addBlock(block);

                block = new AlignmentBlock();
                seq = SequenceDetails(30, false, 240, reference_id, 1, 15);
                block->addSequence(seq);
                seq = SequenceDetails(30, false, 150, forward_id, 16, 15);
                block->addSequence(seq);
                seq = SequenceDetails(90, true, 180, reverse_id, 31, 15);
                block->addSequence(seq);
                bitseq = GenerateBitSequence(GetParam(), "1"
                        "111110000011111"
                        "111111100000111"
                        "000111111111100");
                block->setBitSequence(bitseq);
                al->addBlock(block);

                block = new AlignmentBlock();
                seq = SequenceDetails(50, false, 240, reference_id, 1, 15);
                block->addSequence(seq);
                seq = SequenceDetails(40, false, 150, forward_id, 16, 15);
                block->addSequence(seq);
                seq = SequenceDetails(70, true, 180, reverse_id, 31, 15);
                block->addSequence(seq);
                bitseq = GenerateBitSequence(GetParam(), "1"
                        "111110000011111"
                        "000111111111100"
                        "111111100000111");
                block->setBitSequence(bitseq);
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

    // TODO: Strengthen this; probably needs more complex test cases.
    TEST_P(WholeGenomeAlignmentTest, RegionMaps)
    {
        pair<size_t, size_t> result;

        // A few obviously invalid queries.
        EXPECT_THROW(al->mapRegionToInformant(10, 70, "nonexistent"),
                SequenceDoesNotExist);
        EXPECT_THROW(al->mapRegionToInformant(2, 5, "forwardinf"),
                OutOfSequence);
        EXPECT_THROW(al->mapRegionToInformant(42, 47, "forwardinf"),
                OutOfSequence);
        // Not a colinear sequence.
        EXPECT_THROW(al->mapRegionToInformant(20, 54, "reverseinf"),
                MappingDoesNotExist);

        // Mapping within a single block; if the start is before the first
        // block, start at the first one.
        EXPECT_NO_THROW(result = al->mapRegionToInformant(10, 25,
                    "forwardinf"));
        EXPECT_EQ(10, result.first);
        EXPECT_EQ(16, result.second);

        // Spanning multiple blocks.
        EXPECT_NO_THROW(result = al->mapRegionToInformant(21, 33,
                    "forwardinf"));
        EXPECT_EQ(11, result.first);
        EXPECT_EQ(33, result.second);

        // Spanning multiple blocks; the end is in the last but one.
        EXPECT_NO_THROW(result = al->mapRegionToInformant(31, 51,
                    "forwardinf"));
        EXPECT_EQ(31, result.first);
        EXPECT_EQ(39, result.second);

        // Reverse alignment, multiple blocks.
        EXPECT_NO_THROW(result = al->mapRegionToInformant(20, 34,
                    "reverseinf"));
        EXPECT_EQ(129, result.first);
        EXPECT_EQ(88, result.second);
    }

    INSTANTIATE_BITSEQ_TEST_P(WholeGenomeAlignmentTest);

}  // namespace
