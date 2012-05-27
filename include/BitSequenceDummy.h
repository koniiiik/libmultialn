#ifndef BITSEQUENCEDUMMY_H
#define BITSEQUENCEDUMMY_H

#include <BitSequence.h>


class BitSequenceDummy: public cds_static::BitSequence
{
    public:
        BitSequenceDummy()
        { }
        virtual ~BitSequenceDummy()
        { }
        virtual size_t rank0(const size_t) const
        {
            return -1;
        }
        virtual size_t select0(const size_t) const
        {
            return -1;
        }
        virtual size_t rank1(const size_t) const
        {
            return -1;
        }
        virtual size_t select1(const size_t) const
        {
            return -1;
        }
        virtual size_t selectNext1(const size_t) const
        {
            return -1;
        }
        virtual size_t selectNext0(const size_t) const
        {
            return -1;
        }
        virtual size_t selectPrev1(const size_t) const
        {
            return -1;
        }
        virtual size_t selectPrev0(const size_t) const
        {
            return -1;
        }
        virtual bool access(const size_t) const
        {
            return false;
        }
        virtual bool access(const size_t, size_t &) const
        {
            return false;
        }
        virtual size_t getLength() const
        {
            return -1;
        }
        virtual size_t countOnes() const
        {
            return -1;
        }
        virtual size_t countZeros() const
        {
            return -1;
        }
        virtual size_t getSize() const
        {
            return -1;
        }
        virtual void save(ofstream &) const
        { }
};

#endif /* BITSEQUENCEDUMMY_H */
