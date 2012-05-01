#ifndef MAFREADER_H
#define MAFREADER_H

#include <string>
#include <set>
#include <istream>


class WholeGenomeAlignment;
class BitSequenceFactory;

namespace maf_reader
{

class ParseError: public std::exception
{
    public:
        ParseError() throw(): exception() {};
        ParseError(const ParseError &other) throw():
            exception(other)
        { }
};

/*
** Reads a MAF file from stream s and fills WholeGenomeAlignment wga with
** its contents. factory specifies the implementation of BitSequence to be
** used and the optional limit parameter specifies which sequences
** (including reference) should be taken into consideration, ignoring the
** rest.
*/
void ReadMafFile(std::istream &s, WholeGenomeAlignment &wga,
        BitSequenceFactory &factory, const std::set<std::string> *limit=NULL);

/*
** Reads a MAF file specified by file_name and fills WholeGenomeAlignment
** wga with its contents. factory specifies the implementation of
** BitSequence to be used and the optional limit parameter specifies which
** sequences (including reference) should be taken into consideration,
** ignoring the rest.
*/
void ReadMafFile(const std::string &file_name, WholeGenomeAlignment &wga,
        BitSequenceFactory &factory, const std::set<std::string> *limit=NULL);


} /* namespace maf_reader */

#endif /* MAFREADER_H */
