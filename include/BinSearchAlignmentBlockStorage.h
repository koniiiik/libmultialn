#ifndef BINSEARCHALIGNMENTBLOCKSTORAGE_H
#define BINSEARCHALIGNMENTBLOCKSTORAGE_H

#include <vector>

#include <AlignmentBlock.h>
#include <AlignmentBlockStorage.h>


// forward declaration
class BinSearchAlignmentBlockStorageIteratorImplementation;

/*
** Implementation of AlignmentBlockStorage which stores the blocks in a
** sorted vector and ises binary search to find the right block.
**
** The sorting is handled lazily, i. e. the list is sorted the first time
** a find operation is performed.
*/
class BinSearchAlignmentBlockStorage: public AlignmentBlockStorage
{
    public:
        typedef std::vector<AlignmentBlock *> Container;

        BinSearchAlignmentBlockStorage():
            prepared_(false)
        { }
        virtual ~BinSearchAlignmentBlockStorage();
        virtual void addBlock(AlignmentBlock *block);
        virtual iterator find(const size_t pos);
        virtual iterator begin();
        virtual iterator end();
        virtual size_t size() const;

    private:
        typedef BinSearchAlignmentBlockStorageIteratorImplementation
            IteratorImplementation;
        Container contents_;
        bool prepared_;
        void prepare();
};

class BinSearchAlignmentBlockStorageIteratorImplementation:
    public AlignmentBlockStorageIteratorImplementation
{
    public:
        BinSearchAlignmentBlockStorageIteratorImplementation(
                const BinSearchAlignmentBlockStorageIteratorImplementation &other):
            current_(other.current_)
        { }
        BinSearchAlignmentBlockStorageIteratorImplementation(
                const BinSearchAlignmentBlockStorage::Container::iterator &it):
            current_(it)
        { }
        virtual BinSearchAlignmentBlockStorageIteratorImplementation * clone() const
        {
            return new BinSearchAlignmentBlockStorageIteratorImplementation(*this);
        }
        virtual reference operator*() const
        {
            return **(this->current_);
        }
        virtual pointer operator->() const
        {
            return *(this->current_);
        }
        virtual BinSearchAlignmentBlockStorageIteratorImplementation& operator++()
        {
            ++(this->current_);
            return *this;
        }

    protected:
        virtual bool equal(
                const AlignmentBlockStorageIteratorImplementation &other) const
        {
            const BinSearchAlignmentBlockStorageIteratorImplementation &other2 =
                static_cast<const BinSearchAlignmentBlockStorageIteratorImplementation&>(other);
            return other2.current_ == this->current_;
        }

    private:
        BinSearchAlignmentBlockStorage::Container::iterator current_;
};

#endif /* BINSEARCHALIGNMENTBLOCKSTORAGE_H */
