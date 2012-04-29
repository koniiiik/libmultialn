#ifndef SEQUENCEGENERATOR_H
#define SEQUENCEGENERATOR_H

#include <string>
#include <BitSequence.h>
#include <SequenceDetails.h>
#include <BitSequenceFactory.h>


cds_static::BitSequence * GenerateBitSequence(const BitSequenceFactory *,
        const std::string &);

SequenceDetails * GenerateSequenceDetails(const BitSequenceFactory *, size_t,
        size_t, bool, const std::string &);

#endif /* SEQUENCEGENERATOR_H */
