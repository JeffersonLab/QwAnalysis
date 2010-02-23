#ifndef QWCOLOR_H
#define QWCOLOR_H

// System headers
#include <map>
#include <cstdio>
#include <iostream>

// This header contains the terminals color codes.
// This might be useful to distingush many output of QwAnalysis into
// terminals.

// Simply use printf function
// printf("%s Bold text print  %s\n", BOLD, NORMAL);
// printf("%s RED text %s\n", RED, NORMAL);

// detailed information
// see http://www.faqs.org/docs/abs/HTML/colorizing.html
//     http://linuxgazette.net/issue65/padala.html

#define BLACK     "\E[30m"
#define RED       "\E[31m"
#define GREEN     "\E[32m"
#define BROWN     "\E[33m"
#define BLUE      "\E[34m"
#define MAGENTA   "\E[35m"
#define CYAN      "\E[36m"
#define WHITE     "\E[37m"

#define BOLD        "\033[1m"

#define BOLDRED     "\E[31m\033[1m"
#define BOLDGREEN   "\E[32m\033[1m"
#define BOLDBROWN   "\E[33m\033[1m"
#define BOLDBLUE    "\E[34m\033[1m"
#define BOLDMAGENTA "\E[35m\033[1m"
#define BOLDCYAN    "\E[36m\033[1m"
#define BOLDWHITE   "\E[37m\033[1m"

#define BACKRED     "\E[31m\033[7m"
#define BACKGREEN   "\E[32m\033[7m"
#define BACKBLUE    "\E[34m\033[7m"
#define NORMAL      "\033[0m"

// All colors need to go in a namespace because of conflicts with ROOT colors.
// Really, ROOT should have its colors in a namespace.
namespace Qw {

  enum EQwColor {
    kBlack, kRed, kGreen, kBrown, kBlue, kMagenta, kCyan, kWhite, kBold,
    kBoldRed, kBoldGreen, kBoldBrown, kBoldBlue, kBoldMagenta, kBoldCyan, kBoldWhite,
    kBackRed, kBackGreen, kBackBlue, kNormal
  };

}; // namespace Qw

typedef std::map<Qw::EQwColor, std::string> QwColorMap;

class QwColor
{
  public:

    QwColor(const Qw::EQwColor f = Qw::kWhite, const Qw::EQwColor b = Qw::kBlack)
    : foreground(f), background(b) { };
    virtual ~QwColor() { };

  friend std::ostream& operator<<(std::ostream& out, const QwColor& color);

  protected:

    static QwColorMap CreateColorMap() {
      QwColorMap map;
      map[Qw::kBlack] = BLACK;
      map[Qw::kRed] = RED;
      map[Qw::kGreen] = GREEN;
      map[Qw::kBrown] = BROWN;
      map[Qw::kBlue] = BLUE;
      map[Qw::kMagenta] = MAGENTA;
      map[Qw::kCyan] = CYAN;
      map[Qw::kBoldRed] = BOLDRED;
      map[Qw::kBoldGreen] = BOLDGREEN;
      map[Qw::kBoldBrown] = BOLDBROWN;
      map[Qw::kBoldBlue] = BOLDBLUE;
      map[Qw::kBoldMagenta] = BOLDMAGENTA;
      map[Qw::kBoldCyan] = BOLDCYAN;
      map[Qw::kWhite] = WHITE;
      return map;
    };
    static QwColorMap kColorMap;

  private:

    Qw::EQwColor foreground;
    Qw::EQwColor background; // not yet supported

}; // class QwColor

inline std::ostream& operator<<(std::ostream& out, const QwColor& color)
{
  return out << color.kColorMap[color.foreground];
}

#endif // QWCOLOR_H
