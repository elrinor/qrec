#ifndef __QRECONSTRUCTOR_CONFIG_H__
#define __QRECONSTRUCTOR_CONFIG_H__

#ifdef foreach
#  error Foreach already defined. Did you forget to include "config.h" first?
#endif
#define foreach BOOST_FOREACH

#ifdef _WIN32
#  define NOMINMAX
#endif

#ifdef _MSC_VER
#  ifdef _DEBUG
#    pragma comment(lib, "carvelibd.lib")
#  else
#    pragma comment(lib, "carvelib.lib")
#  endif
#endif

#endif // __QRECONSTRUCTOR_CONFIG_H__
