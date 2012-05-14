#include <AlignmentBlockStorage.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>


AlignmentBlock * AlignmentBlockStorage::getBlock(const size_t pos)
{
    Iterator * it = this->find(pos);
    AlignmentBlock *block = &**it;
    delete it;
    return block;
}
