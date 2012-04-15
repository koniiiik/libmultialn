#include <string>

#include <SequenceDetails.h>
#include <MultialnConstants.h>


size_t SequenceDetails::sequenceToAlignment(size_t index) const
{
    return this->sequence_->select1(index - this->start_ + 1);
}

size_t SequenceDetails::alignmentToSequence(size_t index,
        IntervalBoundary boundary) const
{
    size_t wanted = index;
    if (!this->sequence_->access(index))
    {
        if (boundary == INTERVAL_BEGIN)
        {
            wanted = this->sequence_->selectNext1(index);
        }
        else
        {
            wanted = this->sequence_->selectPrev1(index);
        }
    }
    return this->sequence_->rank1(wanted) + this->start_ - 1;
}

size_t SequenceDetails::rank(size_t index) const
{
    return this->sequence_->rank1(index);
}

size_t SequenceDetails::select(size_t index) const
{
    return this->sequence_->select1(index);
}
