/*
** This smaple program loads a MAF file according to given parameters,
** maps the positions given on stdin and prints timing statistics.
*/

#include <sys/time.h>
#include <sys/resource.h>

#include <string>
#include <vector>
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
using std::clock;
using std::max;
using std::min;
using std::pair;

string progname;

void usage()
{
    cerr << "Usage: " << progname << " <file.maf> <reference> "
        "RG2|RG3|RG4|RG20|RRR|SDArray binsearch|rank" << endl;
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
    if (argc != 5)
    {
        usage();
    }

    clock_t start = clock();
    struct rusage self_usage;
    getrusage(RUSAGE_SELF, &self_usage);
    size_t rss_before = self_usage.ru_maxrss;
    WholeGenomeAlignment wga(argv[2], GetAlignmentBlockStorage(argv[4]));

    {
        BitSequenceFactory * factory = GetSequenceFactory(argv[3]);

        maf_reader::ReadMafFile(argv[1], wga, *factory);

        delete factory;
    }
    clock_t end = clock();
    cerr.precision(10);
    cerr << "Parsed MAF in " << clock_to_sec(end - start) <<
            " seconds." << endl;

    size_t misses = 0, attempts = 0;
    start = clock();
    string informant;
    size_t position;
    while (cin >> informant >> position)
    {
        ++attempts;
        try
        {
            wga.mapPositionToInformant(position, informant);
        }
        catch (OutOfSequence &e)
        {
            ++misses;
        }
        catch (SequenceDoesNotExist &e)
        {
            ++misses;
        }
    }
    end = clock();

    getrusage(RUSAGE_SELF, &self_usage);
    size_t rss_diff = self_usage.ru_maxrss;
    rss_diff -= rss_before;

    double seconds = clock_to_sec(end - start);
    cerr << "Attempts:\t" << attempts << endl;
    cerr << "Missed:\t" << misses << endl;
    cerr << "Total secs:\t" << seconds << endl;
    cerr << "Secs per attempt:\t" << seconds / attempts << endl;
    cerr << "Resident-set diff:\t" << rss_diff << " kb" << endl;
}
