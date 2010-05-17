#ifndef __QR_UTILITY_H__
#define __QR_UTILITY_H__

#include "config.h"
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/typeof/typeof.hpp>

#define Unreachable() { assert(!"Unreachable code executed."); throw ::std::runtime_error(__FILE__ ":" BOOST_PP_STRINGIZE(__LINE__) ": Unreachable code executed."); } 

#define FOREACH_VAR(NAME) BOOST_PP_CAT(NAME, __LINE__)

/* TODO: break / continue doesn't work! */
#define map_foreach(KEY, VAL, MAP)                                              \
  BOOST_FOREACH(BOOST_TYPEOF(*(MAP).begin())& pair, MAP)                        \
  for(bool FOREACH_VAR(stop) = false; !FOREACH_VAR(stop);)                      \
  for(KEY = pair.first; !FOREACH_VAR(stop);)                                    \
  for(VAL = pair.second; !FOREACH_VAR(stop); FOREACH_VAR(stop) = true)


#endif // __QR_UTILITY_H__
