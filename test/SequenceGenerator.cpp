#include <string>
#include <BitString.h>
#include <BitSequence.h>
#include <SequenceDetails.h>
#include <BitSequenceFactory.h>
#include <MultialnConstants.h>

#include "SequenceGenerator.h"


cds_static::BitSequence * GenerateBitSequence(const BitSequenceFactory *factory,
        const std::string &contents)
{
    cds_utils::BitString bstr(contents.size());
    for (size_t i = 0; i < contents.size(); ++i)
    {
        bstr.setBit(i, contents[i] == '1');
    }
    return factory->getInstance(bstr);
}
