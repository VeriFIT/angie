#include "OsUtils.hpp"

#include <cstdlib>
#include <fstream>

#if defined _WIN32
#include <Windows.h>
#endif

namespace OsUtils {

std::string GetEnv(boost::string_view str)
{
  return{std::getenv(str.data())};
}

void Exec(boost::string_view str)
{
  std::system(str.data());
}

void PasteToClipboard(boost::string_view str)
{
  #if ! defined _WIN32
  std::runtime_error("PasteToClipboard is only supported on Windows");
  #else
  if (!OpenClipboard(0))
  {
    std::runtime_error("Cannot open the Clipboard");
    return;
  }
  // Remove the current Clipboard contents
  if (!EmptyClipboard())
  {
    std::runtime_error("Cannot empty the Clipboard");
    return;
  }
  // Get the currently selected data
  HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, str.size() + 20);
  std::memcpy((char*)hGlob, str.data(), str.size());
  *((char*)hGlob + str.size()) = 0;

  // For the appropriate data formats...
  if (SetClipboardData(CF_TEXT, hGlob) == NULL)
  {
    CloseClipboard();
    GlobalFree(hGlob);
    std::runtime_error("Unable to set Clipboard data");
    return;
  }
  CloseClipboard();
  #endif
}

void WriteToFile(boost::string_view str, boost::string_view filename)
{
  std::ofstream file(filename.data(), std::ios_base::trunc);
  bool check = file.is_open();
  if (!check)
    std::runtime_error("failed to open the file for writing");

  file.write(str.data(), str.size());
  file.close();
}

}