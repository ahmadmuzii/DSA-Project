#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>

class CalendarSystem;

bool saveEventsToFile(const std::string& filename, CalendarSystem& calendar);
int loadEventsFromFile(const std::string& filename, CalendarSystem& calendar);

#endif
