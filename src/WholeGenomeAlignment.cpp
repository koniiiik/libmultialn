#include <string>
#include <tuple>
#include <vector>
#include <utility>

#include <WholeGenomeAlignment.h>
#include <AlignmentBlock.h>
#include <AlignmentBlockStorage.h>

using std::string;
using std::tuple;
using std::make_tuple;
using std::get;
using std::vector;
using std::pair;
using std::make_pair;
using cds_static::BitSequence;


WholeGenomeAlignment::WholeGenomeAlignment(const string &reference,
        AlignmentBlockStorage *storage):
    reference_(reference), storage_(storage), bit_sequence_(NULL)
{
    this->sequence_name_map_[reference] = kReferenceSequenceId;
}

WholeGenomeAlignment::~WholeGenomeAlignment()
{
    delete this->storage_;
    delete this->bit_sequence_;
}

size_t WholeGenomeAlignment::mapPositionToInformant(size_t position,
        const string &informant, IntervalBoundary boundary) const
{
    AlignmentBlock *block = this->storage_->getBlock(position);
    seqid_t informant_id = this->getSequenceId(informant);
    return block->mapPositionToInformant(position, informant_id, boundary);
}

WholeGenomeAlignment::PositionMapping *
WholeGenomeAlignment::mapPositionToAll(size_t position,
        IntervalBoundary boundary) const
{
    AlignmentBlock *block = this->storage_->getBlock(position);
    const AlignmentBlock::PositionMapping *block_mapping =
        block->mapPositionToAll(position, boundary);
    PositionMapping *mapping = new PositionMapping();
    for (auto it = block_mapping->begin(); it != block_mapping->end(); ++it)
    {
        (*mapping)[this->getSequenceName(it->first)] = it->second;
    }
    delete block_mapping;
    return mapping;
}

pair<size_t, size_t> WholeGenomeAlignment::mapRegionToInformant(
        size_t region_start, size_t region_end, const string &informant)
{
    auto first_block = this->storage_->begin();
    try
    {
        first_block = this->storage_->find(region_start);
    }
    catch (OutOfSequence &e)
    {
        region_start = first_block->getReferenceSequence()->get_start();
    }
    auto last_block = this->storage_->find(region_end);

    // To avoid possible nasty NULL dereferencing surprises.
    if (first_block == this->storage_->end()
            || last_block == this->storage_->end())
    {
        throw OutOfSequence();
    }

    seqid_t informant_id = this->getSequenceId(informant);

    // Trivial case: the region fits within a single block.
    if (first_block == last_block)
    {
        return make_pair(first_block->mapPositionToInformant(region_start,
                    informant_id, INTERVAL_BEGIN),
                last_block->mapPositionToInformant(region_end,
                    informant_id, INTERVAL_END));
    }

    long long start_map = -1, last_position = -1, last_but_one = -1;

    // If the start has a mapping within the first possible block, get it,
    // otherwise we'll just advance and get it from the first possible
    // block.
    try
    {
        start_map = first_block->mapPositionToInformant(region_start,
                informant_id, INTERVAL_BEGIN);
        last_position = first_block->getSequence(informant_id)->get_end();
    }
    catch (OutOfSequence &e) { }
    catch (SequenceDoesNotExist &e) { }

    do
    {
        ++first_block;
        const SequenceDetails *informant_seq;
        try
        {
            informant_seq = first_block->getSequence(informant_id);
        }
        catch (SequenceDoesNotExist &e)
        {
            continue;
        }
        if (start_map < 0)
        {
            start_map = informant_seq->get_start();
            last_position = informant_seq->get_end();
            continue;
        }
        // Here we check that this block's informant sequence is in the
        // same strand as the previous ones and that it doesn't break the
        // order.
        long long this_start = informant_seq->get_start(),
                  this_end = informant_seq->get_end();
        if ((last_position - start_map) * (this_end - this_start) < 0 ||
                (last_position - start_map) * (this_start - last_position) < 0)
        {
            throw MappingDoesNotExist();
        }
        last_but_one = last_position;
        last_position = this_end;
    }
    while (first_block != last_block);

    // Lastly, check if there is a valid mapping of the end in the last
    // block.
    int stage = 0;
    try
    {
        last_block->getReferenceSequence()->sequenceToAlignment(region_end);
        ++stage;
        last_position = last_block->mapPositionToInformant(region_end,
                informant_id, INTERVAL_END);
    }
    catch (SequenceDoesNotExist &e) { }
    catch (OutOfSequence &e)
    {
        // In this case we have to go one step back, since the last block
        // is too far.
        if (stage == 1)
        {
            last_position = last_but_one;
        }
    }

    // The informant was not present in any block.
    if (last_position < 0)
    {
        throw SequenceDoesNotExist();
    }

    return make_pair(start_map, last_position);
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

const string & WholeGenomeAlignment::getSequenceName(seqid_t id) const
{
    auto iter = this->sequence_details_map_.find(id);
    if (iter == this->sequence_details_map_.end())
    {
        throw SequenceDoesNotExist();
    }
    return get<0>(iter->second);
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

void WholeGenomeAlignment::setBitSequence(BitSequence *bit_sequence)
{
    this->bit_sequence_ = bit_sequence;
    for (auto it = this->storage_->begin(); it != this->storage_->end();
            ++it)
    {
        it->setBitSequence(bit_sequence);
    }
}
