/*
** This sample program loads a MAF file and generates an endless stream of
** pairs (informant, reference position) that are guaranteed to map
** correctly.
*/
#include <string>
#include <vector>
#include <set>
#include <ctime>
#include <iostream>
#include <utility>

#include <MafReader.h>
#include <WholeGenomeAlignment.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <RankAlignmentBlockStorage.h>
#include <BitSequenceFactory.h>


using std::string;
using std::vector;
using std::set;
using std::clock;
using std::max;
using std::min;
using std::pair;
using maf_reader::ReadMafFile;

string progname;

void usage()
{
    cerr << "Usage: " << progname << " <file.maf> <reference> "
        "RG2|RG3|RG4|RG20|RRR|SDArray binsearch|rank "
        "[seqname seqname ...]" << endl;
    exit(1);
}

BitSequenceFactory * GetSequenceFactory(const string &param)
{
    if (param == "RRR")
        return new BitSequenceRRRFactory();
    if (param == "SDArray")
        return new BitSequenceSDArrayFactory();
    if (param == "RG3")
        return new BitSequenceRGFactory(3);
    if (param == "RG4")
        return new BitSequenceRGFactory(4);
    if (param == "RG20")
        return new BitSequenceRGFactory(20);
    return new BitSequenceRGFactory(2);
}

AlignmentBlockStorage * GetAlignmentBlockStorage(const string &param)
{
    if (param == "binsearch")
        return new BinSearchAlignmentBlockStorage();
    return new RankAlignmentBlockStorage();
}

inline double clock_to_sec(clock_t time_interval)
{
    return time_interval / (double)CLOCKS_PER_SEC;
}

int main(int argc, char **argv)
{
    progname = argv[0];
    if (argc < 5)
    {
        usage();
    }

    clock_t start = clock();
    WholeGenomeAlignment wga(argv[2], GetAlignmentBlockStorage(argv[4]));

    {
        set<string> filter;
        filter.insert(argv[2]);
        for (int i = 5; i < argc; ++i)
        {
            filter.insert(argv[i]);
        }

        BitSequenceFactory * factory = GetSequenceFactory(argv[3]);

        if (filter.size() > 1)
        {
            ReadMafFile(argv[1], wga, *factory, &filter);
        }
        else
        {
            ReadMafFile(argv[1], wga, *factory);
        }

        delete factory;
    }
    clock_t end = clock();
    cerr.precision(10);
    cerr << "Parsed MAF in " << clock_to_sec(end - start) <<
            " seconds." << endl;

    size_t reference_size = wga.getReferenceSize();

    while (1)
    {
        size_t position = rand() % reference_size;
        try
        {
            WholeGenomeAlignment::PositionMapping *mapping;
            mapping = wga.mapPositionToAll(position);
            if (mapping->begin() == mapping->end())
            {
                delete mapping;
                throw OutOfSequence();
            }
            cout << mapping->begin()->first << "\t" << position << endl;
            delete mapping;
        }
        catch (OutOfSequence &e)
        { }
    }
}
