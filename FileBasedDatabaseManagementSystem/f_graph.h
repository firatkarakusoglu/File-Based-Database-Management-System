//BASIC GRAPHICS FOR CONSOLE APPLICATION
//FIRAT KARAKUSOGLU   
//firatkarakusoglu@gmail.com
//DATE: 02.02.2007
//PUBLISHED: http://www.codeproject.com/Articles/17482/Basic-Graphics-For-Your-Console-Applications

#ifndef _F_GRAPH_H
#define _F_GRAPH_H

#include <windows.h>
#include <iostream>

class f_graph : public std::basic_ostream<char, std::char_traits<char> >
{
   
private:
        
   // copy constructor and operator= forbidden
   f_graph(const f_graph&) : std::basic_ostream<char, std::char_traits<char> >(std::cout.rdbuf()) {};
   f_graph & operator=(const f_graph&) {return *this;};

protected:
          
   // to handle console window
   HANDLE consoleHandler;
   
public:

   // f_graph class constructor
   f_graph(void);

   enum CursorMode
   {
      CURSOR_ON,
      CURSOR_OFF,
      CURSOR_BIG
   };

   // to set cursor mode
   int setCursor(CursorMode _cursorMode);

   // to get current screen x dimension
   int getDimensionX(void);

   // to get current screen y dimension
   int getDimensionY(void);

   // to set cursor to given position
   int gotoxy(const int _x, const int _y);

   // to get current cursor position on x dimension
   int whereX(void);

   // to get current cursor position on y dimension
   int whereY(void);

   // to set cursor to the top left corner
   void goBeginPixels(void) {gotoxy(1, 1);};

   // to set the cursor position to the right
   void goRight(void);
   
   // to set the cursor position to the left
   void goLeft(void);
   
   // to set the cursor position to the up
   void goUp(void);
   
   // to set the cursor position to the down
   void goDown(void);
      
   // to set color for output on console window
   void setColor(const WORD _newColorToSet);

   // to clear console window with special color
   void clrscr(const WORD _newColorToClr);

   // to clear console window with default colors
   void clrscr(void);

   // to set title of console window
   void setTitle(const char* _title);
   
   // waits as much as given value
   void wait(int _value);
   
   // to clear the area in given position
   void clearArea(int _x1,int _y1,int _x2,int _y2);
   
   // to make a box with given directions
   void box(int _x1,int _y1,int _x2,int _y2);
   
   // to make a box and restrict given information in that box
   void showTextInBox(char *_shownInfo,bool _newBeginKey,bool _controlKey ,int _x1,int _y1,int _x2,int _y2);
   
   // to show given items, as menu items on the left side
   void showLeftSide(char * _shownInfo,bool _newBeginKey = 0);
   
   // to show given information on the right box - (it means screen)
   void showRightSide(char *_shownInfo,bool _newBeginKey = 0,bool _controlKey = 1);

    // to make the base lines for our graph
    void makeSkeleton(const WORD _skeletonColor = FOREGROUND_GREEN);
   
   // to set the text on the headline
   void headerLine(char _headerText[20]);
   
   // to set the text at the bottom
   void footerLine(char *_footerText);
   
   // to set the text that out of the boxes, <bottom right corner>
   void rightBottomText(char _headerText[20],const WORD _textColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY);
   
   // to make a blinked text at the bottom, out of boxes
   void blink(char _blinkText[20],const WORD _textColor =  FOREGROUND_RED |  FOREGROUND_INTENSITY);
   
   // to blink red at the bottom
   void warning(char _warningText[20]);
   
   // to blink blue at the bottom
   void message(char _messageText[20]);

};

// to define the instance of our class
extern f_graph gf;

#endif

// eof f_graph.h
