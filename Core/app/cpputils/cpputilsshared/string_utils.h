/**
 * std::string and std::wstring utilty functions
 * @author Copyright (c) 2001 - 2023 Martin Oberzalek
 */

#ifndef TOOLS_string_utils_h
#define TOOLS_string_utils_h

#include <string>
#include <vector>
#include <functional>

#define HAVE_STL_SSTREAM

#if __GNUC__ == 2
#undef HAVE_STL_SSTREAM
#endif

#ifdef HAVE_STL_SSTREAM
#  include <sstream>
#  include <cctype>
#  define strstream stringstream
#  define ENDS
#else
#  include <ctype.h>
#  include <strstream>
#  define ENDS << std::ends;
#  define STRSTREAM
#endif  

namespace Tools {

std::string toupper( std::string s );
std::wstring toupper( std::wstring s );

std::string tolower( std::string s );
std::wstring tolower( std::wstring s );

std::string strip( const std::string& str, const std::string& what = " \t\n\r" );
std::wstring strip( const std::wstring& str, const std::wstring& what = L" \t\n\r" );

#if __cplusplus >= 201703L
std::string_view strip_view( const std::string_view & str, const std::string_view & what = " \t\n\r" );
std::wstring_view strip_view( const std::wstring_view & str, const std::wstring_view & what = L" \t\n\r" );
#endif

std::string strip_leading( const std::string& str, const std::string& what = " \t\n\r" );
std::wstring strip_leading( const std::wstring& str, const std::wstring& what = L" \t\n\r" );

std::string strip_trailing( const std::string& str, const std::string& what = " \t\n\r" );
std::wstring strip_trailing( const std::wstring& str, const std::wstring& what = L" \t\n\r" );

bool is_int( const std::string &s );
bool is_int( const std::wstring &s );

std::vector<std::string> split_simple( std::string str, std::string seperator = " \t\n", int max = -1 );
std::vector<std::wstring> split_simple( std::wstring str, std::wstring seperator = L" \t\n", int max = -1 );

std::vector<std::string> split_string( std::string str, std::string seperator, int max = -1 );
std::vector<std::wstring> split_string( std::wstring str, std::wstring seperator, int max = -1 );

std::vector<std::string> split_and_strip_simple( std::string str, const std::string & sep = " \t\n", int max = -1 );
std::vector<std::wstring> split_and_strip_simple( std::wstring str, const std::wstring & sep = L" \t\n", int max = -1 );

#if __cplusplus >= 201703L
std::vector<std::string_view> split_and_strip_simple_view( const std::string_view & str, const std::string_view & sep = " \t\n", int max = -1 );
std::vector<std::wstring_view> split_and_strip_simple_view( const std::wstring_view & str, const std::wstring_view & sep = L" \t\n", int max = -1 );
#endif

inline bool is_bool( const bool &b ) { return true; }
template<class T> bool is_bool( const T &t ) { return false; }


inline bool get_bool( const bool & b ) { return b; }

// dummy function
template<class T> T get_bool( bool b ) { return T(); }


bool s2bool( const std::string &s );
bool s2bool( const std::wstring &s );

/// convert a string to anything
template <class T> T s2x( const std::string& s )
{
    if( is_bool( T() ) )
    {
      bool b = s2bool( s );

      return get_bool<T>(b);
    }

  std::strstream str;

  str << s ENDS;
  T t;
  str >> t;

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return t;
}


/// convert a string to anything
template <class T> T s2x( const std::string& s, const T & init )
{
    if( is_bool( T() ) )
    {
	return s2bool( s );
    }

  std::strstream str;
  
  str << s ENDS;
  T t(init);
  str >> t;

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return t;
}

#if __cplusplus >= 201103
/// convert a string to anything
template <class T> T s2x( const std::wstring& s, const T & init )
{
    if( is_bool( T() ) )
    {
	return s2bool( s );
    }

  std::wstringstream str;

  str << s;
  T t(init);
  str >> t;

  return t;
}
#endif

/// converts anything to a string
template<class T>std::string x2s( T what )
{
  std::strstream str;
  
  str << what ENDS;
  std::string s( str.str() );

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return s;
}

#if __cplusplus >= 201103
/// converts anything to a string
template<class T> std::wstring x2ws( T what )
{
  std::wstringstream str;

  str << what;

  return str.str();
}
#endif

std::string x2s( const bool b );


std::string text_right_format( std::string text, unsigned int max_size, unsigned int spaces );

inline std::string auto_linebreak( std::string text, unsigned int max_size = 90 )
{
  return text_right_format( text, max_size, 0 );
}

std::string substitude( const std::string & str, const std::string & what, const std::string & with, std::string::size_type start = 0 );
std::wstring substitude( const std::wstring & str, const std::wstring & what, const std::wstring & with, std::wstring::size_type start = 0 );
 
std::string prepand( std::string str, std::string what, std::string prefix );

bool icase_cmp( const std::string &a, const std::string &b );
bool icase_cmp( const std::wstring &a, const std::wstring &b );

std::string bin_encode( const std::string &s );
std::string bin_decode( const std::string &s );

std::string group_thousand( const std::string &s, unsigned digit = 3, const std::string &sep = "." );

std::string escape( const std::string &s );

std::vector<std::string> split_safe( const std::string &s, const std::string &sep = " \n\t");
std::vector<std::wstring> split_safe( const std::wstring &s, const std::wstring &sep = L" \n\t");

template <class T> std::string createInStatement( const T & list )
{
  std::string res;

  if( list.empty() )
	return res;

  for( typename T::const_iterator it = list.begin(); it != list.end(); it++ )
	{
	  if( !res.empty() )
		  res += ", ";

	  res += "'";
	  res += x2s(*it);
	  res += "'";		
	}  

  return "(" + res + ")";
}

/**
 * template <class T> std::string IterableToFormattedString (
 *		const T & list,
 *		const std::string &sep = ", ",
 *		const std::string &lineSep = "\n",
 *		unsigned int lineEle = 5,
 *		unsigned int maxEle = 10,
 *		const std::string &strFurtherEle = " (...)")
 *
 * Creates a formatted string for the given iterable container
 *
 * PARAMS:
 * 	const T & list
 * 		the collection containing the elements
 * 	const std::string &eleSep
 * 		the element separator, defaults to ", "
 * 	const std::string &lineSep
 * 		the line separator, defaults to "\n"
 * 	unsigned int lineEle
 * 		elements in one line, defaults to 5
 * 	unsigned int maxEle
 * 		number of elements that shall be listed, defaults to 10
 * 	const std::string &strFurtherEle
 * 		string that is added if there are more than max. elements,
 * 		defaults to "(...)"
 *
 * RETURNS:
 *  The formatted string
 *
 * EXAMPLES:
 * 1.
 *		IterableToFormattedString<set<long> >(sWrongStatusKsNr, "; ", "\n", 3);
 *		=> generates OUTPUT:
 *		81167; 87345; 87346
 *		87347; 87348; 87404
 * 		87424; 87425; 87464
 * 		87527; 87604; 87624 (...)
 * 2.
 * 		IterableToFormattedString<set<long> >(sWrongStatusKsNr);
 * 		=> generates OUTPUT:
 *		81167, 87345, 87346, 87347, 87348
 *		87404, 87424, 87425, 87464, 87527
 *		87604, 87624 (...)
 *
 */
template <class T, class t_std_string, class convertFunc> t_std_string IterableToFormattedStringT (
		const T & list,
		const t_std_string & eleSep,
		const t_std_string & lineSep,
		unsigned int lineEle,
		unsigned int maxEle,
		const t_std_string & strFurtherEle,
        convertFunc x2sfunc)
{

	t_std_string res;

	if (list.empty()) {
		return res;
	}

	unsigned int cnt = 0;
	for (typename T::const_iterator it = list.begin(); it != list.end(); it++) {

		if (!res.empty()) {
			res += eleSep;
			if( lineEle != static_cast<unsigned int>(-1) )  {
				if (cnt % lineEle == 0) {
					res += lineSep;
				}
			}
		}
		res += x2sfunc(*it);
		cnt++;

		if( maxEle != static_cast<unsigned int>(-1) ) {
			if (cnt >= maxEle) {
				res += strFurtherEle;
				break;
			}
		}

	}
	return res;
}

template <class T> std::string IterableToFormattedString (
		const T & list,
		const std::string &eleSep = ", ",
		const std::string &lineSep = "\n",
		unsigned int lineEle = 5,
		unsigned int maxEle = 10,
		const std::string &strFurtherEle = " (...)")
{
	return IterableToFormattedStringT<T,std::string>(list, eleSep, lineSep, lineEle, maxEle, strFurtherEle,
			[]( const typename T::value_type & value ) { return x2s( value ); });
}

template <class T> std::wstring IterableToFormattedWString (
		const T & list,
		const std::wstring &eleSep = L", ",
		const std::wstring &lineSep = L"\n",
		unsigned int lineEle = 5,
		unsigned int maxEle = 10,
		const std::wstring &strFurtherEle = L" (...)")
{
	return IterableToFormattedStringT<T,std::wstring>(list, eleSep, lineSep, lineEle, maxEle, strFurtherEle,
			[]( const typename T::value_type & value ) { return x2ws( value ); });
}

template <class T> std::string IterableToCommaSeparatedString( const T & list, 
															 const std::string &eleSep = ", " ) {
	return IterableToFormattedString<T>( list, eleSep, "", static_cast<unsigned int>(-1), static_cast<unsigned int>(-1), "" );
}															 


std::string fill_leading( std::string s, const std::string fill_sign, unsigned int len );
std::wstring fill_leading( std::wstring s, const std::wstring fill_sign, unsigned int len );

std::string fill_trailing( std::string s, const std::string fill_sign, unsigned int len );
std::wstring fill_trailing( std::wstring s, const std::wstring fill_sign, unsigned int len );

bool is_empty_str( const char *pcString );
bool is_empty_str( const std::string & string );

bool is_empty_str( const wchar_t *pcString );
bool is_empty_str( const std::wstring & string );

inline bool is_empty_string( const char *pcString ) { return is_empty_str( pcString ); }
inline bool is_empty_string(  const std::string & s ) {  return is_empty_str( s ); }

inline bool is_empty_string( const wchar_t *pcString ) { return is_empty_str( pcString ); }
inline bool is_empty_string(  const std::wstring & s ) {  return is_empty_str( s ); }


/**
 * find all occurences of the needle in the haystack
 * and passes the found position to the func()
 */
template <class t_std_string>
void find_all_of_t( const t_std_string & haystack,
				  const t_std_string & needle,
				  std::function< bool(typename t_std_string::size_type) > func )
{
	typename t_std_string::size_type start = 0;
	typename t_std_string::size_type pos;

	while( (pos = haystack.find( needle, start ) ) != t_std_string::npos ) {
		if( !func( pos ) ) {
			return;
		}

		start = pos + needle.size();
	}
}

inline void find_all_of( const std::wstring & haystack,
				  	  	 const std::wstring & needle,
						 std::function< bool(typename std::wstring::size_type) > func )
{
	find_all_of_t<std::wstring>( haystack, needle, func );
}

inline void find_all_of( const std::string & haystack,
				  	  	 const std::string & needle,
						 std::function< bool(typename std::string::size_type) > func )
{
	find_all_of_t<std::string>( haystack, needle, func );
}

template <class container, class t_std_string>
container find_all_of_t( const t_std_string & haystack,
				  	   const t_std_string & needle )
{
	container res;
	Tools::find_all_of( haystack, needle,
			[&res]( typename t_std_string::size_type pos ) {
				res.push_back( pos );
				return true;
			});

	return res;
}

template <class container>
container find_all_of( const std::string & haystack,
				  	   const std::string & needle )
{
	return find_all_of_t<container>( haystack, needle );
}

template <class container>
container find_all_of( const std::wstring & haystack,
				  	   const std::wstring & needle )
{
	return find_all_of_t<container>( haystack, needle );
}

} // namespace Tools

#undef STRSTREAM
#undef strstream
#undef ENDS

#endif
