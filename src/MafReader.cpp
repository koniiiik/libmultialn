#include <string>
#include <set>
#include <fstream>
#include <istream>
#include <sstream>
#include <ios>

#include <BitString.h>

#include <MafReader.h>
#include <WholeGenomeAlignment.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <BitSequenceFactory.h>


namespace maf_reader
{

using std::istream;
using std::string;
using std::set;

typedef pair<string, SequenceDetails *> ParsedLine;

string getNextLine(istream &s)
{
    using std::getline;
    string buf;
    getline(s, buf);
    while ((!buf.empty())
           && (buf[0] != 'a')
           && (buf[0] != 's'))
    {
        getline(s, buf);
    }
    return buf;
}

ParsedLine * parseMafLine(const string &line, BitSequenceFactory &factory,
        const set<string> *limit)
{
    std::istringstream s(line);
    s.exceptions(istream::failbit | istream::badbit);

    try
    {
        string buf, name;

        // Skip the "s" marker at the beginning of the line.
        s >> buf;
        s >> name;

        if (limit != NULL && limit->count(name) == 0)
        {
            return NULL;
        }

        size_t start, size, src_size;
        bool reverse = false;
        s >> start >> size >> buf >> src_size;
        if (buf[0] == '-')
        {
            reverse = true;
        }

        // We build a BitString according to the sequence we read, dashes (aka
        // insertions) are zeroes, everything else is one.
        s >> buf;
        cds_utils::BitString bitstr(buf.size());
        for (size_t i = 0; i < buf.size(); ++i)
        {
            bitstr.setBit(i, buf[i] != '-');
        }

        cds_static::BitSequence *bitseq = factory.getInstance(bitstr);

        // We have all we need, create and return the instance.
        SequenceDetails *datials = new SequenceDetails(start, size, reverse,
                src_size, bitseq);
        return new ParsedLine(name, datials);
    }
    catch (std::ios_base::failure &e)
    {
        throw ParseError();
    }
}

void ReadMafFile(const string &file_name, WholeGenomeAlignment &wga,
        BitSequenceFactory &factory, const set<string> *limit)
{
    std::ifstream s(file_name.c_str());
    ReadMafFile(s, wga, factory, limit);
}

void ReadMafFile(istream &s, WholeGenomeAlignment &wga,
        BitSequenceFactory &factory, const set<string> *limit)
{
    s.exceptions(istream::failbit | istream::badbit);
    try
    {
        // For each block...
        while (1)
        {
            string buf = getNextLine(s);
            // Skip the first empty line preceding all blocks.
            if (buf.empty())
            {
                buf = getNextLine(s);
            }
            while (buf[0] != 'a')
            {
                // We encountered an invalid paragraph, skip it.
                buf = getNextLine(s);
                while (!buf.empty())
                {
                    buf = getNextLine(s);
                }
                buf = getNextLine(s);
            }
            // Skip the line marking the start of a block.
            buf = getNextLine(s);
            AlignmentBlock *block = new AlignmentBlock(&(wga.get_reference()));
            // This try block is required to finish the last alignment
            // block in the MAF and successfully add it to our WGA.
            try
            {
                // Iterate over the block itself.
                while (!buf.empty())
                {
                    if (buf[0] != 's')
                    {
                        throw ParseError();
                    }
                    ParsedLine *pline = parseMafLine(buf, factory, limit);
                    if (pline != NULL)
                    {
                        block->addSequence(pline->first, pline->second);
                        delete pline;
                    }
                    buf = getNextLine(s);
                }
            }
            catch (std::ios_base::failure &e)
            {
                wga.addBlock(block);
                return;
            }
            catch (ParseError &e)
            {
                delete block;
                throw;
            }
            wga.addBlock(block);
        }
    }
    catch (std::ios_base::failure &e)
    { };
}

} /* namespace maf_reader */
