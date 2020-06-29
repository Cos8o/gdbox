#ifndef _GEOMETRYDASH_HPP
#define _GEOMETRYDASH_HPP

#include <string>

namespace GeometryDash
{
	bool init(void* cb);
	bool showMessageBox(
		std::string const& title,
		std::string const& text,
		std::string const& button1 = "Ok",
		std::string const& button2 = "");
	void showMenu(void* pthis);
}

#endif /* _GDTYPES_HPP */