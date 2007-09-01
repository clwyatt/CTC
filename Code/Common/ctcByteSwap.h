/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: ctcByteSwap.h 23 2006-06-29 14:45:24Z clwyatt $
Language:  C++
*******************************************************************************/

#ifndef __ctcByteSwap_h
#define __ctcByteSwap_h

#include <cassert>

namespace ctc
{

  // byte swap function
  template< class T >
  inline void swab( T& t ) {
    assert( sizeof( T ) % 2 == 0 );
    char* ca = reinterpret_cast< char* >( &t );
    std::reverse( ca, ca + sizeof( T ) );
  };

} // end namespace ctc

#endif // __ctcByteSwap_h
