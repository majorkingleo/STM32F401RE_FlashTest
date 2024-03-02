/**
 * character set encoding functions
 * @author Copyright (c) 2022 - 2023 Martin Oberzalek
 */
#include "ConvertEncoding.h"
#include <xml.h>
#include <utf8_util.h>

/* win-iconv seems to define source pointer as const char * */
#if defined(__MINGW64__) || defined(__MINGW32__) || defined(_MSC_VER)
#  define ICONV_CONST
#endif
#include <iconv.h>


#include <cstring>
#include <stderr_exception.h>

using namespace Tools;

std::string ConvertEncoding::convert( const std::string & s, const std::string & from, const std::string & to )
{
	std::string result;
    std::string error;

	if( !convert( s, from, to, result, error ) ) {
		throw STDERR_EXCEPTION( format( "cannot convert from '%s' to '%s' error: %s", from, to, error ) );
	}

	return result;
}

bool ConvertEncoding::convert( const std::string & s, const std::string & from, const std::string & to, std::string & result, std::string & error )
{
    std::string ret;

    int extra_bufsize = 50;
    bool cont = false;

    iconv_t h = iconv_open( to.c_str(), from.c_str() );

    if( h == (iconv_t)-1 ) {
        return false;
    }

    do
      {
        cont = false;

        char *out_buffer = new char[s.size()+extra_bufsize];
        char *in_buffer = new char[s.size()+1];

        memcpy( in_buffer, s.c_str(), s.size() );

        in_buffer[s.size()]='\0';

        size_t size, in_left=s.size()+1, out_left=s.size()+extra_bufsize;
        char *in;
        char *out;

        in = in_buffer;
        out = out_buffer;


        size = iconv( h, &in, &in_left, &out, &out_left );

        if( size != (size_t)-1 )
          {
            ret = out_buffer;
          } else if( errno == E2BIG ) {
            extra_bufsize*=2;
            cont = true;
            ret = "E2BIG";
          } else if( errno == EILSEQ ) {
        	error = format( "EILSEQ: %s", strerror(errno));
            return false;
          } else if( errno == EINVAL ) {
            error = format( "EINVAL: %s", strerror(errno));
            return false;
          }

        delete[] out_buffer;
        delete[] in_buffer;

      } while( cont == true );

    iconv_close( h );

    result = ret;

    return true;
}

