#include <string>
#include <tuple>
#include <vector>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlock.h>
#include <AlignmentBlockStorage.h>

using std::string;
using std::tuple;
using std::make_tuple;
using std::get;
using std::vector;


WholeGenomeAlignment::WholeGenomeAlignment(const string &reference,
        AlignmentBlockStorage *storage):
    reference_(reference), storage_(storage)
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
    AlignmentBlock * block = this->storage_->getBlock(position);
    seqid_t informant_id = this->getSequenceId(informant);
    return block->mapPositionToInformant(position, informant_id, boundary);
}

size_t WholeGenomeAlignment::getSequenceSize(seqid_t sequence) const
{
    auto it = this->sequence_details_map_.find(sequence);
    if (it == this->sequence_details_map_.end())
    {
        throw SequenceDoesNotExist();
    }
    return get<1>(it->second);
}

size_t WholeGenomeAlignment::getReferenceSize() const
{
    return this->getSequenceSize(kReferenceSequenceId);
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

seqid_t WholeGenomeAlignment::requestSequenceId(const string &name,
        size_t size)
{
    try
    {
        seqid_t id = this->getSequenceId(name);
        // This is needed the first time the reference is requested.
        if (this->sequence_details_map_.count(id) == 0)
        {
            this->sequence_details_map_[id] = make_tuple(name, size);
        }
        return id;
    }
    catch (SequenceDoesNotExist &e)
    { }
    seqid_t new_id = 1;
    // TODO: unphpize this
    while (this->sequence_details_map_.count(new_id) > 0)
    {
        ++new_id;
        if (new_id == 0)
        {
            throw std::exception();
        }
    }
    this->sequence_details_map_[new_id] = make_tuple(name, size);
    this->sequence_name_map_[name] = new_id;
    return new_id;
}

size_t WholeGenomeAlignment::countKnownSequences() const
{
    return this->sequence_name_map_.size();
}

vector<string> * WholeGenomeAlignment::getSequenceList() const
{
    vector<string> *res = new vector<string>;
    res->push_back(this->get_reference());
    for (auto it = this->sequence_details_map_.begin();
            it != this->sequence_details_map_.end();
            ++it)
    {
        if (it->first != kReferenceSequenceId)
        {
            res->push_back(get<0>(it->second));
        }
    }
    return res;
}
