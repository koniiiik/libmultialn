#ifndef WHOLEGENOMEALIGNMENT_H
#define WHOLEGENOMEALIGNMENT_H

#include <string>

#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <AlignmentBlockStorage.h>
#include <MultialnConstants.h>


class WholeGenomeAlignment
{
    public:
        WholeGenomeAlignment(const std::string &reference,
                AlignmentBlockStorage * storage):
            reference_(reference), storage_(storage)
        { }
        ~WholeGenomeAlignment();

        const string & get_reference() const
        {
            return reference_;
        }

        void addBlock(AlignmentBlock *block)
        {
            this->storage_->addBlock(block);
        }

        /*
        ** Takes a position in the reference sequence and maps it to a
        ** single specified informant.
        **
        ** Throws OutOfSequence in case the position is not covered by the
        ** alignment.
        **
        ** Throws SequenceDoesNotExist in case the informant does not have
        ** an alignment to this particular position.
        */
        size_t mapPositionToInformant(size_t position,
                const std::string &informant,
                IntervalBoundary boundary=INTERVAL_BEGIN) const;


    private:
        std::string reference_;
        AlignmentBlockStorage * storage_;

        // The following methods are not allowed.
        WholeGenomeAlignment();
        WholeGenomeAlignment(const WholeGenomeAlignment &);
        void operator=(const WholeGenomeAlignment &);
};

#endif /* WHOLEGENOMEALIGNMENT_H */
