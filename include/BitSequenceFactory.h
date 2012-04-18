#ifndef BITSEQUENCEFACTORY_H
#define BITSEQUENCEFACTORY_H

#include <BitSequence.h>
#include <BitString.h>


using cds_static::BitSequence;
using cds_static::BitSequenceRG;
using cds_static::BitSequenceRRR;
using cds_static::BitSequenceSDArray;
using cds_utils::BitString;

class BitSequenceFactory
{
    public:
        virtual ~BitSequenceFactory()
        { }

        virtual BitSequence * getInstance(const BitString &) const = 0;
};

class BitSequenceRGFactory: public BitSequenceFactory
{
    public:
        BitSequenceRGFactory(size_t factor):
            factor_(factor)
        { }

        virtual ~BitSequenceRGFactory()
        { }

        virtual BitSequence * getInstance(const BitString &str) const
        {
            return new BitSequenceRG(str, this->factor_);
        }


    private:
        size_t factor_;
};

class BitSequenceRRRFactory: public BitSequenceFactory
{
    public:
        BitSequenceRRRFactory()
        { }

        virtual ~BitSequenceRRRFactory()
        { }

        virtual BitSequence * getInstance(const BitString &str) const
        {
            return new BitSequenceRRR(str);
        }
};

class BitSequenceSDArrayFactory: public BitSequenceFactory
{
    public:
        BitSequenceSDArrayFactory()
        { }

        virtual ~BitSequenceSDArrayFactory()
        { }

        virtual BitSequence * getInstance(const BitString &str) const
        {
            return new BitSequenceSDArray(str);
        }
};

#endif /* BITSEQUENCEFACTORY_H */
