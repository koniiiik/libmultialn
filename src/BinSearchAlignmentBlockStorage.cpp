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

BinSearchAlignmentBlockStorage::Iterator *
BinSearchAlignmentBlockStorage::find(const size_t pos)
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

    if (this->contents_[start]->getReferenceSequence()->get_start() > pos)
    {
        throw OutOfSequence();
    }

    return new Iterator(this->contents_.begin() + start);
}

BinSearchAlignmentBlockStorage::Iterator *
BinSearchAlignmentBlockStorage::first()
{
    this->prepare();
    return new Iterator(this->contents_.begin());
}

size_t BinSearchAlignmentBlockStorage::size() const
{
    return this->contents_.size();
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

BinSearchAlignmentBlockStorage::Iterator::reference
BinSearchAlignmentBlockStorage::Iterator::operator*() const
{
    return **(this->current_);
}

BinSearchAlignmentBlockStorage::Iterator::pointer
BinSearchAlignmentBlockStorage::Iterator::operator->() const
{
    return *(this->current_);
}

BinSearchAlignmentBlockStorage::Iterator&
BinSearchAlignmentBlockStorage::Iterator::operator++()
{
    ++(this->current_);
    return *this;
}

void BinSearchAlignmentBlockStorage::Iterator::operator++(int)
{
    this->current_++;
}
