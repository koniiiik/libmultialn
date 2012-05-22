#ifndef SEQUENCEGENERATOR_H
#define SEQUENCEGENERATOR_H

#include <string>
#include <BitSequence.h>
#include <SequenceDetails.h>
#include <BitSequenceFactory.h>
#include <MultialnConstants.h>


cds_static::BitSequence * GenerateBitSequence(
        const BitSequenceFactory *factory, const std::string &contents);

#endif /* SEQUENCEGENERATOR_H */
