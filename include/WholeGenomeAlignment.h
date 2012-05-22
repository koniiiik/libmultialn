#ifndef WHOLEGENOMEALIGNMENT_H
#define WHOLEGENOMEALIGNMENT_H

#include <string>
#include <tuple>
#include <vector>
#include <map>

#include <AlignmentBlock.h>
#include <SequenceDetails.h>
#include <AlignmentBlockStorage.h>
#include <MultialnConstants.h>

// forward declaration
namespace cds_static
{
class BitSequence;
}

class MappingDoesNotExist: public std::exception
{
    public:
        MappingDoesNotExist() throw(): exception() {};
        MappingDoesNotExist(const MappingDoesNotExist &other) throw():
            exception(other)
        { }
};

class WholeGenomeAlignment
{
    public:
        typedef std::map<std::string, size_t> PositionMapping;

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
        ** Takes a position in the reference sequence and maps it to all
        ** informants possible.
        **
        ** Throws OutOfSequence in case the position is not covered by the
        ** alignment.
        */
        PositionMapping * mapPositionToAll(size_t position,
                IntervalBoundary boundary=INTERVAL_BEGIN) const;

        /*
        ** Maps a region on the forward strand of the reference sequence
        ** to a region on the given informant.
        **
        ** Throws MappingDoesNotExist in case the mapped region is not
        ** colinear to the reference region.
        **
        ** TODO: make the exceptions thrown at least a little bit sane
        */
        std::pair<size_t, size_t> mapRegionToInformant(size_t region_start,
                size_t region_end, const std::string &informant);

        /*
        ** Returns the size of the specified sequence. Throws
        ** SequenceDoesNotExist in case the sequence ID is invalid.
        */
        size_t getSequenceSize(seqid_t sequence) const;
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
        ** Also sets the size of the sequence in the alignment's global
        ** sequence information cache.
        **
        ** TODO: handle the case where all IDs are taken
        */
        seqid_t requestSequenceId(const std::string &name, size_t size);

        /*
        ** Given a sequence ID, returns a reference to a string containing
        ** its name.
        **
        ** Throws SequenceDoesNotExist in case the ID is unknown.
        */
        const std::string & getSequenceName(seqid_t id) const;

        size_t countKnownSequences() const;

        /*
        ** Returns a vector containing the names of all sequences
        ** contained within this alignment. The first one is reference,
        ** others are in the order in which they have been registered with
        ** requestSequenceId.
        */
        std::vector<std::string> * getSequenceList() const;

        /*
        ** Provides the instance of BitSequence to be used by this
        ** alignment class. This is passed on to each AlignmentBlock which
        ** in turn passes it to its SequenceDetails.
        */
        void setBitSequence(cds_static::BitSequence *bit_sequence);


    private:
        // This maps sequence IDs to tuples (name, total length).
        std::map<seqid_t, std::tuple<std::string, size_t> > sequence_details_map_;
        std::map<std::string, seqid_t> sequence_name_map_;
        std::string reference_;
        AlignmentBlockStorage *storage_;
        cds_static::BitSequence *bit_sequence_;

        // The following methods are not allowed.
        WholeGenomeAlignment();
        WholeGenomeAlignment(const WholeGenomeAlignment &);
        void operator=(const WholeGenomeAlignment &);
};

#endif /* WHOLEGENOMEALIGNMENT_H */
