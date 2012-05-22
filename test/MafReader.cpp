#include <gtest/gtest.h>
#include <string>
#include <set>
#include <sstream>
#include <BitString.h>
#include <BitSequence.h>

#include <MafReader.h>
#include <WholeGenomeAlignment.h>
#include <BitSequenceFactory.h>
#include <AlignmentBlockStorage.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <SequenceDetails.h>


using std::string;
using std::set;
using maf_reader::ReadMafFile;
using maf_reader::ParseError;
using ::testing::Test;
using cds_utils::BitString;
using cds_static::BitSequence;

// A few declarations to be able to test private code...
namespace maf_reader
{
    bool passesLimitCheck(const string &line, const set<string> *limit);
    SequenceDetails parseMafLine(const string &line,
            WholeGenomeAlignment &wga, size_t offset, BitString &bitstr);
}  // namespace maf_reader

namespace
{
    using maf_reader::parseMafLine;
    using maf_reader::passesLimitCheck;

    BitSequenceRRRFactory factory;

    string test_line = "s hg18.chr7    27578828 38 + 158545518 "
        "AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG";

    TEST(MafReaderTest, LimitCheck)
    {
        set<string> limit;
        limit.insert("random1");
        limit.insert("random2");
        EXPECT_FALSE(passesLimitCheck(test_line, &limit));
        limit.insert("hg18.chr7");
        EXPECT_TRUE(passesLimitCheck(test_line, &limit));
        EXPECT_TRUE(passesLimitCheck(test_line, NULL));
    }

    void VerifyParsedTestLine(SequenceDetails *seq,
            WholeGenomeAlignment *wga)
    {
        ASSERT_TRUE(NULL != seq);
        EXPECT_EQ(kReferenceSequenceId, seq->get_id());
        EXPECT_EQ(27578828, seq->get_start());
        EXPECT_EQ(38, seq->get_size());
        EXPECT_EQ(158545518, seq->get_src_size());
        EXPECT_EQ(158545518, wga->getReferenceSize());
        EXPECT_EQ(0, seq->sequenceToAlignment(27578828));
        EXPECT_EQ(2, seq->sequenceToAlignment(27578830));
        EXPECT_EQ(4, seq->sequenceToAlignment(27578831));
        EXPECT_EQ(26, seq->sequenceToAlignment(27578850));
        EXPECT_EQ(41, seq->sequenceToAlignment(27578865));
        EXPECT_THROW(seq->sequenceToAlignment(27578827), OutOfSequence);
        EXPECT_THROW(seq->sequenceToAlignment(27578866), OutOfSequence);
    }

    TEST(MafReaderTest, LineParser)
    {
        WholeGenomeAlignment *wga = new WholeGenomeAlignment("hg18.chr7",
                NULL);
        SequenceDetails seq(0, false, 0, 0, 0, 0);
        BitString bitstr(100);
        ASSERT_NO_THROW(seq = parseMafLine(test_line, *wga, 0, bitstr));
        {
            SCOPED_TRACE("");
            BitSequence *bit_sequence = factory.getInstance(bitstr);
            seq.set_sequence(bit_sequence);
            VerifyParsedTestLine(&seq, wga);
            delete bit_sequence;
        }
        delete wga;

        wga = new WholeGenomeAlignment("hg18.chr7", NULL);
        EXPECT_THROW(parseMafLine("random useless stuff", *wga, 0, bitstr),
                ParseError);
        EXPECT_THROW(parseMafLine("s name 47 12 + 470 ", *wga, 0, bitstr),
                ParseError);
        EXPECT_THROW(parseMafLine("s panTro1.chr6 28741140Invalid! 38 + 161576975i "
                    "AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG", *wga, 0,
                    bitstr), ParseError);
        delete wga;
    }

    string test_file = "track name=euArc visibility=pack\n\
##maf version=1 scoring=tba.v8\n\
# tba.v8 (((human chimp) baboon) (mouse rat))\n\
\n\
a score=23262.0\n\
s hg18.chr7    27578828 38 + 158545518 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG\n\
s panTro1.chr6 28741140 38 + 161576975 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG\n\
s baboon         116834 38 +   4622798 AAA-GGGAATGTTAACCAAATGA---GTTGTCTCTTATGGTG\n\
s mm4.chr6     53215344 38 + 151104725 -AATGGGAATGTTAAGCAAACGA---ATTGTCTCTCAGTGTG\n\
s rn3.chr4     81344243 40 + 187371129 -AA-GGGGATGCTAAGCCAATGAGTTGTTGTCTCTCAATGTG\n\
\n\
a score=5062.0\n\
s hg18.chr7    27699739 6 + 158545518 TAAAGA\n\
s panTro1.chr6 28862317 6 + 161576975 TAAAGA\n\
s baboon         241163 6 +   4622798 TAAAGA\n\
s mm4.chr6     53303881 6 + 151104725 TAAAGA\n\
s rn3.chr4     81444246 6 + 187371129 taagga\n\
\n\
a score=6636.0\n\
s hg18.chr7    27707221 13 + 158545518 gcagctgaaaaca\n\
s panTro1.chr6 28869787 13 + 161576975 gcagctgaaaaca\n\
s baboon         249182 13 +   4622798 gcagctgaaaaca\n\
s mm4.chr6     53310102 13 + 151104725 ACAGCTGAAAATA\n\
";

    TEST(MafReaderTest, SuccessOnValid)
    {
        istringstream s(test_file);
        AlignmentBlockStorage *storage = new BinSearchAlignmentBlockStorage();

        WholeGenomeAlignment wga("hg18.chr7", storage);
        ASSERT_NO_THROW(ReadMafFile(s, wga, factory));

        EXPECT_EQ(3, storage->size());

        {
            AlignmentBlockStorage::iterator it;
            ASSERT_NO_THROW(it = storage->begin());
            EXPECT_EQ(27578828, it->getReferenceSequence()->get_start());
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("panTro1.chr6")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("baboon")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("mm4.chr6")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("rn3.chr4")));

            ++it;
            EXPECT_EQ(27699739, it->getReferenceSequence()->get_start());
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("panTro1.chr6")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("baboon")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("mm4.chr6")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("rn3.chr4")));

            ++it;
            EXPECT_EQ(27707221, it->getReferenceSequence()->get_start());
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("panTro1.chr6")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("baboon")));
            EXPECT_NO_THROW(it->getSequence(wga.getSequenceId("mm4.chr6")));
            EXPECT_THROW(it->getSequence(wga.getSequenceId("rn3.chr4")),
                    SequenceDoesNotExist);
        }

        EXPECT_EQ(116836, wga.mapPositionToInformant(27578830,
                    "baboon"));

        EXPECT_EQ(28869791, wga.mapPositionToInformant(27707225,
                    "panTro1.chr6"));
        EXPECT_THROW(wga.mapPositionToInformant(27707225, "rn3.chr4"),
                SequenceDoesNotExist);

        EXPECT_EQ(5, wga.countKnownSequences());
    }

    TEST(MafReaderTest, FailsOnInvalid)
    {
        string invalid_input = "##maf version=1 scoring=tba.v8\n\
# tba.v8 (((human chimp) baboon) (mouse rat))\n\
\n\
a score=23262.0\n\
s hg18.chr7    27578828 38 + 158545518 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG\n\
s panTro1.chr6 28741140Invalid! 38 + 161576975 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG\n\
";

        istringstream s(invalid_input);
        AlignmentBlockStorage *storage = new BinSearchAlignmentBlockStorage();

        WholeGenomeAlignment wga("hg18.chr7", storage);
        EXPECT_THROW(ReadMafFile(s, wga, factory), ParseError);
    }
}  // namespace
