#include <BitSequence.h>
#include <SequenceDetails.h>

template<typename content_t>
SequenceDetails * GenerateSequence(size_t start, size_t total,
        bool reverse, content_t content)
{
    cds_static::BitSequence *seq = new cds_static::BitSequenceRG(
            &content, 8 * sizeof(content), 2);
    return new SequenceDetails(start, seq->countOnes(), reverse, total, seq);
}
