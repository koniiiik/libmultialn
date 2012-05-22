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
        /*
        ** The offset is required to be at least 1; a padding of at least
        ** one bit is required at the beginning of the BitSequence.
        */
        SequenceDetails(size_t start, bool reverse, size_t src_size,
                        seqid_t id, size_t offset, size_t block_size):
            start_(start), src_size_(src_size), offset_(offset),
            block_size_(block_size), reverse_(reverse), id_(id),
            sequence_(NULL)
        { }

        /*
        ** Given a position on the whole sequence returns the position in
        ** this alignment. Throws OutOfSequence if the position is not
        ** within this sequence boundaries.
        **
        ** TODO: Add error handling for cases where sequence_ has not yet
        ** been provided.
        */
        size_t sequenceToAlignment(size_t index) const;

        /*
        ** Given a position in this alignment finds the appropriate
        ** position in this alignment. Depending on whether we are
        ** searching for the beginning or the end of an interval, we
        ** find the first filled position in this alignment to the right
        ** or to the left respectively.
        **
        ** TODO: Add error handling for cases where sequence_ has not yet
        ** been provided.
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
        **
        ** TODO: Add error handling for cases where sequence_ has not yet
        ** been provided.
        */
        size_t get_size() const
        {
            return this->sequence_->rank1(this->offset_ + this->block_size_ - 1)
                - this->sequence_->rank1(this->offset_ - 1);
        }
        size_t get_src_size() const
        {
            return this->src_size_;
        }
        seqid_t get_id() const
        {
            return this->id_;
        }
        /*
        ** This doesn't give the instance ownership of the bit sequence;
        ** it is meant to be shared with other SequenceDetails instances.
        */
        void set_sequence(cds_static::BitSequence *sequence)
        {
            this->sequence_ = sequence;
        }

        static bool compareById(const SequenceDetails &d1,
                const SequenceDetails &d2)
        {
            return d1.get_id() < d2.get_id();
        }


    private:
        // position in the source sequence, size of the original sequence,
        // offset in the bit sequence and length of the block
        size_t start_, src_size_, offset_, block_size_;
        // true if from reverse-complement source
        bool reverse_;
        seqid_t id_;
        cds_static::BitSequence *sequence_;
};

#endif /* SEQUENCEDETAILS_H */
