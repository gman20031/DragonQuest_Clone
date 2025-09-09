#pragma once

#include <ostream>
#include <stdint.h>

namespace BlackBoxEngine
{
    /**
     * @brief Copied Directly from the SDL3 scancodes 
     * @brief https://wiki.libsdl.org/SDL3/SDL_Scancode
     */
    enum class KeyCode : uint8_t
    {
        kA = 4,
        kB = 5,
        kC = 6,
        kD = 7,
        kE = 8,
        kF = 9,
        kG = 10,
        kH = 11,
        kI = 12,
        kJ = 13,
        kK = 14,
        kL = 15,
        kM = 16,
        kN = 17,
        kO = 18,
        kP = 19,
        kQ = 20,
        kR = 21,
        kS = 22,
        kT = 23,
        kU = 24,
        kV = 25,
        kW = 26,
        kX = 27,
        kY = 28,
        kZ = 29,

        k1 = 30,
        k2 = 31,
        k3 = 32,
        k4 = 33,
        k5 = 34,
        k6 = 35,
        k7 = 36,
        k8 = 37,
        k9 = 38,
        k0 = 39,

        kReturn = 40,
        kEscape = 41,
        kBackspace= 42,
        kTab = 43,
        kSpace = 44,

        kMinus = 45,
        kEquals = 46,
        kLeftBracket= 47,
        kRightBracket = 48,

        kBackslash = 49, /** Located at the lower left of the return */
        kNonusSlash = 50, /** ISO USB keyboards actually use this code */

        kSemicolon = 51,
        kApostraphe = 52,
        kGrave = 53, /** Located in the top left corner */
        kComma = 54,
        kPeriod = 55,
        kSlash = 56,

        kCapslock = 57,

        kF1 = 58,
        kF2 = 59,
        kF3 = 60,
        kF4 = 61,
        kF5 = 62,
        kF6 = 63,
        kF7 = 64,
        kF8 = 65,
        kF9 = 66,
        kF10 = 67,
        kF11 = 68,
        kF12 = 69,

        kPrintScreen = 70,
        kScrollLock= 71,
        kPause = 72,
        kInsert = 73, /** insert on PC, help on some Mac keyboards (butdoes send code 73, not 117) */
        kHome= 74,
        kPageUp = 75,
        kDelete = 76,
        kEnd = 77,
        kPageDown = 78,
        kRight = 79,
        kLeft = 80,
        kDown = 81,
        kUp = 82,

        kNumlockClear = 83, /** num lock on PC, clear on Mac keyboards */

        kKeyPad_DIVIDE = 84,
        kKeyPad_MULTIPLY = 85,
        kKeyPad_MINUS = 86,
        kKeyPad_PLUS = 87,
        kKeyPad_ENTER = 88,
        kKeyPad_1 = 89,
        kKeyPad_2 = 90,
        kKeyPad_3 = 91,
        kKeyPad_4 = 92,
        kKeyPad_5 = 93,
        kKeyPad_6 = 94,
        kKeyPad_7 = 95,
        kKeyPad_8 = 96,
        kKeyPad_9 = 97,
        kKeyPad_0 = 98,
        kKeyPad_PERIOD = 99,

        kNonusBackslash = 100, /** This is the additional key that ISO
                                            *   keyboards have over ANSI ones,
                                            *   located between left shift and Y.
                                            *   Produces GRAVE ACCENT and TILDE in a
                                            *   US or UK Mac layout, REVERSE SOLIDUS
                                            *   (backslash) and VERTICAL LINE in a
                                            *   US or UK Windows layout, and
                                            *   LESS-THAN SIGN and GREATER-THAN SIGN
                                            *   in a Swiss German, German, or French
                                            *   layout. */
        kApplication = 101, /** windows contextual menu, compose */
        kPower = 102, /** The USB document says this is a status flag,
                                   *   not a physical key - but some Mac keyboards
                                   *   do have a power key. */
        kKpEquals = 103,
        kF13 = 104,
        kF14 = 105,
        kF15 = 106,
        kF16 = 107,
        kF17 = 108,
        kF18 = 109,
        kF19 = 110,
        kF20 = 111,
        kF21 = 112,
        kF22 = 113,
        kF23 = 114,
        kF24 = 115,
        kExecute = 116,
        kHelp = 117,    /** AL Integrated Help Center */
        kMenu = 118,    /** Menu (show menu) */
        kSelect = 119,
        kStop = 120,    /** AC Stop */
        kAgain = 121,   /** AC Redo/Repeat */
        kUndo = 122,    /** AC Undo */
        kCut = 123,     /** AC Cut */
        kCopy = 124,    /** AC Copy */
        kPaste = 125,   /** AC Paste */
        kFind = 126,    /** AC Find */
        kMute = 127,
        kVolumeUP = 128,
        kVolumeDown = 129,

        KP_COMMA = 133,
        KP_EQUALSAS400 = 134,

        kCount /** not a key, just marks the number of scancodes for array bounds */

    };

    std::ostream& operator<<(std::ostream& os, KeyCode out);
};