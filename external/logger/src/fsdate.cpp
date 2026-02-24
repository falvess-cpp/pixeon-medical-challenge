/********************************************************************  
	@file       fsdate.cpp
	@author     Fabiano Souza
	@brief      Implementation of FsDate class.
	@history    1.0      Initial version 
	@date 		04-25-2022
***********************************************************************/
#include "fsdate.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <algorithm>


FsDate::FsDate() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::time_t t = system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&t);

    miYear  = local_tm.tm_year + 1900;
    miMonth = local_tm.tm_mon + 1;
    miDay   = local_tm.tm_mday;
    miHour  = local_tm.tm_hour;
    miMin   = local_tm.tm_min;
    miSec   = local_tm.tm_sec;
    miMill  = static_cast<int>(ms.count());
}

FsDate::FsDate(unsigned int year, 
               unsigned int month, 
               unsigned int day, 
               unsigned int hour, 
               unsigned int min, 
               unsigned int sec, 
               unsigned int mill)
    : miYear(year)
    , miMonth(month)
    , miDay(day)
    , miHour(hour)
    , miMin(min)
    , miSec(sec)
    , miMill(mill)
{
    if (!isValidDate(year, month, day))
        throw std::invalid_argument("Invalid date");
    if (hour > 23 || min > 59 || sec > 59 || mill > 999)
        throw std::invalid_argument("Invalid time component");
}

bool FsDate::operator<(const FsDate& other) const {
    return std::tie(miYear, miMonth, miDay, miHour, miMin, miSec) <
           std::tie(other.miYear, other.miMonth, other.miDay, other.miHour, other.miMin, other.miSec);
}

bool FsDate::operator>(const FsDate& other) const { return other < *this; }

bool FsDate::operator==(const FsDate& other) const {
    return miYear == other.miYear && miMonth == other.miMonth && miDay == other.miDay &&
           miHour == other.miHour && miMin == other.miMin && miSec == other.miSec;
}

std::string FsDate::today(DateFormat format) {
    FsDate now;
    return now.format(format);
}

std::string FsDate::getDate(DateFormat format) {
    return today(format); // Alias for backward compatibility
}

std::string FsDate::getDateWithMs() {
    FsDate now;
    return now.format(DateFormat::DateTimeMs);
}

bool FsDate::isLeapYear(unsigned int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool FsDate::isValidDate(unsigned int year, unsigned int month, unsigned int day) {
    if (month < 1 || month > 12) return false;
    unsigned int daysInMonth[] = {
        31,
        static_cast<unsigned int>(isLeapYear(year) ? 29 : 28),
        31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    return day >= 1 && day <= daysInMonth[month - 1];
}

FsDate FsDate::fromIso8601(const std::string& isoDate) {
    std::string cleanDate = isoDate;
    cleanDate.erase(std::remove(cleanDate.begin(), cleanDate.end(), '-'), cleanDate.end());
    cleanDate.erase(std::remove(cleanDate.begin(), cleanDate.end(), '/'), cleanDate.end());

    if (cleanDate.size() != 8)
        throw std::invalid_argument("Invalid ISO 8601 date format");

    unsigned int year = std::stoi(cleanDate.substr(0, 4));
    unsigned int month = std::stoi(cleanDate.substr(4, 2));
    unsigned int day = std::stoi(cleanDate.substr(6, 2));

    if (!isValidDate(year, month, day))
        throw std::invalid_argument("Invalid date components in ISO string");

    return FsDate(year, month, day, 0, 0, 0, 0);
}

std::string FsDate::format(DateFormat format) const {
    std::ostringstream oss;
    oss << std::setfill('0');
    switch (format) {
        case DateFormat::Date:
            oss << miYear << "/" << std::setw(2) << miMonth << "/" << std::setw(2) << miDay;
            break;
        case DateFormat::DateTime:
            oss << miYear << "/" << std::setw(2) << miMonth << "/" << std::setw(2) << miDay
                << " " << std::setw(2) << miHour << ":" << std::setw(2) << miMin << ":" << std::setw(2) << miSec;
            break;
        case DateFormat::DateTimeMs:
            oss << miYear << "/" << std::setw(2) << miMonth << "/" << std::setw(2) << miDay
                << " " << std::setw(2) << miHour << ":" << std::setw(2) << miMin << ":" << std::setw(2) << miSec
                << "." << std::setw(3) << miMill;
            break;
        case DateFormat::ISO8601Basic:
            oss << miYear << std::setw(2) << miMonth << std::setw(2) << miDay;
            break;
        case DateFormat::ISO8601Extended:
            oss << miYear << "-" << std::setw(2) << miMonth << "-" << std::setw(2) << miDay;
            break;
    }
    return oss.str();
}

FsDate FsDate::addDays(int days) const {
    auto tp = toTimePoint() + std::chrono::hours(days * 24);
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm local_tm = *std::localtime(&t);
    return FsDate(local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday,
                  local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec, miMill);
}

FsDate FsDate::addMonths(int months) const {
    int newMonth = miMonth + months;
    int newYear = miYear + (newMonth - 1) / 12;
    newMonth = ((newMonth - 1) % 12) + 1;
    int newDay = std::min(miDay, isValidDate(newYear, newMonth, miDay) ? miDay : 28);
    return FsDate(newYear, newMonth, newDay, miHour, miMin, miSec, miMill);
}

FsDate FsDate::addYears(int years) const {
    int newYear = miYear + years;
    int newDay = std::min(miDay, isValidDate(newYear, miMonth, miDay) ? miDay : 28);
    return FsDate(newYear, miMonth, newDay, miHour, miMin, miSec, miMill);
}

// Internal helper
std::chrono::system_clock::time_point FsDate::toTimePoint() const {
    std::tm tm{};
    tm.tm_year = miYear - 1900;
    tm.tm_mon = miMonth - 1;
    tm.tm_mday = miDay;
    tm.tm_hour = miHour;
    tm.tm_min = miMin;
    tm.tm_sec = miSec;
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

