#ifndef FS_LOGGING_HPP
#define FS_LOGGING_HPP

#include "fslogger.hpp"

// MSVC Compatibility for __PRETTY_FUNCTION__
#ifndef __PRETTY_FUNCTION__
    #ifdef _MSC_VER
        #define __PRETTY_FUNCTION__ __FUNCSIG__
    #else
        #define __PRETTY_FUNCTION__ __FUNCTION__
    #endif
#endif

#define LOG_TRACE(msg) FsLogger::trace(__FILE__, __LINE__, msg)
#define LOG_INFO(msg)  FsLogger::info(__FILE__, __LINE__, msg)
#define LOG_WARN(msg)  FsLogger::warning(__FILE__, __LINE__, msg)
#define LOG_ERROR(msg) FsLogger::error(__FILE__, __LINE__, msg)

#define LOG_FUNC_ENTRY() FsLogger::funcEntry(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#define LOG_FUNC_EXIT()  FsLogger::funcExit(__FUNCTION__, __FILE__, __LINE__)
	
#endif // FS_LOGGING_HPP
