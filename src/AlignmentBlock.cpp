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

AlignmentBlock::AlignmentBlock(const AlignmentBlock &ab):
    prepared_(false)
{
    this->copySequences(ab.sequences_);
}

AlignmentBlock::~AlignmentBlock()
{
    this->clearSequences();
}

AlignmentBlock & AlignmentBlock::operator=(const AlignmentBlock &other)
{
    this->clearSequences();
    this->copySequences(other.sequences_);
    this->prepared_ = false;
    return (*this);
}

size_t AlignmentBlock::mapPositionToInformant(const size_t pos,
        seqid_t informant, const IntervalBoundary boundary)
{
    const SequenceDetails *ref = this->getReferenceSequence();
    size_t alignment_pos = ref->sequenceToAlignment(pos);
    const SequenceDetails *inf = this->getSequence(informant);
    return inf->alignmentToSequence(alignment_pos, boundary);
}

const AlignmentBlock::Mapping * AlignmentBlock::mapPositionToAll(const size_t pos,
        const IntervalBoundary boundary)
{
    Mapping * mapping = new Mapping;
    for (Container::const_iterator it = this->sequences_.begin();
            it != this->sequences_.end(); ++it)
    {
        seqid_t seq_id = (*it)->get_id();
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
        if (this->sequences_[middle]->get_id() <= sequence)
        {
            start = middle;
        }
        else
        {
            end = middle;
        }
    }

    if (this->sequences_[start]->get_id() != sequence)
    {
        throw SequenceDoesNotExist();
    }

    return this->sequences_[start];
}

void AlignmentBlock::prepare()
{
    if (this->prepared_)
    {
        return;
    }

    sort(this->sequences_.begin(), this->sequences_.end(),
            SequenceDetails::compareById);
    this->prepared_ = true;
}

void AlignmentBlock::clearSequences()
{
    for (Container::iterator it = this->sequences_.begin();
            it != this->sequences_.end(); ++it)
    {
        delete *it;
    }
    this->sequences_.clear();
}

void AlignmentBlock::copySequences(const Container &sequences)
{
    this->sequences_ = sequences;
    // We need to explicitly copy each SequenceDetails instance stored in
    // the sequences_ map.
    for (Container::iterator it = this->sequences_.begin();
            it != this->sequences_.end(); ++it)
    {
        *it = new SequenceDetails(**it);
    }
}
