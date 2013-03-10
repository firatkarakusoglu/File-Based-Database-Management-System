//BASIC GRAPHICS FOR CONSOLE APPLICATION
//FIRAT KARAKUSOGLU   
//firatkarakusoglu@gmail.com
//DATE: 02.02.2007
//PUBLISHED: http://www.codeproject.com/Articles/17482/Basic-Graphics-For-Your-Console-Applications

#include "stdafx.h"

#include <windows.h>
#include <iostream>
#include <conio.h>

#include "f_graph.h" 

// an instance of our class - to access member functions
f_graph gf;

// to save where our cursor on the screen
int countLine=7;

// f_graph class constructor
f_graph::f_graph(void): std::basic_ostream<char, std::char_traits<char> >(std::cout.rdbuf())
{
   consoleHandler = GetStdHandle(STD_OUTPUT_HANDLE);
}

// to set cursor mode
int f_graph::setCursor(CursorMode _cursorMode)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return 0;

   CONSOLE_CURSOR_INFO cursorInfo;

   switch (_cursorMode)
   {
   case CURSOR_OFF:
      cursorInfo.bVisible = FALSE;
      cursorInfo.dwSize = 10;
      break;
   case CURSOR_ON:
      cursorInfo.bVisible = TRUE;
      cursorInfo.dwSize = 10; 
      break;
   case CURSOR_BIG:
      cursorInfo.bVisible = TRUE;
      cursorInfo.dwSize = 100; 
      break;
   }

   SetConsoleCursorInfo(consoleHandler, &cursorInfo);
}

// to get current screen x dimension
int f_graph::getDimensionX(void)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return 0;

   CONSOLE_SCREEN_BUFFER_INFO screenInfo;
   GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);

   return screenInfo.dwMaximumWindowSize.X;

}

// to get current screen y dimension
int f_graph::getDimensionY(void)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return 0;

   CONSOLE_SCREEN_BUFFER_INFO screenInfo;
   GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);

   return screenInfo.dwMaximumWindowSize.Y;
}

// to set cursor to given position
// (1, 1) means upper left corner
int f_graph::gotoxy(const int _x, const int _y)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return 0;

   COORD coords = {static_cast<short>(_x - 1), static_cast<short>(_y - 1)};
   SetConsoleCursorPosition(consoleHandler, coords);
}

// to get current cursor position on x dimension
int f_graph::whereX(void)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return 0;

   CONSOLE_SCREEN_BUFFER_INFO screenInfo;
   GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);

   return screenInfo.dwCursorPosition.X + 1;
}

// to get current cursor position on y dimension
int f_graph::whereY(void)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return 0;

   CONSOLE_SCREEN_BUFFER_INFO screenInfo;
   GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);

   return screenInfo.dwCursorPosition.Y + 1;
}

// to set the cursor position to the left pixel
void f_graph::goRight(void)
{
     int x;
     int y;
     x = whereX();
     y = whereY();
     gotoxy(x+1,y);
     };

// to set the cursor position to the left pixel
void f_graph::goLeft(void)
{
     int x;
     int y;
     x = whereX();
     y = whereY();
     gotoxy(x-1,y);
     }; 

// to set the cursor position to the left pixel
void f_graph::goUp(void)
{
     int x;
     int y;
     x = whereX();
     y = whereY();
     gotoxy(x,y-1);
     };
   
// to set the cursor position to the left pixel
void f_graph::goDown(void)
{
     int x;
     int y;
     x = whereX();
     y = whereY();
     gotoxy(x,y+1);
     };
     
// to set color for output on console window
void f_graph::setColor(const WORD _newColorToSet)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return;

   SetConsoleTextAttribute(consoleHandler, _newColorToSet);
}

// to clear console window with special color
void f_graph::clrscr(const WORD _newColorToClr)
{
   if (consoleHandler == INVALID_HANDLE_VALUE)
      return;

   static const COORD topLeftCorner = {0, 0};

   CONSOLE_SCREEN_BUFFER_INFO screenInfo;
   GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);

   SMALL_RECT screenSize;
   screenSize.Left = 0;
   screenSize.Top = 0;
   screenSize.Right = screenInfo.dwSize.X - 1;
   screenSize.Bottom = screenInfo.dwSize.Y - 1;

   _CHAR_INFO* buffer = new _CHAR_INFO[screenInfo.dwSize.X * screenInfo.dwSize.Y];
   _CHAR_INFO charInfo = {' ', _newColorToClr};
   for (int i = 0; i < screenInfo.dwSize.X * screenInfo.dwSize.Y; i++)
   {
      buffer[i] = charInfo;
   }

   COORD bufferSize = {screenInfo.dwSize.X, screenInfo.dwSize.Y};

   WriteConsoleOutput(consoleHandler,
                      buffer,
                      bufferSize,
                      topLeftCorner,
                      &screenSize);

   delete[] buffer;
   goBeginPixels();
   setColor(screenInfo.wAttributes);
}

// to clear console window (white text on black background)
void f_graph::clrscr(void)
{
   clrscr(FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN );
}

// to set title of console window
//void f_graph::setTitle(const char* _title)
//{
//   SetConsoleTitle(_title);
//}

// waits as much as given value
void f_graph::wait(int _value)
{
     Sleep(_value*1000);
     };

// to clear the area in given directions
void f_graph::clearArea(int _x1,int _y1,int _x2,int _y2)
{
	int i,j;
	int x = whereX();
	int y = whereY();
	gotoxy(_x1,_y1);
	for(j = _y1; j<=_y2 ; j++)
	{
        for(i = _x1;i<=_x2;i++)
    	{
              gotoxy(i,j);
              std::cout<<(" ");
              }
         };
     gotoxy(x,y);
}

// to make a box with given directions
void f_graph::box(int _x1,int _y1,int _x2,int _y2)
{
    char topLeft = 201;
    char horizontalLines = 205;
    char topRight = 187;
    char verticalLines = 186;
    char bottomLeft = 200;
    char bottomRight = 188;
	int i;
	int xBegin = whereX();
	int yBegin = whereY();
	gotoxy(_x1,_y1); std::cout<<topLeft;     //left top corner
	for(i = _x1+1;i<_x2;i++) std::cout<<horizontalLines;     //top line
	gotoxy(_x2,_y1); std::cout<<topRight;     //right top corner
	for(i= _y1+1;i<_y2;i++)        //vertical lines
	{
		gotoxy(_x1,i);std::cout<<verticalLines;
		gotoxy(_x2,i);std::cout<<verticalLines;

	}
	gotoxy(_x1,_y2); std::cout<<bottomLeft;     //left bottom corner
	for(i=_x1+1;i<_x2;i++) std::cout<<horizontalLines;       //bottom line
	gotoxy(_x2,_y2); std::cout<<bottomRight;     //right bottom corner
	gotoxy(xBegin,yBegin);
}

// to show given information on the  screen
void f_graph::showTextInBox(char *_shownInfo,bool _newBeginKey,bool _controlKey, int _x1, int _y1, int _x2, int _y2)
{
     setCursor(CURSOR_OFF);
     box(_x1,_y1,_x2,_y2);
     _x1++;
     _x2--;
     _y2--;
	 int xBegin = whereX();
	 int yBegin = whereY();
     if(_newBeginKey==1) { clearArea(_x1+1,_y1+1,_x2,_y2); countLine=_y1+1; };
     countLine++;
     gotoxy(_x1+1,countLine);
     
     int placeHolder = 0;
     int letterCounterBegin = 0;
     int letterCounterEnd = 0;
     int xPixel;
     int yPixel;
     bool equalityKey = false;
     bool lastLine = false;
     char letter;
     while( letterCounterBegin < (strlen(_shownInfo)) )
     {
              letter = _shownInfo[letterCounterBegin];
              if( letter != ' ' )
              {
                  placeHolder = letterCounterBegin;
                  while(letter != ' ' && letterCounterBegin < (strlen(_shownInfo)))
                  {
                               letterCounterBegin++;
                               letter = _shownInfo[letterCounterBegin];
                               };
                  };
              xPixel = whereX();
              yPixel = whereY();


             if(countLine == _y2-1 && _controlKey == 0 && ((letterCounterBegin - placeHolder) >= (_x2 - xPixel))) 
             {
                      countLine = _y1+1;
                      clearArea(_x1+1,_y1+1,_x2-1,_y2-1);
                      gotoxy(_x1+1,countLine);
                      }
             if(countLine == _y2 && _controlKey == 1) // && ((letterCounterBegin - placeHolder) >= (_x2 - xPixel))
             {
                      countLine = _y1+1;
                      setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
                      rightBottomText("Press any key to go to the next page");
                      getch();
                      clearArea(_x1+1,_y1+1,_x2,_y2);
                      setColor(FOREGROUND_GREEN);
                      clearArea(1,24,80,24);
                      gotoxy(_x1+2,countLine+1);
                      }
              if((letterCounterBegin - placeHolder) < (_x2 - xPixel))
              {
                                     for(;placeHolder<letterCounterBegin;placeHolder++) 
                                     putchar(_shownInfo[placeHolder]);
                                     }
              else if((letterCounterBegin - placeHolder) < _x2-_x1)
              {
                   countLine++;
                   gotoxy(_x1+1,countLine);                   
                   for(;placeHolder<letterCounterBegin;placeHolder++)
                   putchar(_shownInfo[placeHolder]);
                   }
              else 
              {
                   for(;placeHolder<letterCounterBegin;placeHolder++)
                   {
                    int xPixel2 = whereX();
                    if(xPixel2 == _x2) {countLine++; gotoxy(_x1+1,countLine); };
                    putchar(_shownInfo[placeHolder]);                                                 
                                                                     };
                                      
                   };
              letterCounterBegin++;
              if(whereX() != _x2-1)std::cout<<(" ");
              }
     };

// to show given items as menu items on the left side
void f_graph::showLeftSide(char * _shownInfo,bool _newBeginKey)
{
     CONSOLE_SCREEN_BUFFER_INFO screenInfo;
     GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);
     setColor(BACKGROUND_GREEN);
     showTextInBox(_shownInfo,_newBeginKey,1,7,6,27,19);
     setColor(screenInfo.wAttributes);
     }

// to show given information on the right box - (it means screen)
void f_graph::showRightSide(char *_shownInfo,bool _newBeginKey,bool _controlKey)
{
     showTextInBox(_shownInfo,_newBeginKey,1,29,6,73,19);
     };

// to make the mother lines of our graph
void f_graph::makeSkeleton(const WORD _skeletonColor)
{
     setColor(_skeletonColor);
     box(5,2,75,4); //top box
     box(5,5,75,20); //center - out box      
     box(7,6,27,19); //left - menu box     
     box(29,6,73,19); //right - screen box
     box(5,21,75,23); //bottom box
     
     gotoxy(6,22);
     };
     
// to set the text on the headline
void f_graph::headerLine(char _headString[20])
{
    setCursor(CURSOR_OFF);
    int xBegin = whereX();
    int yBegin = whereY();
	int strLength;
	clearArea(6,3,74,3);
	box(5,2,75,4);
	strLength = strlen(_headString);
	if(strLength<=67)
	{
    	strLength /= 2;
    	gotoxy((40 - strLength),3);
    	std::cout<<_headString;
        }
     else
     {
         char *lengthWarning;
         lengthWarning = "Your string's length can not be larger than 67 character.";
         strLength = strlen(lengthWarning);
    	 strLength /= 2;
         gotoxy((40 - strLength),3);
    	 std::cout<<lengthWarning;         
    	 getch();
    	 exit(1);
         };
	gotoxy(xBegin,yBegin);
    };
                
// to set the text at the bottom
void f_graph::footerLine(char *_footString)
{
    int strLength;
	clearArea(6,22,74,22);
	box(5,21,75,23);
	int dimX;
	dimX = getDimensionX();
	strLength = strlen(_footString);
	if(strLength<=67)
	{
    	strLength /= 2;
    	gotoxy(8,22);
    	std::cout<<_footString;
        }
     else
     {
         char *lengthWarning;
         lengthWarning = "Your string's length can not be larger than 67 character.";
         strLength = strlen(lengthWarning);
    	 strLength /= 2;
         gotoxy((40 - strLength),22);
    	 std::cout<<lengthWarning;         
    	 getch();
    	 exit(1);
         };
    setCursor(CURSOR_ON);
    };
// to set the text that out of the boxes
void f_graph::rightBottomText(char _headerText[20],const WORD _textColor)
{
    setCursor(CURSOR_OFF);
    int xBegin = whereX();
    int yBegin = whereY();
    setColor(_textColor);
	char *ptrLetter;
	int ctr=0;
	int xAxes = 76;
	ptrLetter = _headerText;
	while(*ptrLetter != '\0') {
	ptrLetter++;
	ctr++;
	gotoxy(xBegin,yBegin);
	}

	for(;ctr != -1;ctr--)
	{
		gotoxy(xAxes,24);
		std::cout<<*ptrLetter-- ;
		xAxes--;
	}
	gotoxy(75,24);
	setColor(_textColor);
	gotoxy(xBegin,yBegin);
    }
                
// to make a blinked text at the bottom, out of boxes
void f_graph::blink(char _blinkText[50],const WORD _textColor)
{
     CONSOLE_SCREEN_BUFFER_INFO screenInfo;
     GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);
     setCursor(CURSOR_OFF);
     int xBegin = whereX();
     int yBegin = whereY();
     int i=0;
     Sleep(500);
     for(;i<1;i++)
     {
         setColor(_textColor);
         rightBottomText(_blinkText,_textColor);
         Sleep(500);
         clearArea(1,24,75,24);
         Sleep(700);
         setColor(_textColor);
         rightBottomText(_blinkText,_textColor);
         };
         Sleep(2000);
         clearArea(1,24,75,24);
         gotoxy(xBegin,yBegin);
     setColor(screenInfo.wAttributes);
     }

// to blink red at the bottom
void f_graph::warning(char _warningText[20])
{
     blink(_warningText,FOREGROUND_RED | FOREGROUND_INTENSITY);
     };

// to blink red at the bottom
void f_graph::message(char _messageText[20])
{
     blink(_messageText,FOREGROUND_BLUE | FOREGROUND_INTENSITY);
     };

// eof f_graph.cpp
