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

//  Also see a listing of the full ANSI escape sequences at:
//     http://en.wikipedia.org/wiki/ANSI_escape_code

//  Note that you may replace "\E" with "\033" as the
//  escape character

#define BLACK       "\E[30m"
#define RED         "\E[31m"
#define GREEN       "\E[32m"
#define BROWN       "\E[33m"
#define BLUE        "\E[34m"
#define MAGENTA     "\E[35m"
#define CYAN        "\E[36m"
#define WHITE       "\E[37m"

#define BOLD        "\E[1m"
/*  Code "\E[2m" sets the intensity to faint, but it is not
 *  universally supported.
 *  Code "\E[22m" sets the intensity to normal (not bold or faint).
 */

/*  The bolded codes below could alternately be achieved by
 *  two codes;  i.e. BOLDRED could be done by the concatenation
 *  of BOLD and RED.
 */
#define BOLDRED     "\E[31;1m"
#define BOLDGREEN   "\E[32;1m"
#define BOLDBROWN   "\E[33;1m"
#define BOLDBLUE    "\E[34;1m"
#define BOLDMAGENTA "\E[35;1m"
#define BOLDCYAN    "\E[36;1m"
#define BOLDWHITE   "\E[37;1m"

#define BACKRED     "\E[41m"
#define BACKGREEN   "\E[42m"
#define BACKBLUE    "\E[44m"

/*  Code "\E[0m" clears all formatting codes.
 */
#define NORMAL      "\E[0m"

// All colors need to go in a namespace because of conflicts with ROOT colors.
// Really, ROOT should have its colors in a namespace.
namespace Qw {

  enum EQwColor {
    kBlack, kRed, kGreen, kBrown, kBlue, kMagenta, kCyan, kWhite, kDefaultForeground,
    kBold,
    kBoldRed, kBoldGreen, kBoldBrown, kBoldBlue, kBoldMagenta, kBoldCyan, kBoldWhite,
    kBackRed, kBackGreen, kBackBlue, kDefaultBackground,
    kNormal
  };

}; // namespace Qw

// Map of the enum to the defined strings
typedef std::map<Qw::EQwColor, std::string> QwColorMap;

/**
 *  \class QwColor
 *  \ingroup QwAnalysis
 *  \brief A color changing class for the output stream
 *
 * This class changes the color in the output stream by inserting 'magic'
 * escape sequences that are interpreted by smart terminals.  On dumb terminals
 * this will probably just print garbage.  To use the color, a QwColor object
 * should be streamed to the output stream:
 * \code
 * QwMessage << "Hello, " << QwColor(Qw::kRed) << "Qweak!" << QwLog::endl;
 * \endcode
 */
class QwColor
{
  public:

    /// Default constructor
    QwColor(const Qw::EQwColor f = Qw::kDefaultForeground, const Qw::EQwColor b = Qw::kDefaultBackground)
    : foreground(f), background(b) { };
    virtual ~QwColor() { };

  /// \brief Output stream operator
  friend std::ostream& operator<<(std::ostream& out, const QwColor& color);

  protected:

    static QwColorMap CreateColorMap() {
      QwColorMap map;
      map[Qw::kBlack]   = BLACK;
      map[Qw::kRed]     = RED;
      map[Qw::kGreen]   = GREEN;
      map[Qw::kBrown]   = BROWN;
      map[Qw::kBlue]    = BLUE;
      map[Qw::kMagenta] = MAGENTA;
      map[Qw::kCyan]    = CYAN;
      map[Qw::kWhite]   = WHITE;
      //
      map[Qw::kBold]    = BOLD;
      //
      map[Qw::kBoldRed]     = BOLDRED;
      map[Qw::kBoldGreen]   = BOLDGREEN;
      map[Qw::kBoldBrown]   = BOLDBROWN;
      map[Qw::kBoldBlue]    = BOLDBLUE;
      map[Qw::kBoldMagenta] = BOLDMAGENTA;
      map[Qw::kBoldCyan]    = BOLDCYAN;
      map[Qw::kBoldWhite]   = BOLDWHITE;
      //
      map[Qw::kBackRed]   = BACKRED;
      map[Qw::kBackGreen] = BACKGREEN;
      map[Qw::kBackBlue]  = BACKBLUE;
      //
      map[Qw::kDefaultForeground]   = "\E[39m";
      map[Qw::kDefaultBackground]   = "\E[49m";
      //
      map[Qw::kNormal] = NORMAL;
      return map;
    };
    static QwColorMap kColorMap;

  private:

    Qw::EQwColor foreground; ///< Foreground color
    Qw::EQwColor background; ///< Background color (not yet supported)

}; // class QwColor

/// Output stream operator which uses the enum-to-escape-code mapping
inline std::ostream& operator<<(std::ostream& out, const QwColor& color)
{
  return out << color.kColorMap[color.foreground];
}

#endif // QWCOLOR_H
