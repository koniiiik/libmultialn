#include <algorithm>
#include <BitString.h>
#include <BitSequence.h>

#include <RankAlignmentBlockStorage.h>
#include <AlignmentBlock.h>
#include <SequenceDetails.h>


using cds_utils::BitString;
using cds_static::BitSequence;
using cds_static::BitSequenceRRR;

RankAlignmentBlockStorage::~RankAlignmentBlockStorage()
{
    for (size_t i = 0; i < this->contents_.size(); ++i)
    {
        delete this->contents_[i];
    }
    delete this->index_;
}

void RankAlignmentBlockStorage::addBlock(AlignmentBlock *block)
{
    this->unprepare();
    this->contents_.push_back(block);
}

RankAlignmentBlockStorage::iterator
RankAlignmentBlockStorage::find(const size_t pos)
{
    if (this->contents_.empty())
    {
        throw OutOfSequence();
    }

    this->prepare();

    size_t index = this->index_->rank1(pos);
    if (index == 0 || index > this->contents_.size())
    {
        throw OutOfSequence();
    }

    --index;

    return
        iterator(IteratorImplementation(this->contents_.begin() + index));
}

RankAlignmentBlockStorage::iterator
RankAlignmentBlockStorage::begin()
{
    this->prepare();
    return iterator(IteratorImplementation(this->contents_.begin()));
}

RankAlignmentBlockStorage::iterator
RankAlignmentBlockStorage::end()
{
    this->prepare();
    return iterator(IteratorImplementation(this->contents_.end()));
}

size_t RankAlignmentBlockStorage::size() const
{
    return this->contents_.size();
}

void RankAlignmentBlockStorage::unprepare()
{
    this->prepared_ = false;
    delete this->index_;
    this->index_ = NULL;
}

void RankAlignmentBlockStorage::prepare()
{
    if (this->prepared_)
    {
        return;
    }
    std::sort(this->contents_.begin(), this->contents_.end(),
            AlignmentBlock::compareReferencePosition);

    size_t reference_size =
        this->contents_[0]->getReferenceSequence()->get_src_size();
    // add a padding to make sure we don't exceed it on boundary
    reference_size += 47;

    BitString bitstr(reference_size);
    for (auto it = this->contents_.begin(); it != this->contents_.end(); ++it)
    {
        size_t position = (*it)->getReferenceSequence()->get_start();
        bitstr.setBit(position, 1);
    }
    this->index_ = new BitSequenceRRR(bitstr);

    this->prepared_ = true;
}
