#ifndef ALIGNMENTBLOCKSTORAGE_H
#define ALIGNMENTBLOCKSTORAGE_H

#include <AlignmentBlock.h>


class AlignmentBlock;

class AlignmentBlockStorage
{
    public:
        virtual ~AlignmentBlockStorage()
        { }
        virtual void addBlock(AlignmentBlock *) = 0;
        /*
        ** Returns the last block whose starting position on the reference
        ** sequence compares less than or equal to the given position.
        */
        virtual const AlignmentBlock * findBlock(const size_t) = 0;
};

#endif /* ALIGNMENTBLOCKSTORAGE_H */
