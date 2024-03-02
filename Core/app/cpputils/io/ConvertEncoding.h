/**
 * character set encoding functions
 * @author Copyright (c) 2022 - 2023 Martin Oberzalek
 */

#ifndef CPPUTILS_IO_CONVERT_ENCODING_H_
#define CPPUTILS_IO_CONVERT_ENCODING_H_

#include <string>

namespace Tools {

class ConvertEncoding
{
public:	
	static std::string convert( const std::string & s, const std::string & from, const std::string & to );
	static bool convert( const std::string & s, const std::string & from, const std::string & to, std::string & result, std::string & error );

	static inline std::string latin12utf8( const std::string & s ) {
	    return convert( s, "LATIN1", "UTF-8" );
	}

	static inline std::string utf82latin1( const std::string & s ) {
	    return convert( s, "UTF-8", "LATIN1" );
	}
};
  
} // namespace Tools

#endif /* CPPUTILS_IO_CONVERT_ENCODING_H_ */
