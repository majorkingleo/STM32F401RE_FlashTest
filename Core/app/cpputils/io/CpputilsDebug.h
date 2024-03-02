#ifndef CPPUTILS_IO_DEBUG_H
#define CPPUTILS_IO_DEBUG_H

#include <string>

#ifdef NDEBUG
# define CPPDEBUG( out )
#else
# ifdef _MSC_VER
#   define CPPDEBUG( x ) if( Tools::x_debug ) { Tools::x_debug->add(  __FILE__, __LINE__, __FUNCSIG__, x ); }
# else
#   define CPPDEBUG( x ) if( Tools::x_debug ) { Tools::x_debug->add(  __FILE__, __LINE__, __PRETTY_FUNCTION__, x ); }
# endif
#endif

namespace Tools {

class Debug
{
 public:
  Debug();
  virtual ~Debug();
  
  virtual void add( const char *file, unsigned line, const char *function, const std::string & s ) = 0;
  virtual void add( const char *file, unsigned line, const char *function, const std::wstring & s ) = 0;
};

extern Debug *x_debug;

}

#endif
