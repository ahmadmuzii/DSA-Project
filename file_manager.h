#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include "calendar.h"

// Function prototypes
void saveEventsToFile(std::string filename, BST* bst);
void loadEventsFromFile(std::string filename, class CalendarSystem& sys);

#endif