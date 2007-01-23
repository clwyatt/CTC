#ifndef _bytereverse_hh_
#define _bytereverse_hh_
#include <algorithm>
template <class T> T byte_reverse(T val)
{
  //treat val as a byte stream
 unsigned char *p=reinterpret_cast<unsigned char*> (&val);
 std::reverse(p, p+sizeof(val));
 return val;
}
#endif
