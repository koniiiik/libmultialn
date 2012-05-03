#ifndef SEQUENCEDETAILS_H
#define SEQUENCEDETAILS_H

#include <string>
#include <memory>
#include <exception>

#include <BitSequence.h>
#include <MultialnConstants.h>


class OutOfSequence: public std::exception
{
    public:
        OutOfSequence() throw(): exception() {};
        OutOfSequence(const OutOfSequence &other) throw():
            exception(other)
        { }
};

class SequenceDetails
{
    public:
        SequenceDetails(size_t start, size_t size, bool reverse,
                        size_t src_size, seqid_t id,
                        cds_static::BitSequence *sequence):
            start_(start), size_(size), src_size_(src_size),
            reverse_(reverse), id_(id), sequence_(sequence)
        { }

        /*
        ** Given a position on the whole sequence returns the position in
        ** this alignment. Throws OutOfSequence if the position is not
        ** within this sequence boundaries.
        */
        size_t sequenceToAlignment(size_t index) const;

        /*
        ** Given a position in this alignment finds the appropriate
        ** position in this alignment. Depending on whether we are
        ** searching for the beginning or the end of an interval, we
        ** find the first filled position in this alignment to the right
        ** or to the left respectively.
        */
        size_t alignmentToSequence(size_t index,
                IntervalBoundary boundary=INTERVAL_BEGIN) const;

        size_t rank(size_t index) const;
        size_t select(size_t index) const;

        size_t get_start() const
        {
            return this->start_;
        }
        size_t get_size() const
        {
            return this->size_;
        }
        size_t get_src_size() const
        {
            return this->src_size_;
        }
        seqid_t get_id() const
        {
            return this->id_;
        }

        static bool compareById(const SequenceDetails *d1,
                const SequenceDetails *d2)
        {
            return d1->get_id() < d2->get_id();
        }


    private:
        // position in the source sequence, size of this subsequence and
        // the size of the original sequence
        size_t start_, size_, src_size_;
        // true if from reverse-complement source
        bool reverse_;
        seqid_t id_;
        std::shared_ptr<cds_static::BitSequence> sequence_;
};

#endif /* SEQUENCEDETAILS_H */
