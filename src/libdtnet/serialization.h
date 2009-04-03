#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#ifdef BUILDING_LIBRARY

#ifdef SERIALIZE_TEXT
    #include <boost/archive/text_oarchive.hpp>
    #include <boost/archive/text_iarchive.hpp>
#else
    #include <boost/archive/binary_oarchive.hpp>
    #include <boost/archive/binary_iarchive.hpp>
#endif

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/deque.hpp>

#endif

#endif
