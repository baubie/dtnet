// vt100.h
/*
 ----------------------------------------------
 Terminal macros to use in C++ console programs
 ----------------------------------------------

 Use the following macros in your C++ cout statements to manipulate the
 video display. Most systems emulate either VT100 terminals or the ANSI
 terminal protocol. If yours does not, these won't work until you find
 an ANSI terminal driver.

 The following macros are practically guaranteed to work:
   clear_screen
   audible_bell
   goto_xy()
   move_to()
   default_attributes
   set_reverse()
   set_colors()
   finalize

 The least likely to work are
   show_cursor()
   set_italic()

 .............................................................................
 Example usage:

   cout << clear_screen
        << goto_xy( 10, 2 ) << "Hello";

 To terminate your program, your code should look like this:

   cout << finalize;
   return EXIT_SUCCESS;

 .............................................................................
 T H E   M A C R O S

   cout << reset                       Restores the terminal to it's original
                                       state.
                                       You probably don't want to use this.

   cout << clear_screen                Clear the entire screen.
        << clear_to_bos                Clear from the top of the screen.
        << clear_to_eos                Clear to the bottom of the screen.

   cout << clear_to_eol                Clear to the end of the line.
        << clear_to_bol                Clear to the beginning of the line.
        << clear_line                  Clear the entire line.

   cout << visual_bell                 Blinks the screen (unreliable).
        << audible_bell                Sounds the speaker beep.

   cout << show_cursor( TRUE )         Make the cursor visible (default)
        << show_cursor( FALSE )        Make the cursor invisible

   cout << goto_xy( x, y )             Move the cursor to coordinate (x, y),
                                       or (column x, line y), where (1, 1)
                                       is the upper-left corner of the screen.
        << move_to( line, column )     Same.

   cout << cursor_up(    count )       Move the cursor "count" characters
        << cursor_down(  count )       (or lines) from its current position.
        << cursor_left(  count )
        << cursor_right( count )

   cout << set_scrolling_region( top, bottom )
                                       Set the lines in the range [top,bottom]
                                       to be those that scroll.
        << scroll_up(   count )        Scroll the region up or down "count"
        << scroll_down( count )        lines.

   cout << insert_line( count )        Insert or delete "count" items.
        << delete_line( count )        Always inserts blanks.
        << insert_char( count )        The result is affected if you change
        << delete_char( count )        the scroll region.

   cout << default_attributes          Reset so that the cursor attributes are
                                       the default (no bold, underline, etc.
                                       and colors are VT_DEFAULT).
        << set_bold( bool )            Bold text.
        << set_italic( bool )          Italic text.
        << set_underline( bool )       Underline text.
        << set_reverse( bool )         Reverse or invert the text colors.
        << set_attributes( bold, italic, underline, reverse )
                                       Set all four text attributes at once
                                       without affecting the color attributes.
        << set_colors( fg, bg )        The colors are
                                         VT_BLACK    VT_YELLOW    VT_CYAN
                                         VT_RED      VT_BLUE      VT_WHITE
                                         VT_GREEN    VT_MAGENTA   VT_DEFAULT

   cout << finalize                    Do this before your program ends.
                                       (Alias for default_attributes.)

 .............................................................................
 Lastly

   All this does is drive some basic functionality of a VT100/ANSI terminal.
   Notibly, it lacks:
    * The ability to tell you where the cursor is (so you must remember!)
    * The ability to tell you the size of the terminal screen.
      (You can use "stty size" at the command prompt to find out.)
    * The ability to get just one character from the keyboard.
      This takes some jumping through a few hoops. If you want to do some more
      advanced stuff than what is presented here, consider using the ncurses
      library (type "man ncurses" at the prompt).
      (Here at RU, you'll have to include </usr/local/include/ncurses.h>
      instead of just plain old <ncurses.h> to make curses work. Don't forget
      to compile with "g++ ... -lncurses"!)
   If you're interested in this kind of junk, type "man screen" at the prompt.

 Enjoy!

 --Michael Thomas Greer

 */

#ifndef VT100_MACRO_H
#define VT100_MACRO_H

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define VT_BLACK    0
#define VT_RED      1
#define VT_GREEN    2
#define VT_YELLOW   3
#define VT_BLUE     4
#define VT_MAGENTA  5
#define VT_CYAN     6
#define VT_WHITE    7
#define VT_DEFAULT  9

#define VT_reset "\33c"

#define VT_clear_screen "\33[2J"
#define VT_clear_to_bos "\33[1J"
#define VT_clear_to_eos "\33[J"

#define VT_clear_line   "\33[2K"
#define clear_to_bol "\33[1K"
#define clear_to_eol "\33[K"

#define VT_visual_bell  "\33g"
#define VT_audible_bell "\a"

#define VT_show_cursor( v ) ((v) ? "\33\67p" : "\33\66p")

#define VT_goto_xy( x, y ) "\33[" << y << ";" << x << "H"
#define VT_move_to( y, x ) "\33[" << y << ";" << x << "f"

#define VT_cursor_up(    count ) "\33[" << count << "A"
#define VT_cursor_down(  count ) "\33[" << count << "B"
#define VT_cursor_right( count ) "\33[" << count << "C"
#define VT_cursor_left(  count ) "\33[" << count << "D"

#define VT_set_scrolling_region( top, bottom ) "\33[" << top << ";" << bottom << "r"

#define VT_scroll_up(   count ) "\33[" << count << "S"
#define VT_scroll_down( count ) "\33[" << count << "T"

#define VT_insert_line( count ) "\33[" << count << "L"
#define VT_delete_line( count ) "\33[" << count << "M"

#define VT_insert_char( count ) "\33[" << count << "@"
#define VT_delete_char( count ) "\33[" << count << "P"

static bool is_b = FALSE;
static bool is_i = FALSE;
static bool is_u = FALSE;
static bool is_r = FALSE;
static int  fg_c = VT_DEFAULT;
static int  bg_c = VT_DEFAULT;

#define VT_default_attributes "\33[0m"

// these four following are not meant to be used externally
#define VT_set_b( b ) ((is_b = b) ? "\33[1m" : "")
#define VT_set_i( i ) ((is_i = i) ? "\33[3m" : "")
#define VT_set_u( u ) ((is_u = u) ? "\33[4m" : "")
#define VT_set_r( r ) ((is_r = r) ? "\33[7m" : "")

#define VT_set_colors( fg, bg ) "\33[3" << (fg_c = fg) << ";4" << (bg_c = bg) << "m"

#define VT_set_attributes( b, i, u, r ) VT_set_b( b ) << VT_set_i( i ) << VT_set_u( u ) << VT_set_r( r ) << VT_set_colors( fg_c, bg_c )

#define VT_set_bold(      b ) VT_default_attributes << VT_set_attributes( b, is_i, is_u, is_r )
#define VT_set_italic(    i ) VT_default_attributes << VT_set_attributes( is_b, i, is_u, is_r )
#define VT_set_underline( u ) VT_default_attributes << VT_set_attributes( is_b, is_i, u, is_r )
#define VT_set_reverse(   r ) VT_default_attributes << VT_set_attributes( is_b, is_i, is_u, r )

#define VT_finalize VT_default_attributes

#endif

// end vt100.h 
