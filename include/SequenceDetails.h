#include <string>
#include <BitSequence.h>


#ifndef SEQUENCEDETAILS_H
#define SEQUENCEDETAILS_H

class SequenceDetails
{
    private:
        // position in the source sequence, size of this subsequence and
        // the size of the original sequence
        size_t start, size, src_size;
        // true if from reverse-complement source
        bool reverse;
        cds_static::BitSequence * sequence;


    public:
        SequenceDetails(size_t _start, size_t _size, bool _reverse,
                        size_t _src_size,
                        cds_static::BitSequence * bseq):
            start(_start), size(_size), reverse(_reverse),
            src_size(_src_size), sequence(bseq)
            {};
        ~SequenceDetails();

        /*
        ** Given a position on the whole sequence returns the position in
        ** this alignment.
        */
        size_t sequenceToAlignment(size_t index) const;

        /*
        ** Given a position in this alignment finds the first position
        ** in the whole sequence occurring right of or at the given
        ** position in this alignment.
        */
        size_t alignmentToSequenceBegin(size_t index) const;

        /*
        ** Given a position in this alignment finds the first position
        ** in the whole sequence occurring left of or at the given
        ** position in this alignment.
        */
        size_t alignmentToSequenceEnd(size_t index) const;

        size_t rank(size_t index) const;
        size_t select(size_t index) const;
};

#endif /* SEQUENCEDETAILS_H */
