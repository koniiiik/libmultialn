#include <string>
#include <sstream>
#include <cassert>
#include <map>
#include <BitString.h>
#include <BitSequence.h>

#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <MultialnConstants.h>

#ifndef RG_SEQUENCE_FACTOR
    #define RG_SEQUENCE_FACTOR 20
#endif


/*
** Public methods
*/

AlignmentBlock::AlignmentBlock(const AlignmentBlock &ab):
    reference_name_(ab.reference_name_)
{
    this->copySequences(ab.sequences_);
}

AlignmentBlock::~AlignmentBlock()
{
    this->clearSequences();
}

const size_t AlignmentBlock::mapPositionToInformant(const size_t pos,
        const std::string &informant, const IntervalBoundary boundary) const
{
    const SequenceDetails *ref = this->getReferenceSequence();
    size_t alignment_pos = ref->sequenceToAlignment(pos);
    const SequenceDetails *inf = this->getSequence(informant);
    return inf->alignmentToSequence(alignment_pos, boundary);
}

const SequenceDetails * AlignmentBlock::getSequence(const std::string &name) const
{
    Container::const_iterator it = this->sequences_.find(name);
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
