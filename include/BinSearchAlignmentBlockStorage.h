#ifndef BINSEARCHALIGNMENTBLOCKSTORAGE_H
#define BINSEARCHALIGNMENTBLOCKSTORAGE_H

#include <vector>

#include <AlignmentBlock.h>
#include <AlignmentBlockStorage.h>


/*
** Implementation of AlignmentBlockStorage which stores the blocks in a
** sorted vector and ises binary search to find the right block.
**
** The sorting is handled lazily, i. e. the list is sorted the first time
** a find operation is performed.
*/
class BinSearchAlignmentBlockStorage: public AlignmentBlockStorage
{
    private:
        typedef std::vector<AlignmentBlock *> Container;

    public:
        class Iterator: public AlignmentBlockStorage::Iterator
        {
            public:
                Iterator(const Container::iterator &it): current_(it)
                { }
                virtual reference operator*() const;
                virtual pointer operator->() const;
                virtual Iterator& operator++();
                virtual void operator++(int);

            private:
                Container::iterator current_;
        };
        BinSearchAlignmentBlockStorage():
            prepared_(false)
        { }
        virtual ~BinSearchAlignmentBlockStorage();
        virtual void addBlock(AlignmentBlock *block);
        virtual Iterator * find(const size_t pos);

    private:
        Container contents_;
        bool prepared_;
        void prepare();
};

#endif /* BINSEARCHALIGNMENTBLOCKSTORAGE_H */
