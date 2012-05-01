#include <gtest/gtest.h>
#include <string>
#include <set>
#include <sstream>

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

// A few declarations to be able to test private code...
namespace maf_reader
{
    typedef pair<string, SequenceDetails *> ParsedLine;

    ParsedLine * parseMafLine(const string &line, BitSequenceFactory &factory,
            const set<string> *limit);
}  // namespace maf_reader

namespace
{
    using maf_reader::ParsedLine;
    using maf_reader::parseMafLine;

    BitSequenceRRRFactory factory;

    string test_line = "s hg18.chr7    27578828 38 + 158545518 "
        "AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG";

    void VerifyParsedTestLine(ParsedLine *pline)
    {
        ASSERT_TRUE(NULL != pline);
        EXPECT_EQ("hg18.chr7", pline->first);
        EXPECT_EQ(27578828, pline->second->get_start());
        EXPECT_EQ(38, pline->second->get_size());
        EXPECT_EQ(158545518, pline->second->get_src_size());
        EXPECT_EQ(0, pline->second->sequenceToAlignment(27578828));
        EXPECT_EQ(2, pline->second->sequenceToAlignment(27578830));
        EXPECT_EQ(4, pline->second->sequenceToAlignment(27578831));
        EXPECT_EQ(26, pline->second->sequenceToAlignment(27578850));
        EXPECT_EQ(41, pline->second->sequenceToAlignment(27578865));
        EXPECT_THROW(pline->second->sequenceToAlignment(27578827),
                OutOfSequence);
        EXPECT_THROW(pline->second->sequenceToAlignment(27578866),
                OutOfSequence);
    }

    TEST(MafReaderTest, LineParser)
    {
        ParsedLine *pline;
        ASSERT_NO_THROW(pline = parseMafLine(test_line, factory, NULL));
        {
            SCOPED_TRACE("");
            VerifyParsedTestLine(pline);
        }
        delete pline;

        set<string> limit;
        limit.insert("random1");
        limit.insert("random2");
        EXPECT_TRUE(NULL == parseMafLine(test_line, factory, &limit));

        limit.insert("hg18.chr7");
        ASSERT_NO_THROW(pline = parseMafLine(test_line, factory, &limit));
        {
            SCOPED_TRACE("");
            VerifyParsedTestLine(pline);
        }
        delete pline;

        EXPECT_THROW(parseMafLine("random useless stuff", factory, NULL),
                ParseError);
        EXPECT_THROW(parseMafLine("s name 47 12 + 470 ", factory, NULL), ParseError);
        EXPECT_THROW(parseMafLine("s panTro1.chr6 28741140Invalid! 38 + 161576975i "
                    "AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG", factory,
                    NULL), ParseError);
    }

    string test_file = R"(track name=euArc visibility=pack
##maf version=1 scoring=tba.v8
# tba.v8 (((human chimp) baboon) (mouse rat))

a score=23262.0
s hg18.chr7    27578828 38 + 158545518 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG
s panTro1.chr6 28741140 38 + 161576975 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG
s baboon         116834 38 +   4622798 AAA-GGGAATGTTAACCAAATGA---GTTGTCTCTTATGGTG
s mm4.chr6     53215344 38 + 151104725 -AATGGGAATGTTAAGCAAACGA---ATTGTCTCTCAGTGTG
s rn3.chr4     81344243 40 + 187371129 -AA-GGGGATGCTAAGCCAATGAGTTGTTGTCTCTCAATGTG

a score=5062.0
s hg18.chr7    27699739 6 + 158545518 TAAAGA
s panTro1.chr6 28862317 6 + 161576975 TAAAGA
s baboon         241163 6 +   4622798 TAAAGA
s mm4.chr6     53303881 6 + 151104725 TAAAGA
s rn3.chr4     81444246 6 + 187371129 taagga

a score=6636.0
s hg18.chr7    27707221 13 + 158545518 gcagctgaaaaca
s panTro1.chr6 28869787 13 + 161576975 gcagctgaaaaca
s baboon         249182 13 +   4622798 gcagctgaaaaca
s mm4.chr6     53310102 13 + 151104725 ACAGCTGAAAATA
)";

    TEST(MafReaderTest, SuccessOnValid)
    {
        istringstream s(test_file);
        AlignmentBlockStorage *storage = new BinSearchAlignmentBlockStorage();

        WholeGenomeAlignment wga("hg18.chr7", storage);
        ASSERT_NO_THROW(ReadMafFile(s, wga, factory));

        EXPECT_EQ(3, storage->size());

        AlignmentBlockStorage::Iterator *it;
        ASSERT_NO_THROW(it = storage->first());
        EXPECT_EQ(27578828, (*it)->getReferenceSequence()->get_start());
        EXPECT_NO_THROW((*it)->getSequence("panTro1.chr6"));
        EXPECT_NO_THROW((*it)->getSequence("baboon"));
        EXPECT_NO_THROW((*it)->getSequence("mm4.chr6"));
        EXPECT_NO_THROW((*it)->getSequence("rn3.chr4"));

        ++(*it);
        EXPECT_EQ(27699739, (*it)->getReferenceSequence()->get_start());
        EXPECT_NO_THROW((*it)->getSequence("panTro1.chr6"));
        EXPECT_NO_THROW((*it)->getSequence("baboon"));
        EXPECT_NO_THROW((*it)->getSequence("mm4.chr6"));
        EXPECT_NO_THROW((*it)->getSequence("rn3.chr4"));

        ++(*it);
        EXPECT_EQ(27707221, (*it)->getReferenceSequence()->get_start());
        EXPECT_NO_THROW((*it)->getSequence("panTro1.chr6"));
        EXPECT_NO_THROW((*it)->getSequence("baboon"));
        EXPECT_NO_THROW((*it)->getSequence("mm4.chr6"));
        EXPECT_THROW((*it)->getSequence("rn3.chr4"), SequenceDoesNotExist);

        EXPECT_EQ(116836, wga.mapPositionToInformant(27578830,
                    "baboon"));

        EXPECT_EQ(28869791, wga.mapPositionToInformant(27707225,
                    "panTro1.chr6"));
        EXPECT_THROW(wga.mapPositionToInformant(27707225, "rn3.chr4"),
                SequenceDoesNotExist);
    }

    TEST(MafReaderTest, FailsOnInvalid)
    {
        string invalid_input = R"(##maf version=1 scoring=tba.v8
# tba.v8 (((human chimp) baboon) (mouse rat))

a score=23262.0
s hg18.chr7    27578828 38 + 158545518 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG
s panTro1.chr6 28741140Invalid! 38 + 161576975 AAA-GGGAATGTTAACCAAATGA---ATTGTCTCTTACGGTG
)";

        istringstream s(invalid_input);
        AlignmentBlockStorage *storage = new BinSearchAlignmentBlockStorage();

        WholeGenomeAlignment wga("hg18.chr7", storage);
        EXPECT_THROW(ReadMafFile(s, wga, factory), ParseError);
    }
}  // namespace
