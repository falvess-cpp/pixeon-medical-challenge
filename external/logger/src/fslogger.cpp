/********************************************************************  
	@file       fslogger.cpp
	@author     Fabiano Souza
	@brief      Implementation of FsLogger class.
	@history    1.0      Initial version 
	@date 		11-11-2025
***********************************************************************/
#include "fslogger.hpp"
#include "fsdate.hpp"

#include <iostream>
#include <algorithm>

bool FsLogger::isLogTrace = getConfigTrace();

bool FsLogger::isTraceEnabled()
{
	return isLogTrace;
}

bool FsLogger::getConfigTrace()
{
	const char* logTrace = std::getenv("LOG_TRACE");
	if (logTrace) {
		std::string strTrace(logTrace);
		std::transform(strTrace.begin(), strTrace.end(), strTrace.begin(), [](unsigned char c) {return std::toupper(c);});
		if (strTrace == "TRUE")
			return true;
	}
	
	return false;	
}

void FsLogger::funcEntry(const std::string& prettyFunction, const std::string& fileName, unsigned long lineNumber)
{
	if (isTraceEnabled()) {
		logFuncEntry(prettyFunction, fileName, lineNumber);
	}		
}

void FsLogger::logFuncEntry(const std::string& prettyFunction, const std::string& fileName, unsigned long lineNumber)
{
	std::cout << FsDate::getDateWithMs() << " "
		<< "---{ Enter " << prettyFunction << " "
		<< fileName << "#" << lineNumber << " "
		<< std::endl;
}

void FsLogger::funcExit(const std::string& function, const std::string& fileName, unsigned long lineNumber)
{
	if (isTraceEnabled()) {
		logFuncExit(function, fileName, lineNumber);
	}		
}

void FsLogger::logFuncExit(const std::string& function, const std::string& fileName, unsigned long lineNumber)
{
	std::cout << FsDate::getDateWithMs() << " "
		<< "---} Exit " << function << " "
		<< fileName << "#" << lineNumber << " "
		<< std::endl;
}

void FsLogger::log(LogType logType, const std::string& fileName, unsigned long lineNumber, const std::string& message)
{
	std::cout << FsLogger::getLogType(logType) << " " 
		<< fileName << "#" << lineNumber << " "
		<< FsDate::getDateWithMs() << " "
		<< message << std::endl;
}

void FsLogger::trace(const std::string& fileName, unsigned long lineNumber, const std::string& msg)
{
	if (isTraceEnabled()) {
		FsLogger::log(LogType::Trace, fileName, lineNumber, msg);
	}		
}

void FsLogger::info(const std::string& fileName, unsigned long lineNumber, const std::string& msg)
{
	FsLogger::log(LogType::Inform, fileName, lineNumber, msg);
}

void FsLogger::warning(const std::string& fileName, unsigned long lineNumber, const std::string& msg)
{
	FsLogger::log(LogType::Warn, fileName, lineNumber, msg);
}

void FsLogger::error(const std::string& fileName, unsigned long lineNumber, const std::string& msg)
{
	FsLogger::log(LogType::Error, fileName, lineNumber, msg);
}

std::string FsLogger::getLogType(LogType logType)
{
    switch (logType) 
	{
        case LogType::Error:  return "ERROR-----";
        case LogType::Warn:   return "WARN------";
        case LogType::Inform: return "INFORM----";
        case LogType::Trace:  return "TRACE-----";
        default:              return "UNKNOWN---";
    }
}
