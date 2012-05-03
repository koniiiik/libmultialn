#ifndef ALIGNMENTBLOCK_H
#define ALIGNMENTBLOCK_H

#include <map>
#include <string>
#include <exception>

#include <SequenceDetails.h>
#include <MultialnConstants.h>


class SequenceDoesNotExist: public std::exception
{
    public:
        SequenceDoesNotExist() throw(): exception() {};
        SequenceDoesNotExist(const SequenceDoesNotExist &other) throw():
            exception(other)
        { }
};

class AlignmentBlock
{
    public:
        typedef std::map<seqid_t, size_t> Mapping;

        AlignmentBlock()
        { }
        AlignmentBlock(const AlignmentBlock &ab);
        ~AlignmentBlock();

        AlignmentBlock & operator=(const AlignmentBlock &other)
        {
            this->clearSequences();
            this->copySequences(other.sequences_);
            return (*this);
        }

        /*
        ** Takes a position in the reference sequence and maps it to a
        ** single specified informant.
        **
        ** Throws OutOfSequence in case the position can't be mapped
        ** within this block.
        **
        ** Throws SequenceDoesNotExist in case the informant is not
        ** present in this block.
        */
        size_t mapPositionToInformant(const size_t pos, seqid_t informant,
                const IntervalBoundary boundary=INTERVAL_BEGIN) const;
        /*
        ** Takes a position in the reference sequence and maps it to those
        ** informants where such mapping is possible.
        **
        ** Throws OutOfSequence in case the position is not present in the
        ** reference sequence.
        */
        const Mapping * mapPositionToAll(const size_t pos,
                const IntervalBoundary boundary=INTERVAL_BEGIN) const;

        /*
        ** Returns the specified sequence. Throws SequenceDoesNotExist if
        ** not present.
        */
        const SequenceDetails * getSequence(seqid_t sequence) const;
        /*
        ** Returns the reference sequence. Throws SequenceDoesNotExist if
        ** the sequence has not yet been added to this block.
        */
        const SequenceDetails * getReferenceSequence() const
        {
            return this->getSequence(kReferenceSequenceId);
        }

        void addSequence(seqid_t id, const SequenceDetails *details)
        {
            this->sequences_[id] = details;
        }

        static bool compareReferencePosition(const AlignmentBlock *a,
                const AlignmentBlock *b)
        {
            return (a->getReferenceSequence()->get_start()
                    < b->getReferenceSequence()->get_start());
        }


    private:
        typedef std::map<seqid_t, const SequenceDetails *> Container;
        Container sequences_;

        void clearSequences();
        void copySequences(const Container &sequences);
};

#endif /* ALIGNMENTBLOCK_H */
