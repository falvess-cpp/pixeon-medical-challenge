#ifndef FSDATE_HPP
#define FSDATE_HPP

#include <string>
#include <chrono>

enum class DateFormat {
    Date,        	// YYYY/MM/DD
    DateTime,    	// YYYY/MM/DD HH:MM:SS
    DateTimeMs,  	// YYYY/MM/DD HH:MM:SS.mmm
    ISO8601Basic,	// YYYYMMDD
    ISO8601Extended // YYYY-MM-DD
};
	
class FsDate
{
private:
	int miYear;	// years [0,9999]
	int miMonth; // months [1,12]
	int miDay;   // day [1,31]
	int miHour;  // hours [0,23]
	int miMin;   // minutes [0,59]
	int miSec;   // seconds [0,59]
	int miMill;  // milliseconds [0,999]

	// Internal helpers
    std::chrono::system_clock::time_point toTimePoint() const;

public:
	FsDate(); // Current date/time
	FsDate(unsigned int year, 
		   unsigned int month, 
		   unsigned int day, 
		   unsigned int hour, 
		   unsigned int min, 
		   unsigned int sec, 
		   unsigned int mill);	
	
    FsDate(const FsDate& obj) = default;
    FsDate& operator=(const FsDate& obj) = default;	   
	
    // Static methods for formatted date/time strings
    static std::string today(DateFormat format);
    static std::string getDate(DateFormat format);
    static std::string getDateWithMs();
	
    // Static factory from ISO string
    static FsDate fromIso8601(const std::string& isoDate);
	
    std::string format(DateFormat format) const;
	
    FsDate addDays(int days) const;
    FsDate addMonths(int months) const;
    FsDate addYears(int years) const;
	
    bool operator<(const FsDate& other) const;
    bool operator>(const FsDate& other) const;
    bool operator==(const FsDate& other) const;
	
    static bool isLeapYear(unsigned int year);
    static bool isValidDate(unsigned int year, unsigned int month, unsigned int day);
};

#endif // FSDATE_HPP
