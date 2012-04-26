#include <string>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlock.h>

using std::string;


size_t WholeGenomeAlignment::mapPositionToInformant(size_t position,
        const string &informant, IntervalBoundary boundary) const
{
    const AlignmentBlock * block = this->storage_->findBlock(position);
    return block->mapPositionToInformant(position, informant, boundary);
}
