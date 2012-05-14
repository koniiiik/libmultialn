#include <string>
#include <ctime>
#include <iostream>

#include <BitSequence.h>
#include <BitString.h>
#include <BitSequenceFactory.h>


using std::string;
using std::clock;
using cds_static::BitSequence;
using cds_utils::BitString;

string progname;

void usage()
{
    cerr << "Usage: " << progname << " <file> RG2|RG3|RG4|RG20|RRR|SDArray" << endl;
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

int main(int argc, char **argv)
{
    progname = argv[0];
    if (argc != 3)
    {
        usage();
    }

    BitSequence * seq = NULL;

    // Parse the input file...
    {
        BitSequenceFactory * factory = GetSequenceFactory(argv[2]);

        ifstream s(argv[1]);
        string buf;
        getline(s, buf);

        BitString bstr(buf.size());

        for (size_t i = 0; i < buf.size(); ++i)
        {
            bstr.setBit(i, buf[i] != '-');
        }

        seq = factory->getInstance(bstr);

        delete factory;
    }

    cout << seq->getSize() << endl;

    delete seq;
}
