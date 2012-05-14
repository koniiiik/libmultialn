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

    vector<string> *seqnames = wga.getSequenceList();
    size_t reference_size = wga.getReferenceSize();
    size_t tries_per_sequence = 0;
    size_t reference_misses = 0;
    double total_time = 0;

    double step = 100 / (double)percentage;

    for (size_t seq_no = 1; seq_no < seqnames->size(); ++seq_no)
    {
        start = clock();
        size_t misses = 0, reference_misses_loc = 0, try_no = 0;
        for (; (size_t)(try_no * step) < reference_size; ++try_no)
        {
            size_t position = (size_t)(try_no * step);
            try
            {
                wga.mapPositionToInformant(position, seqnames->at(seq_no));
            }
            catch (OutOfSequence &e)
            {
                ++reference_misses_loc;
            }
            catch (SequenceDoesNotExist &e)
            {
                ++misses;
            }
        }
        end = clock();
        double sequence_time = clock_to_sec(end - start);
        cerr << seqnames->at(seq_no) << "\t" << misses << "\t" <<
                sequence_time << endl;
        total_time += sequence_time;
        reference_misses = reference_misses_loc;
        tries_per_sequence = try_no;
    }

    cerr << "Total time:\t" << total_time << endl;
    cerr << "Tries per seq:\t" << tries_per_sequence << endl;
    cerr << "Average time per try:\t" << total_time / (tries_per_sequence
            * (seqnames->size() - 1)) << endl;
    cerr << "Reference misses:\t" << reference_misses << endl;
}
