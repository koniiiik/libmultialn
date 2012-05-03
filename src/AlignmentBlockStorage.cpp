#include <AlignmentBlockStorage.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>


AlignmentBlock * AlignmentBlockStorage::getBlock(const size_t pos)
{
    Iterator * it = this->find(pos);
    AlignmentBlock *block = &**it;
    delete it;
    // We need to do this to verify the position is contained within this
    // block and throw OutOfSequence otherwise.
    block->getReferenceSequence()->sequenceToAlignment(pos);
    return block;
}
