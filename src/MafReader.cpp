#include <string>
#include <set>
#include <vector>
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
#include <MultialnConstants.h>


namespace maf_reader
{

using std::istream;
using std::istringstream;
using std::string;
using std::set;
using std::vector;
using cds_utils::BitString;

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

// TODO: Some error handling in getBlockLength and passesLimitCheck might
// be appropriate.

size_t getBlockLength(const string &line)
{
    string buf;
    istringstream s(line);
    for (size_t i = 0; i < 7; ++i)
    {
        s >> buf;
    }
    return buf.size();
}

bool passesLimitCheck(const string &line, const set<string> *limit)
{
    if (limit == NULL)
    {
        return true;
    }
    string buf;
    istringstream s(line);
    s >> buf;
    s >> buf;
    return limit->count(buf) != 0;
}

SequenceDetails parseMafLine(const string &line, WholeGenomeAlignment &wga,
        size_t offset, BitString &bitstr)
{
    istringstream s(line);
    s.exceptions(istream::failbit | istream::badbit);

    try
    {
        string buf, name;

        // Skip the "s" marker at the beginning of the line.
        s >> buf;
        s >> name;

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
        for (size_t i = 0; i < buf.size(); ++i)
        {
            bitstr.setBit(i + offset, buf[i] != '-');
        }

        seqid_t id = wga.requestSequenceId(name, src_size);

        // We have all we need, create and return the instance.
        return SequenceDetails(start, reverse, src_size, id, offset,
                buf.size());
    }
    catch (std::ios_base::failure &e)
    {
        throw ParseError();
    }
}

AlignmentBlock * ParseMafBlock(const vector<string> &block_lines,
        WholeGenomeAlignment &wga, BitSequenceFactory &factory)
{
    if (block_lines.size() == 0)
    {
        throw ParseError();
    }
    AlignmentBlock *block = new AlignmentBlock();
    size_t block_length = getBlockLength(block_lines[0]);
    size_t total_length = block_lines.size() * block_length + 1;
    BitString bitstr(total_length);
    bitstr.setBit(0, 1);
    try
    {
        size_t offset = 1;
        for (auto it = block_lines.begin(); it != block_lines.end(); ++it)
        {
            SequenceDetails details = parseMafLine(*it, wga,
                    offset, bitstr);
            block->addSequence(details);
            offset += block_length;
        }
    }
    catch (ParseError &e)
    {
        delete block;
        throw;
    }
    block->setBitSequence(factory.getInstance(bitstr));
    return block;
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
        bool can_continue = true;
        // For each block...
        while (can_continue)
        {
            vector<string> block_lines;
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
                    if (passesLimitCheck(buf, limit))
                    {
                        block_lines.push_back(buf);
                    }
                    buf = getNextLine(s);
                }
            }
            catch (std::ios_base::failure &e)
            {
                can_continue = false;
            }
            wga.addBlock(ParseMafBlock(block_lines, wga, factory));
        }
    }
    catch (std::ios_base::failure &e)
    { };
}

} /* namespace maf_reader */
