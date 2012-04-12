#include <string>
#include <memory>

#include <BitSequence.h>


#ifndef SEQUENCEDETAILS_H
#define SEQUENCEDETAILS_H

class SequenceDetails
{
    public:
        SequenceDetails(size_t start, size_t size, bool reverse,
                        size_t src_size,
                        cds_static::BitSequence *sequence):
            start_(start), size_(size), reverse_(reverse),
            src_size_(src_size), sequence_(sequence)
        { }

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

        size_t get_start() const
        {
            return this->start_;
        }


    private:
        // position in the source sequence, size of this subsequence and
        // the size of the original sequence
        size_t start_, size_, src_size_;
        // true if from reverse-complement source
        bool reverse_;
        std::shared_ptr<cds_static::BitSequence> sequence_;
};

#endif /* SEQUENCEDETAILS_H */
