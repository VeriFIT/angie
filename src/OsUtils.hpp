#pragma once

#include <boost/utility/string_view.hpp>

#if defined _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

namespace OsUtils {
std::string GetEnv(boost::string_view str);
void Exec(boost::string_view str);
void PasteToClipboard(boost::string_view str);
void WriteToFile(boost::string_view str, boost::string_view file);
};
