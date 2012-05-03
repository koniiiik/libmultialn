#ifndef MULTIALNCONSTANTS_H
#define MULTIALNCONSTANTS_H

// Indicates whether we are mapping the beginning or the end of an
// interval.
enum IntervalBoundary {
    INTERVAL_BEGIN,
    INTERVAL_END,
};

typedef unsigned short int seqid_t;

const seqid_t kReferenceSequenceId = -1;

#endif /* MULTIALNCONSTANTS_H */
