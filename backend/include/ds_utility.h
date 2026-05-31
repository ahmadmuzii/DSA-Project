#ifndef DS_UTILITY_H
#define DS_UTILITY_H

#include <string>
#include <vector>

std::string convertToComparable(const std::string& date);
int timeToMinutes(const std::string& time);
std::string escapeCSV(const std::string& field);
std::vector<std::string> parseCSVLine(const std::string& line);

#endif
