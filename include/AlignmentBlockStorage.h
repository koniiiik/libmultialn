#ifndef ALIGNMENTBLOCKSTORAGE_H
#define ALIGNMENTBLOCKSTORAGE_H

#include <AlignmentBlock.h>
#include <iterator>


// forward declaration
class AlignmentBlockStorageIterator;

class AlignmentBlockStorage
{
    public:
        typedef AlignmentBlockStorageIterator iterator;

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
        virtual iterator find(const size_t pos) = 0;

        /*
        ** Returns the last block whose starting position on the reference
        ** sequence compares less than or equal to the given position.
        **
        ** Throws OutOfSequence in case the position is not contained in
        ** any block.
        */
        virtual AlignmentBlock * getBlock(const size_t pos);

        /*
        ** Analogic to the STL begin method on containers, returns an
        ** iterator pointing to the first block.
        */
        virtual iterator begin() = 0;

        /*
        ** Analogic to the STL end method on containers, returns an
        ** iterator pointing after the last block.
        */
        virtual iterator end() = 0;

        /*
        ** Returns the number of blocks contained within this storage.
        */
        virtual size_t size() const = 0;
};

/*
** Inspiration for the following iterator implementation taken from
** http://www.ocoudert.com/blog/2010/07/07/how-to-write-abstract-iterators-in-c/
*/

/*
** This is the class providing the actual iteration. It should be
** subclassed by concrete AlignmentBlockStorage implementations.
*/
class AlignmentBlockStorageIteratorImplementation
{
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef AlignmentBlock value_type;
        typedef AlignmentBlock * pointer;
        typedef AlignmentBlock & reference;
        typedef ptrdiff_t difference_type;

        AlignmentBlockStorageIteratorImplementation()
        { }
        virtual ~AlignmentBlockStorageIteratorImplementation()
        { }

        virtual AlignmentBlockStorageIteratorImplementation * clone() const
        {
            return new AlignmentBlockStorageIteratorImplementation();
        }

        virtual reference operator*() const
        {
            static value_type dummy;
            return dummy;
        }
        virtual pointer operator->() const
        {
            return NULL;
        }
        virtual AlignmentBlockStorageIteratorImplementation & operator++()
        {
            return *this;
        }
        bool operator==(
                const AlignmentBlockStorageIteratorImplementation &other) const
        {
            return (typeid(*this) == typeid(other)) && this->equal(other);
        }

    protected:
        virtual bool equal(
                const AlignmentBlockStorageIteratorImplementation &) const
        {
            return true;
        }
};

/*
** This class is used to iterate over the storage contents. It is
** implemented as a forward iterator and is what the user will be facing.
*/
// TODO: define behavior beyond the storage
class AlignmentBlockStorageIterator
{
    public:
        // iterator_traits requirements
        typedef std::forward_iterator_tag iterator_category;
        typedef AlignmentBlock value_type;
        typedef AlignmentBlock * pointer;
        typedef AlignmentBlock & reference;
        typedef ptrdiff_t difference_type;

        AlignmentBlockStorageIterator():
            implementation_(new AlignmentBlockStorageIteratorImplementation)
        { }
        AlignmentBlockStorageIterator(
                const AlignmentBlockStorageIterator &other):
            implementation_(other.implementation_->clone())
        { }
        AlignmentBlockStorageIterator(
                const AlignmentBlockStorageIteratorImplementation &impl):
            implementation_(impl.clone())
        { }
        ~AlignmentBlockStorageIterator()
        {
            delete this->implementation_;
        }

        AlignmentBlockStorageIterator & operator=(
                const AlignmentBlockStorageIterator &other)
        {
            if (this->implementation_ != other.implementation_)
            {
                delete this->implementation_;
                this->implementation_ = other.implementation_->clone();
            }
            return *this;
        }

        // Forward iterator requirements
        reference operator*() const
        {
            return **(this->implementation_);
        }
        AlignmentBlockStorageIteratorImplementation & operator->() const
        {
            return *(this->implementation_);
        }
        AlignmentBlockStorageIterator & operator++()
        {
            ++(*(this->implementation_));
            return *this;
        }
        AlignmentBlockStorageIterator operator++(int)
        {
            AlignmentBlockStorageIterator temp(*this);
            ++(*this);
            return temp;
        }

        bool operator==(const AlignmentBlockStorageIterator &other) const
        {
            return (this->implementation_ == other.implementation_)
                || (*(this->implementation_) == *(other.implementation_));
        }
        bool operator!=(const AlignmentBlockStorageIterator &other) const
        {
            return !(*this == other);
        }

    private:
        AlignmentBlockStorageIteratorImplementation *implementation_;
};


#endif /* ALIGNMENTBLOCKSTORAGE_H */
