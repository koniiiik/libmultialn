#include <string>

#include <SequenceDetails.h>


size_t SequenceDetails::sequenceToAlignment(size_t index) const
{
    return this->sequence->select1(index - this->start + 1);
}

size_t SequenceDetails::alignmentToSequenceBegin(size_t index) const
{
    size_t next1 = index;
    if (!this->sequence->access(index))
    {
        next1 = this->sequence->selectNext1(index);
    }
    return this->sequence->rank1(next1) + this->start - 1;
}

size_t SequenceDetails::alignmentToSequenceEnd(size_t index) const
{
    size_t prev1 = index;
    if (!this->sequence->access(index))
    {
        prev1 = this->sequence->selectPrev1(index);
    }
    return this->sequence->rank1(prev1) + this->start - 1;
}

size_t SequenceDetails::rank(size_t index) const
{
    return this->sequence->rank1(index);
}

size_t SequenceDetails::select(size_t index) const
{
    return this->sequence->select1(index);
}
