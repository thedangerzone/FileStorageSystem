#pragma once
#include <boost/endian.hpp>

using namespace boost::endian;


inline bool isLittleEndian()
{
    const unsigned one = 1U;
    return reinterpret_cast<const char*>(&one) + sizeof(unsigned) - 1;
}

template <typename T>
T littleEndian(T u)
{
    // Make sure we aren't in big 
    if (isLittleEndian()) {
        return u;
    }

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    // flip bytes
    for (size_t i = 0; i < sizeof(T); i++)
        dest.u8[i] = source.u8[sizeof(T) - i - 1];

    return dest.u;
}
