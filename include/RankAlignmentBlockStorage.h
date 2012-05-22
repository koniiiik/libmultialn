#ifndef RANKALIGNMENTBLOCKSTORAGE_H
#define RANKALIGNMENTBLOCKSTORAGE_H

#include <vector>

#include <AlignmentBlock.h>
#include <AlignmentBlockStorage.h>


namespace cds_static
{
class BitSequence;
}

// TODO: Factor out the common parts with BinSearchAlignmentBlockStorage
// into something like an abstract VectorAlignmentBlockStorage

// forward declaration
class RankAlignmentBlockStorageIteratorImplementation;

/*
** Implementation of AlignmentBlockStorage which stores the blocks in a
** sorted vector and ises binary search to find the right block.
**
** The sorting is handled lazily, i. e. the list is sorted the first time
** a find operation is performed.
*/
class RankAlignmentBlockStorage: public AlignmentBlockStorage
{
    public:
        typedef std::vector<AlignmentBlock *> Container;

        RankAlignmentBlockStorage():
            prepared_(false), index_(NULL)
        { }
        virtual ~RankAlignmentBlockStorage();
        virtual void addBlock(AlignmentBlock *block);
        virtual iterator find(const size_t pos);
        virtual iterator begin();
        virtual iterator end();
        virtual size_t size() const;

    private:
        typedef RankAlignmentBlockStorageIteratorImplementation
            IteratorImplementation;
        Container contents_;
        bool prepared_;
        cds_static::BitSequence *index_;
        void unprepare();
        void prepare();
};

class RankAlignmentBlockStorageIteratorImplementation:
    public AlignmentBlockStorageIteratorImplementation
{
    public:
        RankAlignmentBlockStorageIteratorImplementation(
                const RankAlignmentBlockStorageIteratorImplementation &other):
            current_(other.current_)
        { }
        RankAlignmentBlockStorageIteratorImplementation(
                const RankAlignmentBlockStorage::Container::iterator &it):
            current_(it)
        { }
        virtual RankAlignmentBlockStorageIteratorImplementation * clone() const
        {
            return new RankAlignmentBlockStorageIteratorImplementation(*this);
        }
        virtual reference operator*() const
        {
            return **(this->current_);
        }
        virtual pointer operator->() const
        {
            return *(this->current_);
        }
        virtual RankAlignmentBlockStorageIteratorImplementation& operator++()
        {
            ++(this->current_);
            return *this;
        }

    protected:
        virtual bool equal(
                const AlignmentBlockStorageIteratorImplementation &other) const
        {
            const RankAlignmentBlockStorageIteratorImplementation &other2 =
                static_cast<const RankAlignmentBlockStorageIteratorImplementation&>(other);
            return other2.current_ == this->current_;
        }

    private:
        RankAlignmentBlockStorage::Container::iterator current_;
};

#endif /* RANKALIGNMENTBLOCKSTORAGE_H */
