#include <string>
#include <tuple>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlock.h>
#include <AlignmentBlockStorage.h>

using std::string;
using std::tuple;
using std::make_tuple;


WholeGenomeAlignment::WholeGenomeAlignment(const string &reference,
        AlignmentBlockStorage *storage):
    storage_(storage)
{
    this->sequence_name_map_[reference] = kReferenceSequenceId;
}

WholeGenomeAlignment::~WholeGenomeAlignment()
{
    delete this->storage_;
}

size_t WholeGenomeAlignment::mapPositionToInformant(size_t position,
        const string &informant, IntervalBoundary boundary) const
{
    const AlignmentBlock * block = this->storage_->getBlock(position);
    seqid_t informant_id = this->getSequenceId(informant);
    return block->mapPositionToInformant(position, informant_id, boundary);
}

size_t WholeGenomeAlignment::getReferenceSize() const
{
    AlignmentBlockStorage::Iterator * it = this->storage_->first();
    size_t res = (*it)->getReferenceSequence()->get_src_size();
    delete it;
    return res;
}

seqid_t WholeGenomeAlignment::getSequenceId(const string &name) const
{
    auto it = this->sequence_name_map_.find(name);
    if (it == this->sequence_name_map_.end())
    {
        throw SequenceDoesNotExist();
    }
    return it->second;
}

seqid_t WholeGenomeAlignment::requestSequenceId(const string &name)
{
    try
    {
        seqid_t id = this->getSequenceId(name);
        // This is needed the first time the reference is requested.
        if (this->sequence_details_map_.count(id) == 0)
        {
            this->sequence_details_map_[id] = make_tuple(name);
        }
        return id;
    }
    catch (SequenceDoesNotExist &e)
    { }
    seqid_t new_id = 1;
    while (this->sequence_details_map_.count(new_id) > 0)
    {
        ++new_id;
        if (new_id == 0)
        {
            throw std::exception();
        }
    }
    this->sequence_details_map_[new_id] = make_tuple(name);
    this->sequence_name_map_[name] = new_id;
    return new_id;
}

size_t WholeGenomeAlignment::countKnownSequences() const
{
    return this->sequence_name_map_.size();
}
