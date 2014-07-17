#ifndef ELEMENTITERATOR_H__
#define ELEMENTITERATOR_H__


#include <iterator>


namespace geometry
{
    template<class T>
    class const_iterator<T> : public std::iterator<random_access_iterator_tag, T>
    {
    };
}


#endif//ELEMENTITERATOR_H__
