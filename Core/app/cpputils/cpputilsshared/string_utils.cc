/**
 * std::string and std::wstring utilty functions
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 */

#include "string_utils.h"
#include "format.h"
#include <cctype>
#include <cwctype>
#include <algorithm>

namespace Tools {

std::string toupper( std::string s )
{
  std::transform( s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

std::wstring toupper( std::wstring s )
{
  std::transform( s.begin(), s.end(), s.begin(), std::towupper);
  return s;
}

std::string tolower( std::string s )
{
  std::transform( s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

std::wstring tolower( std::wstring s )
{
  std::transform( s.begin(), s.end(), s.begin(), std::towlower);
  return s;
}


bool is_int( const std::string &s )
{
  if( s.empty() )
    return false;

  for( unsigned int i = 0; i < s.size(); ++i )
    {
      switch( s[i] )
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  break;

	default:
	  return false;
	}
    }

  return true;
}

bool is_int( const std::wstring &s )
{
	if( s.empty() ) {
		return false;
	}

	for( unsigned int i = 0; i < s.size(); ++i )
	{
		switch( s[i] )
		{
		case L'0':
		case L'1':
		case L'2':
		case L'3':
		case L'4':
		case L'5':
		case L'6':
		case L'7':
		case L'8':
		case L'9':
			break;

		default:
			return false;
		}
	}

	return true;
}

/*
  Re: Trim Funktion f�r Strings
  Von: Hubert Schmid <h.schmid-usenet@gmx.de>
  Datum:  Sonntag, 10. Oktober 2004 14:13:35
  Gruppen:  de.comp.lang.iso-c++
  Message-ID:  <87acuug49c.fsf@dent.z42.de>
*/


namespace {

template<class t_std_string,
         class t_std_string_search = t_std_string> class TStrip {
public:

	t_std_string strip( const t_std_string& str, const t_std_string& what )
	{
		typename t_std_string::size_type p = str.find_first_not_of(what);

		if( p == std::string::npos )
		{
			return t_std_string();

		} else {

			typename t_std_string::size_type q = str.find_last_not_of(what);
            typename t_std_string::size_type len = q - p + 1;

			return t_std_string(&(*str.begin()) + p, len );
		}
	}
};
} // namespace

std::string strip( const std::string& str, const std::string& what ) {
	TStrip<std::string> tstrip;
	return tstrip.strip( str, what );
}

std::wstring strip( const std::wstring& str, const std::wstring& what ) {
	TStrip<std::wstring> tstrip;
	return tstrip.strip( str, what );
}

#if __cplusplus >= 201703L

std::string_view strip_view( const std::string_view & str, const std::string_view & what ) {
  TStrip<std::string_view> tstrip;
  return tstrip.strip( str, what );
}

std::wstring_view strip_view( const std::wstring_view & str, const std::wstring_view & what ) {
  TStrip<std::wstring_view> tstrip;
  return tstrip.strip( str, what );
}

#endif

std::string strip_leading( const std::string& str, const std::string& what )
{
    std::string::size_type p = str.find_first_not_of(what);
    
    if( p == std::string::npos )
    {
	  return std::string();
    } else {
	  return std::string(str, p);
    }
}

std::wstring strip_leading( const std::wstring& str, const std::wstring& what )
{
    std::wstring::size_type p = str.find_first_not_of(what);

    if( p == std::wstring::npos )
    {
	  return std::wstring();
    } else {
	  return std::wstring(str, p);
    }
}

std::string strip_trailing( const std::string& str, const std::string& what )
{
    std::string::size_type p = str.find_last_not_of(what);
    
    if( p == std::string::npos )
    {
	  return std::string();
    } else {
	  return str.substr(0,p+1);
    }
}

std::wstring strip_trailing( const std::wstring& str, const std::wstring& what )
{
    std::wstring::size_type p = str.find_last_not_of(what);

    if( p == std::wstring::npos )
    {
	  return std::wstring();
    } else {
	  return str.substr(0,p+1);
    }
}

std::string text_right_format( std::string s, unsigned int max_size, unsigned int spaces )
{
  if( max_size > s.size() && spaces == 0 )
	return s;

  std::string space;
  
  for( unsigned int index = 0; index < spaces; ++index )
    space += ' ';
  
  int i;
  
  for( i = max_size; i >= 0 && s[i] != ' '; --i );
  
  std::string ss = s.substr( i + 1 );
  s = s.substr( 0, i );
 
  bool has_new_line = false;
 
  while( !ss.empty() )
    {
      for( i = static_cast<int>(max_size) - spaces; i < static_cast<int>(ss.size()) && i >= 0 && ss[i] != ' '; --i );
      
      s += '\n';
      s += space;      

      s += ss.substr( 0, i );
      
      has_new_line = true;

      if( i >= static_cast<int>(ss.size()) || i < 0 )
	break;
      
      ss = ss.substr( i + 1 );
    }

  if( has_new_line )
    s +='\n';

  return s;
}

namespace {

template<class t_std_string> class TSplitSimple {
public:
	std::vector<t_std_string> split_simple( t_std_string str, t_std_string sep, int max )
	{
		str = strip( str, sep );

		typename t_std_string::size_type start = 0, last = 0;
		int count = 0;

		std::vector<t_std_string> sl;

		while( true )
		{
			if( max > 0 ) {
				count++;
			}

			if( count >= max && max > 0 )
			{
				sl.push_back( str.substr( last ) );
				break;
			}


			start = str.find_first_of( sep, last );

			if( start == std::string::npos )
			{
				sl.push_back( str.substr( last ) );
				break;
			}

			sl.push_back( str.substr( last, start - last ) );

			last = start + 1;
		}

		return sl;
	}
};
} // namespace

std::vector<std::string> split_simple( std::string str, std::string sep, int max )
{
	TSplitSimple<std::string> tsplit;
	return tsplit.split_simple( str, sep, max );
}

std::vector<std::wstring> split_simple( std::wstring str, std::wstring sep, int max )
{
	TSplitSimple<std::wstring> tsplit;
	return tsplit.split_simple( str, sep, max );
}

namespace {

template<class t_std_string> class TSplitString {
public:
	std::vector<t_std_string> split_string( t_std_string str, t_std_string sep, int max  )
	{
		typename t_std_string::size_type start = 0, last = 0;
		int count = 0;

		std::vector<t_std_string> sl;

		while( true )
		{
			if( max > 0 )
				count++;

			if( count >= max && max > 0 )
			{
				sl.push_back( str.substr( last ) );
				break;
			}


			start = str.find( sep, last );

			if( start == t_std_string::npos )
			{
				sl.push_back( str.substr( last ) );
				break;
			}

			sl.push_back( str.substr( last, start - last ) );

			last = start + sep.size();
		}

		return sl;
	}
}; // class TSplitString
} // namespace

std::vector<std::string> split_string( std::string str, std::string sep, int max  )
{
	TSplitString<std::string> tsplit;
	return tsplit.split_string( str, sep, max );
}

std::vector<std::wstring> split_string( std::wstring str, std::wstring sep, int max  )
{
	TSplitString<std::wstring> tsplit;
	return tsplit.split_string( str, sep, max );
}
/*
bool s2x( const std::string &s )
{
  if( s == "0" || toupper( s ) == "FALSE" )
    return false;
  
  if( s == "1" || toupper( s ) == "TRUE" )
    return true;
  
  out( "Warning: illegal value for boolean state: %s\n", s );

  return false;
}
*/

bool s2bool( const std::string &s )
{
    if( s == "1" || toupper( s ) == "TRUE" )
	return true;

    return false;
}

bool s2bool( const std::wstring &s )
{
    if( s == L"1" || toupper( s ) == L"TRUE" )
	return true;

    return false;
}

std::string x2s( bool b )
{
  if( b )
    return "TRUE";

  return "FALSE";
}

namespace {

template<class t_std_string> class TSubstitude {
public:

	t_std_string substitude( const t_std_string &str_orig,
							 const t_std_string &what,
							 const t_std_string &with,
							 typename t_std_string::size_type start ) {
		t_std_string str(str_orig);
		typename t_std_string::size_type pos = start;

		if (what.empty()) {
			return str;
		}

		for (;;) {
			pos = str.find(what, pos);
			if (pos == std::string::npos) {
				break;
			}

			if (with.empty()) {
				t_std_string s = str.substr(0, pos);
				s += str.substr(pos + what.size());
				str = s;
				continue;
			} else {
				str.replace(pos, what.size(), with);
			}

			pos += with.size();
		}
		return str;
	}
};

} // namespace

std::string substitude(const std::string &str_orig, const std::string &what,
		               const std::string &with, std::string::size_type start) {
	TSubstitude<std::string> subst;
	return subst.substitude(str_orig, what, with, start);
}

std::wstring substitude(const std::wstring &str_orig, const std::wstring &what,
					    const std::wstring &with, std::wstring::size_type start) {
	TSubstitude<std::wstring> subst;
	return subst.substitude(str_orig, what, with, start);
}
  
std::string prepand( std::string str, std::string what, std::string prefix )
{
    std::string::size_type pos = 0, start1 = 0, start2 = 0, start = 0;

    for(;;)
    {
	pos = str.find( what, pos );
	
	if( pos == std::string::npos )
	    break;

	start1 = str.rfind( '\"', pos );
	start2 = str.rfind( '=', pos );              

	if( start1 == std::string::npos && start2 != std::string::npos )
	    start = start2;
	else if( start2 == std::string::npos && start1 != std::string::npos )
	    start = start1;
	else if( start1 == std::string::npos && start2 == std::string::npos )	    
	    break;
	else if( start1 > start2 )
	    start = start1;
	else
	    start = start2;

	str.insert( start + 1, prefix );
	pos += prefix.size();
	pos++;
    }

    return str;
}

std::string bin_encode( const std::string &s )
{
    std::string ret;

    for( unsigned i = 0; i < s.size(); i++ )
    {
	ret += format( "%03d", static_cast<int>( s[i] ) );
    }

    return ret;
}

std::string bin_decode( const std::string &s )
{
    std::string ret;

    for( unsigned i = 0; i < s.size(); i += 3 )
    {
	std::string ss = s.substr( i, 3 );
	int x = s2x<int>( ss );
	ret += static_cast<char>(x);
    }

    return ret;
}

std::string group_thousand( const std::string &s, unsigned digit, const std::string &sep )
{
    if( s.empty() )
	return std::string();

    std::string ret;       

    for( int i = static_cast<int>(s.size()) - 1, count = 0; i >= 0; i--, count++ )
    {
	if( count > 0 && count % digit == 0 )
	    ret += sep;
	  
	ret += s[i];
    }

    std::string sret;

    sret.resize( ret.size() );

    for( int i = static_cast<int>(ret.size()) - 1, count = 0; i >= 0; i--, count++ )
    {
	sret[count] = ret[i];
    }

    return sret;
}

std::string escape( const std::string &s )
{
    return substitude( s, "'", "\\'" );
}

template <class character_type> struct Pair
{
  typename std::basic_string<character_type>::size_type start;
  typename std::basic_string<character_type>::size_type end;

  Pair( typename std::basic_string<character_type>::size_type start_, typename std::basic_string<character_type>::size_type end_ )
    : start( start_ ),
	  end( end_ )
  {}
};


static bool is_escaped( const std::string &s, std::string::size_type start )
{
	bool escaped = false;

	if( start > 0 ) {
		if( s[start-1] == '\\' ) {
			escaped = true;

			if( start > 1 )	{
				if( s[start-2] == '\\' ) {
					escaped = false;
				}
			}
		}
	}

	return escaped;
}

static bool is_escaped( const std::wstring &s, std::wstring::size_type start )
{
	bool escaped = false;

	if( start > 0 ) {
		if( s[start-1] == L'\\' ) {
			escaped = true;

			if( start > 1 ) {
				if( s[start-2] == L'\\' ) {
					escaped = false;
				}
			}
		}
	}

	return escaped;
}

template <class t_std_string>
static std::vector<Pair<typename t_std_string::value_type>> find_exclusive( const t_std_string &s )
{
	std::vector<Pair<typename t_std_string::value_type>> ex;

	std::string::size_type pos = 0, start = 0, end = 0;

	while( true )
	{

		/* find starting pos */
		while( true )
		{
			start = s.find( '"', pos );

			if( start == t_std_string::npos )
				return ex;

			/* is the " escaped? */
			if( is_escaped( s, start ) )
			{
				pos = start + 1;
				continue;
			}

			break;
		}

		// find second "
		pos = start + 1;
		while( true )
		{
			end = s.find( '"', pos );

			if( end == t_std_string::npos )
				return ex;

			if( is_escaped( s, end ) )
			{
				pos = end + 1;
				continue;
			}

			break;
		}

		ex.push_back( Pair<typename t_std_string::value_type>( start, end ) );
		pos = end + 1;
	}
}

template <class t_std_string>
static bool is_exclusive( const t_std_string &s,
						  const std::vector<Pair<typename t_std_string::value_type>> &exclude,
						  typename t_std_string::size_type pos1 )
{
	bool exclusive = false;
	for( unsigned i = 0; i < exclude.size(); i++ ) {
		if( exclude[i].start < pos1 &&
				exclude[i].end > pos1 )
		{
			exclusive = true;
			break;
		}
	}

	return exclusive;
}

/* splits
       hello "my name is" 
   correct up into 'hello' and '"my name is"'
*/
template <class t_std_string>
static std::vector<t_std_string> split_safe_int( const t_std_string &s, const t_std_string &sep )
{
  std::vector<Pair<typename t_std_string::value_type>> exclude = find_exclusive( s );

  typename t_std_string::size_type pos1 = 0, pos2 = 0;

  std::vector<t_std_string> sl;

  while( true )
  {
	  pos1 = s.find_first_of( sep, pos1 );

	  if( pos1 == t_std_string::npos )
	  {
		  sl.push_back( s.substr( pos2 ) );
		  return sl;
	  }

	  if( is_exclusive( s, exclude, pos1 ) )
	  {
		  pos1++;
		  continue;
	  }

	  sl.push_back( s.substr( pos2, pos1 - pos2 ) );
	  pos2 = pos1 + 1;
	  pos1++;
  }

  return sl;
}

std::vector<std::string> split_safe( const std::string &s, const std::string &sep )
{
	return split_safe_int( s, sep );
}

std::vector<std::wstring> split_safe( const std::wstring &s, const std::wstring &sep )
{
	return split_safe_int( s, sep );
}

template<class t_std_string>
static std::vector<t_std_string> split_and_strip_simple_int( const t_std_string & input_str, const t_std_string & sep , int max )
{
  TStrip<t_std_string> tstrip;
  t_std_string str = tstrip.strip( input_str, sep );

  std::string::size_type start = 0, last = 0;
  int count = 0;

  std::vector<t_std_string> sl;

  if( str.empty() ) {
    return sl;
  }

  while( true )
  {
    if( max > 0 )
      count++;

    if( count >= max && max > 0 )
    {
      sl.push_back( str.substr( last ) );
      break;
    }

    start = str.find_first_of( sep, last );

    if( start == t_std_string::npos )
    {
      sl.push_back( str.substr( last ) );
      break;
    }

    sl.push_back( str.substr( last, start - last ) );

    for( typename t_std_string::size_type pos = start + 1;
      pos < str.size(); pos++ )
    {
      bool found = false;

      for( typename t_std_string::size_type i = 0; i < sep.size(); i++ )
      {
        if( str[pos] == sep[i] )
        {
          found = true;
          break;				  
        }
      }

      if( found == false )
      {
        last = pos;
        break;
      }
    }

    //      last = start + 1;
  }

  return sl;
}

std::vector<std::string> split_and_strip_simple( std::string str, const std::string & sep , int max )
{
  return split_and_strip_simple_int<std::string>( str, sep, max );  
}

std::vector<std::wstring> split_and_strip_simple( std::wstring str, const std::wstring & sep , int max )
{
  return split_and_strip_simple_int<std::wstring>( str, sep, max );
}

#if __cplusplus >= 201703L
std::vector<std::string_view> split_and_strip_simple_view( const std::string_view & str, const std::string_view & sep , int max )
{
  return split_and_strip_simple_int<std::string_view>( str, sep, max );
}

std::vector<std::wstring_view> split_and_strip_simple_view( const std::wstring_view & str, const std::wstring_view & sep , int max )
{
  return split_and_strip_simple_int<std::wstring_view>( str, sep, max );
}
#endif

std::string fill_trailing( std::string s, const std::string fill_sign, unsigned int len )
{
	s.reserve(len);


	while( s.size() < len )
		s += fill_sign;

	return s;
}

std::wstring fill_trailing( std::wstring s, const std::wstring fill_sign, unsigned int len )
{
	s.reserve(len);


	while( s.size() < len )
		s += fill_sign;

	return s;
}

std::string fill_leading( std::string s, const std::string fill_sign, unsigned int len )
{
	s.reserve(len);

	while( s.size() < len )
		s = fill_sign + s;

	return s;
}

std::wstring fill_leading( std::wstring s, const std::wstring fill_sign, unsigned int len )
{
	s.reserve(len);

	while( s.size() < len )
		s = fill_sign + s;

	return s;
}


bool is_empty_str( const char *pcString )
{
  if( pcString == NULL )
	return true;

  for( int i = 0; pcString[i] != '\0'; i++ )
	{
	  if( pcString[i] != ' ' )
		return false;
	}

  return true;
}

bool is_empty_str( const std::string & string )
{
  if( string.empty() )
	return true;

  return is_empty_str( string.c_str() );
}

bool is_empty_str( const wchar_t *pcString )
{
  if( pcString == NULL )
	return true;

  for( int i = 0; pcString[i] != L'\0'; i++ )
	{
	  if( pcString[i] != L' ' )
		return false;
	}

  return true;
}

bool is_empty_str( const std::wstring & string )
{
  if( string.empty() )
	return true;

  return is_empty_str( string.c_str() );
}

bool icase_cmp( const std::string &a, const std::string &b )
{
    if( a.size() != b.size() ) {
    	return false;
    }

    if( a == b ) {
    	return true;
    }

    if( toupper( a ) == toupper( b ) ) {
    	return true;
    }

    return false;
}

bool icase_cmp( const std::wstring &a, const std::wstring &b )
{
    if( a.size() != b.size() ) {
    	return false;
    }

    if( a == b ) {
    	return true;
    }

    if( toupper( a ) == toupper( b ) ) {
    	return true;
    }

    return false;
}

} // namespace Tools
