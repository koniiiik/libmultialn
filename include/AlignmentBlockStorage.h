#ifndef ALIGNMENTBLOCKSTORAGE_H
#define ALIGNMENTBLOCKSTORAGE_H

#include <AlignmentBlock.h>
#include <iterator>


class AlignmentBlock;

class AlignmentBlockStorage
{
    public:
        /*
        ** This class is used to iterate over the storage contents. It is
        ** implemented as a forward iterator, though its usage is slightly
        ** clumsy because C++ iterators don't play well with class
        ** inheritance and it has no constructors, obviously.
        **
        ** Note that the postfix operator returns void since it would have
        ** to return by value as this is C++ and we obviously can't do
        ** this with an abstract class.
        */
        // TODO: define behavior beyond the storage (exception?)
        class Iterator
        {
            public:
                // iterator_traits requirements
                typedef typename std::forward_iterator_tag iterator_category;
                typedef AlignmentBlock value_type;
                typedef AlignmentBlock * pointer;
                typedef AlignmentBlock & reference;
                typedef ptrdiff_t difference_type;

                // Forward iterator requirements
                virtual reference operator*() const = 0;
                virtual pointer operator->() const = 0;
                virtual Iterator& operator++() = 0;
                virtual void operator++(int) = 0;

                virtual bool operator==(const Iterator &other) const
                {
                    // This checks for identity of the blocks pointed to.
                    return &**this == &*other;
                }
                bool operator!=(const Iterator &other) const
                {
                    return !(*this == other);
                }
        };

        virtual ~AlignmentBlockStorage()
        { }
        virtual void addBlock(AlignmentBlock *) = 0;

        /*
        ** Returns an Iterator pointing to the last block whose starting
        ** position on the reference sequence compares less than or equal
        ** to the given position.
        **
        ** Throws OutOfSequence if there is no such block.
        */
        virtual Iterator * find(const size_t pos) = 0;

        /*
        ** Returns the last block whose starting position on the reference
        ** sequence compares less than or equal to the given position.
        **
        ** Throws OutOfSequence in case the position is not contained in
        ** any block.
        */
        virtual const AlignmentBlock * getBlock(const size_t pos);

        /*
        ** Analogic to the STL first method on containers, returns an
        ** iterator pointing to the first block.
        */
        virtual Iterator * first() = 0;

        /*
        ** Returns the number of blocks contained within this storage.
        */
        virtual size_t size() const = 0;
};

#endif /* ALIGNMENTBLOCKSTORAGE_H */
