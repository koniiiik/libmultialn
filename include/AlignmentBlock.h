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
        typedef std::map<std::string, size_t> Mapping;

        AlignmentBlock(const std::string *reference_name):
            reference_name_(reference_name)
        { }
        AlignmentBlock(const AlignmentBlock &ab);
        ~AlignmentBlock();

        AlignmentBlock & operator=(const AlignmentBlock &other)
        {
            this->clearSequences();
            this->copySequences(other.sequences_);
        }

        const size_t mapPositionToInformant(const size_t pos,
                const std::string &informant,
                const IntervalBoundary boundary=INTERVAL_BEGIN) const;
        const Mapping * mapPositionToAll(const size_t pos,
                const IntervalBoundary boundary=INTERVAL_BEGIN) const;

        const SequenceDetails * getSequence(const std::string &name) const;
        const SequenceDetails * getReferenceSequence() const
        {
            return this->getSequence(*(this->reference_name_));
        }

        void addSequence(const std::string &name,
                const SequenceDetails *details)
        {
            this->sequences_[name] = details;
        }

        static bool compareReferencePosition(const AlignmentBlock *a,
                const AlignmentBlock *b)
        {
            return (a->getReferenceSequence()->get_start()
                    < b->getReferenceSequence()->get_start());
        }


    private:
        typedef std::map<std::string, const SequenceDetails *> Container;
        Container sequences_;
        // This points to the string stored in the owning
        // WholeGenomeAlignment.
        const std::string *reference_name_;

        void clearSequences();
        void copySequences(const Container &sequences);
};

#endif /* ALIGNMENTBLOCK_H */
