#include <algorithm>

#include <BinSearchAlignmentBlockStorage.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>


BinSearchAlignmentBlockStorage::~BinSearchAlignmentBlockStorage()
{
    for (size_t i = 0; i < this->contents_.size(); ++i)
    {
        delete this->contents_[i];
    }
}

void BinSearchAlignmentBlockStorage::addBlock(AlignmentBlock *block)
{
    this->prepared_ = false;
    this->contents_.push_back(block);
}

const AlignmentBlock * BinSearchAlignmentBlockStorage::findBlock(
        const size_t pos)
{
    if (this->contents_.empty())
    {
        throw OutOfSequence();
    }

    this->prepare();
    size_t start = 0, end = this->contents_.size();
    while ((end - start) > 1)
    {
        size_t middle = (start + end) / 2;
        if (this->contents_[middle]->getReferenceSequence()->get_start() <= pos)
        {
            start = middle;
        }
        else
        {
            end = middle;
        }
    }

    AlignmentBlock * block = this->contents_[start];
    // We need to do this to verify the position is contained within this
    // block and thwow OutOfSequence otherwise.
    block->getReferenceSequence()->sequenceToAlignment(pos);
    return block;
}

void BinSearchAlignmentBlockStorage::prepare()
{
    if (this->prepared_)
    {
        return;
    }
    std::sort(this->contents_.begin(), this->contents_.end(),
            AlignmentBlock::compareReferencePosition);
    this->prepared_ = true;
}
