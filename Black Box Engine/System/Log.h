#pragma once

#include <cassert>
#include <fstream>
#include <string>
#include <iostream>
#include <source_location>

#include "ConsoleManip.h"

#ifdef _DEBUG
/**
 * @brief Will log the message with additional information and formatting depending on log type.
 * @brief kMessage Appends nothing                      : "messageId"
 * @brief kWarning Colors text to yellow and Appends    : "'WARNING' FUNCTION(LINE:COLLUMN) | messageId"
 * @brief kError   Colors text to orange and Appends    : "'ERROR' FUNCTION(LINE:COLLUMN) |  messageId"
 * @brief kFailure Colors text to red and Appends       : "'FAIL' FUNCTION(LINE:COLLUMN) |  messageId"
 * THEN ABORTS PROGRAM
 */
#define BB_LOG(type, ...) BlackBoxEngine::ComplexLog(type, std::source_location::current() , __VA_ARGS__ )
#else
/** 
*  @brief This macro does nothing in release mode
*/
#pragma warning( disable : 4390)
#define BB_LOG(type, ...)
#endif

namespace BlackBoxEngine
{
#ifdef _DEBUG
    template<typename Tested_t>
	concept StreamOverloaded = requires (std::ostream os,const Tested_t& obj)
	{
		{ os << obj } -> std::convertible_to<std::ostream&>;
	};

    enum class LogType : uint8_t
    {
        kError,
        kWarning,
        kMessage,
        kFailure
    };

	class Logger
	{
	private:
		inline static std::fstream m_logFile;
		static constexpr const char* logFilePath = "..\\BlackBoxEngine_Build\\BlackBoxLog.txt";
	public:
		~Logger();
		static void AppendToLogFile(const char* str);
		template<StreamOverloaded ...Args>
		static void AppendToLogFile(const Args& ...args);
        static void OpenLog();
	};

    /**
	 * @brief simply prints all of the arguements passed in using std::clog. then appends contents to log file.
	 */
	template<StreamOverloaded ...Args>
		inline void SimpleLog(const Args & ...args)
	{
		(std::clog << ... << args) << '\n';
		Logger::AppendToLogFile(args...);
	}

    /**
     * @brief Will log the messageId with additional information and formatting depending on log type. 
     * @brief kMessage Appends nothing : "messageId"
     * @brief kWarning Colors console text to yellow and Appends : "'WARNING' FUNCTION(LINE:COLLUMN) | messageId"
     * @brief kError   Colors console text to orange and Appends : "'ERROR' FUNCTION(LINE:COLLUMN) |  messageId"
     * @brief kFailure Colors console text to red and Appends : "'FAIL' FUNCTION(LINE:COLLUMN) |  messageId"
     * THEN ABORTS PROGRAM
     */
    template<StreamOverloaded ...Args>
    void ComplexLog(const LogType type, const std::source_location location, const Args & ...args)
    {
        using enum LogType;
        
        switch (type)
        {
        case kMessage:
            SimpleLog(args...);
            break;
        case kError:
            ConsoleManip::ChangeConsoleFormatting(TEXT_ORANGE);
            SimpleLog( "ERROR >> ", location.function_name(), " (", location.line(), ",", location.column(), ") "
                        ,  " | ", args... );
            break;
        case kWarning:
            ConsoleManip::ChangeConsoleFormatting(TEXT_YEL);
            SimpleLog( "WARNING >> ", location.function_name(), " (", location.line(), ",", location.column(), ") "
                        , " | ", args...);;
            break;
        case kFailure:
            ConsoleManip::ChangeConsoleFormatting(TEXT_RED);
            SimpleLog("FAILURE >> ", location.function_name(), " (", location.line(), ",", location.column(), ") "
                , " | ", args...);;
            ConsoleManip::ChangeConsoleFormatting(TEXT_DEF);

            std::exit(1);//?
        };
        
        ConsoleManip::ChangeConsoleFormatting(TEXT_DEF);
    }
    
    template<StreamOverloaded ...Args>
	inline void Logger::AppendToLogFile(const Args&... args)
	{
        if (!m_logFile.is_open())
            OpenLog();
		(m_logFile << ... << args) << '\n';
	}
#endif
}


