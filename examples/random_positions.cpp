#include <string>
#include <vector>
#include <ctime>
#include <iostream>
#include <utility>

#include <MafReader.h>
#include <WholeGenomeAlignment.h>
#include <BinSearchAlignmentBlockStorage.h>
#include <BitSequenceFactory.h>


using std::string;
using std::vector;
using std::clock;
using std::max;
using std::min;
using std::pair;

string progname;

void usage()
{
    cerr << "Usage: " << progname << " <file.maf> <reference> "
        "<percentage> RG2|RG3|RG4|RG20|RRR|SDArray" << endl;
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

inline double clock_to_sec(clock_t time_interval)
{
    return time_interval / (double)CLOCKS_PER_SEC;
}

int main(int argc, char **argv)
{
    progname = argv[0];
    if (argc != 5)
    {
        usage();
    }

    //string file_name = argv[1], reference = argv[2];
    int percentage = atoi(argv[3]);
    if (percentage <= 0 || percentage > 100)
    {
        usage();
    }

    clock_t start = clock();
    WholeGenomeAlignment wga(argv[2],
            new BinSearchAlignmentBlockStorage());

    {
        BitSequenceFactory * factory = GetSequenceFactory(argv[4]);

        maf_reader::ReadMafFile(argv[1], wga, *factory);

        delete factory;
    }
    clock_t end = clock();
    cerr.precision(10);
    cerr << "Parsed MAF in " << clock_to_sec(end - start) <<
            " seconds." << endl;

    size_t reference_size = wga.getReferenceSize();
    size_t requested_attempts = (int)(reference_size * percentage / 100.0);

    // We need to prepae a list of (informant, position) pairs that are
    // guaranteed not to miss to be able to measure the performance of
    // actual hits.
    // We select the requested number of pairs by trying random positions.
    vector<pair<string, size_t> > safe_maps;
    safe_maps.reserve(requested_attempts + 47);

    size_t misses = 0;
    while (safe_maps.size() < requested_attempts)
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
            safe_maps.push_back(make_pair(mapping->begin()->first,
                        position));
            delete mapping;
        }
        catch (OutOfSequence &e)
        {
            ++misses;
        }
    }
    cerr << "Gathered " << requested_attempts << "; missed " << misses << endl;

    start = clock();
    for (size_t attempt = 0; attempt < safe_maps.size(); ++attempt)
    {
        try
        {
            wga.mapPositionToInformant(safe_maps[attempt].second,
                    safe_maps[attempt].first);
        }
        catch (OutOfSequence &e)
        {
            cout << "OutOfSequence called on attempt " << attempt << endl;
        }
    }
    end = clock();

    double seconds = clock_to_sec(end - start);
    cerr << "Attempts:\t" << requested_attempts << endl;
    cerr << "Total secs:\t" << seconds << endl;
    cerr << "Secs per attempt:\t" << seconds / requested_attempts << endl;
}
