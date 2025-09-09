#pragma once
#include <iostream>
#include <concepts>

//// https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences  ////

// Make constexpr instead of macros??

#define TO_STRING(str) #str

#define VT_ESC "\x1b["
#define VT_CSI "\x1b]"

#define CSR_BLINKING_ON "?12h" 	 // Text Cursor Enable Blinking 
#define CSR_BLINKING_OFF "?12l"	 // Text Cursor Disable Blinking 
#define CSR_SHOW_ON "?25h"		 // Text Cursor Enable Mode Show 
#define CSR_SHOW_OFF "?25l"		 // Text Cursor Enable Mode Hide 

#define CSR_SHAPE(n) #n " q"
#define CSR_SHAPE_DEF		  CSR_SHAPE(0) 	 //User Shape		
#define CSR_SHAPE_BLINK_BLOCK CSR_SHAPE(1)	 //Blinking Block	
#define CSR_SHAPE_STEAD_BLOCK CSR_SHAPE(2)	 //Steady Block 	
#define CSR_SHAPE_BLINK_UNDER CSR_SHAPE(3)	 //Blinking Underlin
#define CSR_SHAPE_STEAD_UNDER CSR_SHAPE(4)	 //Steady Underline 
#define CSR_SHAPE_BLINK_BAR   CSR_SHAPE(5)	 //Blinking Bar 	
#define CSR_SHAPE_STEAD_BAR   CSR_SHAPE(6)	 //Steady Bar

#define CSR_MOVE_UP(n)		#n "A" 	//Cursor up by <n>
#define CSR_MOVE_DOWN(n)	#n "B" 	//Cursor down by <n>
#define CSR_MOVE_FORWARD(n)	#n "C" 	//Cursor forward (Right) by <n>
#define CSR_MOVE_BACK(n)	#n "D" 	//Cursor backward (Left) by <n>
#define CSR_MOVE_LINEDOWN(n)#n "E" 	//Cursor down <n> lines from current position
#define CSR_MOVE_LINEUP(n)	#n "F" 	//Cursor up <n> lines from current position
#define CSR_MOVE_COLLUMN(n)	#n "G" 	//Cursor moves to <n>th position horizontally in the current line
#define CSR_MOVE_LINE(n)	#n "d" 	//Cursor moves to the <n>th position vertically in the current column
#define CSR_MOVE_POSITION(y,x)  TO_STRING(y##;##x##H) //"%i;%iH" //Cursor moves to <x>; <y> coordinate within the viewport, where <x> is the column of the <y> line

#define VIEW_SCROLL_UP(n)	#n "SU" //Scroll text up by <n>. Also known as pan down, new lines fill in from the bottom of the screen
#define VIEW_SCROLL_DOWN(n) #n "SD" //Scroll down by <n>. Also known as pan up, new lines fill in from the top of the screen

#define TEXT_INSERT(n)		 #n "@"  //Insert <n> spaces at the current cursor position, shifting all existing text to the right. Text exiting the screen to the right is removed.
#define TEXT_DELETE(n)		 #n "P"  //Delete <n> characters at the current cursor position, shifting in space characters from the right edge of the screen.
#define TEXT_ERASE(n)		 #n "X"  //Erase <n> characters from the current cursor position by overwriting them with a space character.
#define TEXT_INESERT_LINE(n) #n "L"  //Inserts <n> lines into the buffer at the cursor position. The line the cursor is on, and lines below it, will be shifted downwards.
#define TEXT_DELETE_LINE(n)  #n "M"  //Deletes <n> lines from the buffer, starting with the row the cursor is on.
#define TEXT_ERASE_DISPLAY(n)#n "J"	 //Replace all text in the current viewport/screen specified by <n> with space characters
#define TEXT_ERASE_LINE(n)	 #n "K"	 //Replace all text on the line with the cursor specified by <n> with space characters
#define TEXT_ERASE_DISPLAY_UNTIL  TEXT_ERASE_DISPLAY(0)  //0 erases from the current cursor position (inclusive) to the end of the line/display
#define TEXT_ERASE_DISPLAY_FROM	  TEXT_ERASE_DISPLAY(1)	 //1 erases from the beginning of the line/display up to and including the current cursor position
#define TEXT_ERASE_DISPLAY_ALL    TEXT_ERASE_DISPLAY(2)	 //2 erases the entire line/display
#define TEXT_ERASE_LINE_UNTIL	  TEXT_ERASE_LINE(0)
#define TEXT_ERASE_LINE_FROM	  TEXT_ERASE_LINE(1)
#define TEXT_ERASE_LINE_ALL		  TEXT_ERASE_LINE(2)

#define TEXT_FORMAT(n) TO_STRING(n##m) //Set the format of the screen and text as specified by <n>
#define TEXT_RGB(r,g,b) TEXT_FORMAT(38;2;r;g;b)
#define TEXT_RED   TEXT_RGB(255,0,0)
#define TEXT_GREEN TEXT_RGB(0,255,0)
#define TEXT_BLUE  TEXT_RGB(0,0,255)
#define TEXT_YEL   TEXT_RGB(255,255,0)
#define TEXT_ORANGE TEXT_RGB(255,165,0)
#define TEXT_DEF "0m"
#define TEXT_FOR_DEF "39"
#define BACK_RGB(r,g,b) TEXT_FORMAT(48;2;r;g;b)

/*
0 		Default 					Returns all attributes to the default state prior to modification
27 		Positive (No negative)	 	Returns foreground/background to normal
30 		Foreground Black 			Applies non-bold/bright black to foreground
31 		Foreground Red 				Applies non-bold/bright red to foreground
32 		Foreground Green 			Applies non-bold/bright green to foreground
33 		Foreground Yellow 			Applies non-bold/bright yellow to foreground
34 		Foreground Blue 			Applies non-bold/bright blue to foreground
35 		Foreground Magenta 			Applies non-bold/bright magenta to foreground
36 		Foreground Cyan 			Applies non-bold/bright cyan to foreground
37 		Foreground White 			Applies non-bold/bright white to foreground
38 		Foreground Extended 		Applies extended color value to the foreground
39 		Foreground Default 			Applies only the foreground portion of the defaults (see 0)
48 		Background Extended 		Applies extended color value to the background (same as RGB)
49 		Background Default 			Applies only the background portion of the defaults (see 0)
*/

template<typename Type>
concept streamOverloaded = requires(Type var)
{
	std::cout << var;
};

class ConsoleManip
{
	inline static const char* m_pDefaultFormat = TEXT_DEF;

public:
	// dont do this
	static bool EnableVTMode();

	// changes to console
	static void ChangeConsoleTitle(const char* newTitle);
	static void ChangeConsoleFormatting(const char* newFormatting); //sets default color for printing
	static void ResetConsoleFormatting();

	// simple functionality
	static void ClearConsole();
	static void SetCursorPosition(int x, int y);
	static void Pause();

	// printing
	static void Printf(const char* output, const char* formatting = m_pDefaultFormat);
	static void Printf(const char output, const char* formatting = m_pDefaultFormat);

	template<streamOverloaded Type>
	static void Printf(Type output, const char* formatting = m_pDefaultFormat , std::ostream& os = std::cout);

	template<streamOverloaded Type>
	static void Printf(Type output, std::initializer_list<const char*> formatList, std::ostream& os = std::cout);
};

template<streamOverloaded Type>
inline void ConsoleManip::Printf(Type output, const char* formatting, std::ostream& os)
{
	os << VT_ESC << formatting << output << VT_ESC << m_pDefaultFormat;
}

template<streamOverloaded Type>
inline void ConsoleManip::Printf(Type output, std::initializer_list<const char*> formatList, std::ostream& os)
{
	for (auto& format : formatList)
		os << VT_ESC << format;
	Printf(output , NULL , os);
}
