#ifndef ISE102_H
#define ISE102_H

//#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <string>
#include <algorithm>
#include "scn/all.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/color.h"
// Detect target's platform and set some macros in order to wrap platform
// specific code this library depends on.
// Platform check borrowed from termcolor.hpp, github.com/ikalnytskyi
#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#   define TARGET_POSIX
#elif defined(_WIN32) || defined(_WIN64) // || defined _MINGW64, a flag I've added in the makefile (dmac)
#   define TARGET_WINDOWS
#endif

#if defined(TARGET_POSIX)
// Use conio_linux.h on LINUX (including replit)
#   include "conio_linux.h"
#elif defined(TARGET_WINDOWS)
//Use conio.h on WINDOWS (Visual Studio Community, open the sln file)
# include <conio.h>
#endif
//using namespace std;
using fmt::print;
using fmt::format;
using fmt::emphasis;
using fmt::fg;
using fmt::bg;
using scn::input;
using scn::prompt;
using fmt::color;
using std::string;
using namespace std::literals::chrono_literals;

string strToUpper(string& str)
{
  string upper_str = str;
  std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
  return upper_str;
}

std::string strToUpper(const char* str)
{
  string upper_str = string(str);
  std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);
  return upper_str;
}

void delay(int duration_ms)
{
  fflush(stdout); // the terminal buffer doesn't flush/ouput before sleep is called on linux
  std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
}

void clearScreen()
{
#if defined TARGET_POSIX 
  // Assume POSIX
  std::system("clear");
  print("\n linux clearscreen \n");
#else
  std::system("clear");  // in case we're in mingw64, which identifies as windows but can't see cls (even with an ln -s)
  std::system("cls");
  //print("\n windows clearscreen \n");
#endif
}

// Creates a text style using the fmt library. This consists of a foreground colour, background colour and
// (optional) emphasis: bold, italic, underline etc. These only work on linux/posix terminals like those 
// on replit and MacOs. 
fmt::text_style makeStyle(fmt::color foreground, fmt::color background,fmt::emphasis emph = fmt::emphasis(0))
{
  return emph | fg(foreground) | bg (background);
}

#endif  // ISE102_H