#include <string>

#include <SequenceDetails.h>


size_t SequenceDetails::sequenceToAlignment(size_t index) const
{
    return this->sequence_->select1(index - this->start_ + 1);
}

size_t SequenceDetails::alignmentToSequenceBegin(size_t index) const
{
    size_t next1 = index;
    if (!this->sequence_->access(index))
    {
        next1 = this->sequence_->selectNext1(index);
    }
    return this->sequence_->rank1(next1) + this->start_ - 1;
}

size_t SequenceDetails::alignmentToSequenceEnd(size_t index) const
{
    size_t prev1 = index;
    if (!this->sequence_->access(index))
    {
        prev1 = this->sequence_->selectPrev1(index);
    }
    return this->sequence_->rank1(prev1) + this->start_ - 1;
}

size_t SequenceDetails::rank(size_t index) const
{
    return this->sequence_->rank1(index);
}

size_t SequenceDetails::select(size_t index) const
{
    return this->sequence_->select1(index);
}
