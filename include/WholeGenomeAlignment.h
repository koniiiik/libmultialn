#ifndef WHOLEGENOMEALIGNMENT_H
#define WHOLEGENOMEALIGNMENT_H

#include <string>
#include <tuple>

#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <AlignmentBlockStorage.h>
#include <MultialnConstants.h>


class WholeGenomeAlignment
{
    public:
        WholeGenomeAlignment(const std::string &reference,
                AlignmentBlockStorage * storage);
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

        /*
        ** Returns the size of the reference sequence.
        */
        size_t getReferenceSize() const;

        /*
        ** Returns the ID associated to the specified sequence name. If
        ** none exists, throws SequenceDoesNotExist.
        */
        seqid_t getSequenceId(const std::string &name) const;

        /*
        ** Requests for a new ID for the specified sequence. If the
        ** sequence already has an ID associated, returns that one,
        ** otherwise returns a new unique one.
        **
        ** TODO: handle the case where all IDs are taken
        */
        seqid_t requestSequenceId(const std::string &name);

        size_t countKnownSequences() const;


    private:
        std::map<seqid_t, std::tuple<std::string> > sequence_details_map_;
        std::map<std::string, seqid_t> sequence_name_map_;
        std::string reference_;
        AlignmentBlockStorage * storage_;

        // The following methods are not allowed.
        WholeGenomeAlignment();
        WholeGenomeAlignment(const WholeGenomeAlignment &);
        void operator=(const WholeGenomeAlignment &);
};

#endif /* WHOLEGENOMEALIGNMENT_H */
