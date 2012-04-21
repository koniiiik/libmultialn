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
    public:
        BinSearchAlignmentBlockStorage():
            prepared_(false)
        { }
        virtual ~BinSearchAlignmentBlockStorage();
        virtual void addBlock(AlignmentBlock *block);
        virtual const AlignmentBlock * findBlock(const size_t pos);

    private:
        typedef std::vector<AlignmentBlock *> Container;
        Container contents_;
        bool prepared_;
        void prepare();
};

#endif /* BINSEARCHALIGNMENTBLOCKSTORAGE_H */
