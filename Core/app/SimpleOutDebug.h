/*
 * @author Copyright (c) 2023 Martin Oberzalek
 */


#ifndef SIMPLE_OUTDEBUG_H_
#define SIMPLE_OUTDEBUG_H_

#include "CpputilsDebug.h"

class SimpleOutDebug : public Tools::Debug
{
private:
	std::string prefix;
	bool print_line_and_file_info;

public:
	SimpleOutDebug();
	void setPrefix( const std::string & prefix_ ) { prefix = prefix_; }
	void setPrintLineAndFileInfo( bool print_line_and_file_info_ ) {
		print_line_and_file_info = print_line_and_file_info_;
	}

	void add( const char *file, unsigned line, const char *function, const std::string & s ) override;
	void add( const char *file, unsigned line, const char *function, const std::wstring & s ) override;
};

#endif /* OUTDEBUG_H_ */
