#include <string>
#include <sstream>
#include <cassert>
#include <map>
#include <BitString.h>
#include <BitSequence.h>

#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <MultialnConstants.h>


/*
** Public methods
*/

AlignmentBlock::AlignmentBlock(const AlignmentBlock &ab)
{
    this->copySequences(ab.sequences_);
}

AlignmentBlock::~AlignmentBlock()
{
    this->clearSequences();
}

size_t AlignmentBlock::mapPositionToInformant(const size_t pos,
        seqid_t informant, const IntervalBoundary boundary) const
{
    const SequenceDetails *ref = this->getReferenceSequence();
    size_t alignment_pos = ref->sequenceToAlignment(pos);
    const SequenceDetails *inf = this->getSequence(informant);
    return inf->alignmentToSequence(alignment_pos, boundary);
}

const AlignmentBlock::Mapping * AlignmentBlock::mapPositionToAll(const size_t pos,
        const IntervalBoundary boundary) const
{
    Mapping * mapping = new Mapping;
    for (Container::const_iterator it = this->sequences_.begin();
            it != this->sequences_.end(); ++it)
    {
        if (it->first == kReferenceSequenceId)
            continue;
        try
        {
            size_t pos_inf = this->mapPositionToInformant(pos, it->first,
                    boundary);
            (*mapping)[it->first] = pos_inf;
        }
        // If the mapping fails with an exception, we want to silently
        // ignore it.
        catch (OutOfSequence &e)
        { }
    }
    return mapping;
}

const SequenceDetails * AlignmentBlock::getSequence(seqid_t sequence) const
{
    Container::const_iterator it = this->sequences_.find(sequence);
    if (it == this->sequences_.end())
    {
        throw SequenceDoesNotExist();
    }
    return it->second;
}

void AlignmentBlock::clearSequences()
{
    for (Container::iterator it = this->sequences_.begin();
            it != this->sequences_.end(); ++it)
    {
        delete it->second;
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
        it->second = new SequenceDetails(*(it->second));
    }
}
