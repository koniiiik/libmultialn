#include <string>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlock.h>
#include <AlignmentBlockStorage.h>

using std::string;


WholeGenomeAlignment::~WholeGenomeAlignment()
{
    delete this->storage_;
}

size_t WholeGenomeAlignment::mapPositionToInformant(size_t position,
        const string &informant, IntervalBoundary boundary) const
{
    const AlignmentBlock * block = this->storage_->getBlock(position);
    return block->mapPositionToInformant(position, informant, boundary);
}

size_t WholeGenomeAlignment::getReferenceSize() const
{
    AlignmentBlockStorage::Iterator * it = this->storage_->first();
    size_t res = (*it)->getReferenceSequence()->get_src_size();
    delete it;
    return res;
}
