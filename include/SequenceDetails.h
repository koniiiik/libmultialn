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
        SequenceDetails(size_t start, bool reverse, size_t src_size,
                        seqid_t id, cds_static::BitSequence *sequence):
            start_(start), src_size_(src_size), reverse_(reverse),
            id_(id), sequence_(sequence)
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

        /*
        ** Returns the position of the first nucleotide in this region,
        ** normalised to the forward strand's coordinate system.
        */
        size_t get_start() const
        {
            if (this->reverse_)
            {
                return this->get_src_size() - this->start_ - 1;
            }
            return this->start_;
        }
        /*
        ** Returns the position of the last nucleotide in this region,
        ** normalised to the forward strand's coordinate system.
        */
        size_t get_end() const
        {
            if (this->reverse_)
            {
                return this->get_start() - this->get_size() + 1;
            }
            return this->get_start() + this->get_size() - 1;
        }
        /*
        ** Returns the length of the region covered by this sequence. This
        ** equals the number of ones in our part of the bit sequence.
        */
        size_t get_size() const
        {
            return this->sequence_->countOnes();
        }
        size_t get_src_size() const
        {
            return this->src_size_;
        }
        seqid_t get_id() const
        {
            return this->id_;
        }

        static bool compareById(const SequenceDetails &d1,
                const SequenceDetails &d2)
        {
            return d1.get_id() < d2.get_id();
        }


    private:
        // position in the source sequence and the size of the original
        // sequence
        size_t start_, src_size_;
        // true if from reverse-complement source
        bool reverse_;
        seqid_t id_;
        std::shared_ptr<cds_static::BitSequence> sequence_;
};

#endif /* SEQUENCEDETAILS_H */
