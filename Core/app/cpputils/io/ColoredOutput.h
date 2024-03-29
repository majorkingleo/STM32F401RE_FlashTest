/**
 * class for printing console colours
 * @author Copyright (c) 2019 - 2023 Martin Oberzalek
 */
#ifndef COLORED_OUTPUT_H_
#define COLORED_OUTPUT_H_

#include <string>

namespace Tools {

/*
 * class to color a string on the console.
 * On windows it modifies the console, so cross check
 * if you use this in any combination with a curses library
 */
class ColoredOutput
{
	bool colored_output;

public:
	enum Color {
		BLACK,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE,
		BRIGHT_BLACK,
		BRIGHT_RED,
		BRIGHT_GREEN,
		BRIGHT_YELLOW,
		BRIGHT_BLUE,
		BRIGHT_MAGENTA,
		BRIGHT_CYAN,
		BRIGHT_WHITE,
	};
public:
	ColoredOutput();

	std::string color_output( Color color, const std::string & text ) const;

	void setColoredOutput( bool colored_output_ ) {
			colored_output = colored_output_;
	}
};

} // namespace Tools

#endif /* COLORED_OUTPUT_H_ */
