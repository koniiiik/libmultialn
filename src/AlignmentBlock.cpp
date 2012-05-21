#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <map>
#include <algorithm>
#include <BitString.h>
#include <BitSequence.h>

#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <MultialnConstants.h>


using std::sort;

size_t AlignmentBlock::mapPositionToInformant(const size_t pos,
        seqid_t informant, const IntervalBoundary boundary)
{
    const SequenceDetails *ref = this->getReferenceSequence();
    size_t alignment_pos = ref->sequenceToAlignment(pos);
    const SequenceDetails *inf = this->getSequence(informant);
    return inf->alignmentToSequence(alignment_pos, boundary);
}

const AlignmentBlock::PositionMapping * AlignmentBlock::mapPositionToAll(
        const size_t pos, const IntervalBoundary boundary)
{
    PositionMapping * mapping = new PositionMapping;
    // We need to call this here, before we create an iterator, because
    // prepare() creates a new sequences_ vector which invalidates its
    // iterators.
    this->prepare();
    for (Container::const_iterator it = this->sequences_.begin();
            it != this->sequences_.end(); ++it)
    {
        seqid_t seq_id = it->get_id();
        if (seq_id == kReferenceSequenceId)
            continue;
        try
        {
            size_t pos_inf = this->mapPositionToInformant(pos, seq_id,
                    boundary);
            (*mapping)[seq_id] = pos_inf;
        }
        // If the mapping fails with an exception, we want to silently
        // ignore it.
        catch (OutOfSequence &e)
        { }
    }
    return mapping;
}

const SequenceDetails * AlignmentBlock::getSequence(seqid_t sequence)
{
    if (this->sequences_.empty())
    {
        throw SequenceDoesNotExist();
    }

    this->prepare();
    size_t start = 0, end = this->sequences_.size();
    while ((end - start) > 1)
    {
        size_t middle = (start + end) / 2;
        if (this->sequences_[middle].get_id() <= sequence)
        {
            start = middle;
        }
        else
        {
            end = middle;
        }
    }

    if (this->sequences_[start].get_id() != sequence)
    {
        throw SequenceDoesNotExist();
    }

    return &this->sequences_[start];
}

void AlignmentBlock::prepare()
{
    if (this->prepared_)
    {
        return;
    }

    sort(this->sequences_.begin(), this->sequences_.end(),
            SequenceDetails::compareById);
    // Shrink the vector to its minimal required size.
    vector<SequenceDetails>(this->sequences_).swap(this->sequences_);
    this->prepared_ = true;
}
