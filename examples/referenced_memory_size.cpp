#include <string>
#include <fstream>
#include <istream>
#include <sstream>
#include <ios>

#include "referenced_memory_size.h"

using std::string;
using std::ifstream;
using std::getline;
using std::istringstream;
using std::istream;

inline bool is_memory_mapping(const string &line)
{
    istringstream s(line);
    s.exceptions(istream::failbit | istream::badbit);
    try
    {
        // Try reading the fields to verify format.
        string buf;
        s >> buf; // address range
        s >> buf; // permissions
        s >> buf; // offset
        s >> buf; // device major:minor
        size_t inode;
        s >> inode;
        if (inode == 0)
            return true;
    }
    catch (std::ios_base::failure &e)
    { }
    return false;
}

inline size_t get_field_value(const string &line)
{
    istringstream s(line);
    size_t result;
    string buf;
    s >> buf >> result;
    return result;
}

size_t get_referenced_memory_size()
{
    ifstream smaps("/proc/self/smaps");
    smaps.exceptions(istream::failbit | istream::badbit);
    string line;
    size_t result = 0;
    try
    {
        while (1)
        {
            while (!is_memory_mapping(line))
            {
                getline(smaps, line);
            }
            while (!(line.substr(0, 11) == "Referenced:"))
            {
                getline(smaps, line);
            }
            result += get_field_value(line);
        }
    }
    catch (std::ios_base::failure &e)
    { }

    return result;
}

