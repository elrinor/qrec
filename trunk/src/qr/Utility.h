#ifndef __QR_UTILITY_H__
#define __QR_UTILITY_H__

#include "config.h"
#include <boost/preprocessor/stringize.hpp>

#define Unreachable() { assert(!"Unreachable code executed."); throw ::std::runtime_error(__FILE__ ":" BOOST_PP_STRINGIZE(__LINE__) ": Unreachable code executed."); } 

#endif // __QR_UTILITY_H__
