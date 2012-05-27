#ifndef BITSEQUENCEFACTORY_H
#define BITSEQUENCEFACTORY_H

#include <BitSequence.h>
#include <BitString.h>

#include <BitSequenceDummy.h>


class BitSequenceFactory
{
    public:
        virtual ~BitSequenceFactory()
        { }

        virtual cds_static::BitSequence * getInstance(
                const cds_utils::BitString &) const = 0;
};

class BitSequenceRGFactory: public BitSequenceFactory
{
    public:
        BitSequenceRGFactory(size_t factor):
            factor_(factor)
        { }

        virtual ~BitSequenceRGFactory()
        { }

        virtual cds_static::BitSequence * getInstance(
                const cds_utils::BitString &str) const
        {
            return new cds_static::BitSequenceRG(str, this->factor_);
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

        virtual cds_static::BitSequence * getInstance(
                const cds_utils::BitString &str) const
        {
            return new cds_static::BitSequenceRRR(str);
        }
};

class BitSequenceSDArrayFactory: public BitSequenceFactory
{
    public:
        BitSequenceSDArrayFactory()
        { }

        virtual ~BitSequenceSDArrayFactory()
        { }

        virtual cds_static::BitSequence * getInstance(
                const cds_utils::BitString &str) const
        {
            return new cds_static::BitSequenceSDArray(str);
        }
};

class BitSequenceDummyFactory: public BitSequenceFactory
{
    public:
        BitSequenceDummyFactory()
        { }

        virtual ~BitSequenceDummyFactory()
        { }

        virtual cds_static::BitSequence * getInstance(
                const cds_utils::BitString &) const
        {
            return new BitSequenceDummy();
        }
};

#endif /* BITSEQUENCEFACTORY_H */
