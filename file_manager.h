#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include "calendar.h" // Forward declaration if needed

// Function prototypes
void saveEventsToFile(string filename, BST* bst);
void loadEventsFromFile(string filename, class CalendarSystem& sys);

#endif