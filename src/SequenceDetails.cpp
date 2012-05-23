#include <string>

#include <SequenceDetails.h>
#include <MultialnConstants.h>


size_t SequenceDetails::sequenceToAlignment(size_t index) const
{
    // If the range is taken from the reverse strand, convert the
    // coordinate to the forward strand's system.
    if (this->reverse_)
    {
        index = this->get_src_size() - index - 1;
    }
    if (index < this->start_ || index >= (this->start_ + this->get_size()))
    {
        throw OutOfSequence();
    }
    return this->sequence_->select1(index - this->start_ + 1);
}

size_t SequenceDetails::alignmentToSequence(size_t index,
        IntervalBoundary boundary) const
{
    if (index >= this->sequence_->getLength())
    {
        throw OutOfSequence();
    }
    size_t rank = this->sequence_->rank1(index);
    if (!this->sequence_->access(index))
    {
        if (boundary == INTERVAL_BEGIN)
        {
            ++rank;
        }
        // rank can be 0 iff boundary is INTERVAL_END and the sought
        // position is before our block.
        if (rank == 0 || rank > this->get_size())
        {
            throw OutOfSequence();
        }
    }
    size_t position = rank + this->start_ - 1;
    // At this point, the position is in the coordinate system of the
    // strand. Normalize to the forward strand.
    if (this->reverse_)
    {
        position = this->get_src_size() - position - 1;
    }
    return position;
}
