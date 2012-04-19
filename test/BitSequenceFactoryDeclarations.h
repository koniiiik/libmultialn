#ifndef BITSEQUENCEFACTORYDECLARATIONS_H
#define BITSEQUENCEFACTORYDECLARATIONS_H

#include <gtest/gtest.h>
#include <BitSequenceFactory.h>

#define INSTANTIATE_BITSEQ_TEST_P(test_name) \
    INSTANTIATE_TEST_CASE_P( \
        AllBitSequenceImplementations, \
        test_name, \
        ::testing::Values(&fact_rg2, &fact_rg3, &fact_rg4, &fact_rg20, \
            &fact_rrr, &fact_sdarray));

extern BitSequenceRGFactory fact_rg2, fact_rg3, fact_rg4, fact_rg20;
extern BitSequenceRRRFactory fact_rrr;
extern BitSequenceSDArrayFactory fact_sdarray;

typedef ::testing::TestWithParam<BitSequenceFactory *>
    BitSequenceParamTest;

#endif /* BITSEQUENCEFACTORYDECLARATIONS_H */
