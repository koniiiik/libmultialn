#ifndef ALIGNMENTBLOCK_H
#define ALIGNMENTBLOCK_H

#include <vector>
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

        AlignmentBlock():
            prepared_(false)
        { }
        AlignmentBlock(const AlignmentBlock &ab);
        ~AlignmentBlock();

        AlignmentBlock & operator=(const AlignmentBlock &other);

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
                const IntervalBoundary boundary=INTERVAL_BEGIN);
        /*
        ** Takes a position in the reference sequence and maps it to those
        ** informants where such mapping is possible.
        **
        ** Throws OutOfSequence in case the position is not present in the
        ** reference sequence.
        */
        const Mapping * mapPositionToAll(const size_t pos,
                const IntervalBoundary boundary=INTERVAL_BEGIN);

        /*
        ** Returns the specified sequence. Throws SequenceDoesNotExist if
        ** not present.
        */
        const SequenceDetails * getSequence(seqid_t sequence);
        /*
        ** Returns the reference sequence. Throws SequenceDoesNotExist if
        ** the sequence has not yet been added to this block.
        */
        const SequenceDetails * getReferenceSequence()
        {
            return this->getSequence(kReferenceSequenceId);
        }

        void addSequence(const SequenceDetails *details)
        {
            this->prepared_ = false;
            this->sequences_.push_back(details);
        }

        static bool compareReferencePosition(AlignmentBlock *a,
                AlignmentBlock *b)
        {
            return (a->getReferenceSequence()->get_start()
                    < b->getReferenceSequence()->get_start());
        }


    private:
        typedef std::vector<const SequenceDetails *> Container;
        Container sequences_;
        bool prepared_;

        void prepare();
        void clearSequences();
        void copySequences(const Container &sequences);
};

#endif /* ALIGNMENTBLOCK_H */
