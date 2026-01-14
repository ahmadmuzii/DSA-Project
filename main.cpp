#include <iostream>
#include <iomanip>
#include <sys/stat.h>  // For file existence check
#include <ctime>       // Added for time()
#include "calendar.h"
#include "file_manager.h"

using namespace std;

#ifdef _WIN32
    #include <windows.h>
    #define STAT _stat
#else
    #include <unistd.h>
    #define STAT stat
#endif

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(string title) {
    clearScreen();

    cout << "--------------------------------------------------------------\n";
    cout << "                " << title << "\n";
    cout << "--------------------------------------------------------------\n\n";
}

void showMenu() {
    clearScreen();
    cout << "--------------------------------------------------------------\n";
    cout << "                 CALENDAR SYSTEM (DSA)                    \n";
    cout << "--------------------------------------------------------------\n";
    cout << "  1. Add New Event                                        \n";
    cout << "  2. View Events by Date                                  \n";
    cout << "  3. Search Events                                        \n";
    cout << "  4. Check Time Conflicts                                 \n";
    cout << "  5. View All Events                                 \n";
    cout << "  6. Update Event                                         \n";
    cout << "  7. Delete Event                                         \n";
    cout << "  8. Save/Load Data                                       \n";
    cout << "  9. Display Data Structures                              \n";
    cout << "  0. Exit                                                 \n";
    cout << "--------------------------------------------------------------\n";
    cout << "  Choose option: ";
}

// Function to validate DD:MM:YYYY format
bool validateDateFormat(string date) {
    // Check length first
    if(date.length() != 10) return false;
    
    // Check separators
    if(date[2] != ':' || date[5] != ':') return false;
    
    // Extract parts
    string dayStr = date.substr(0, 2);
    string monthStr = date.substr(3, 2);
    string yearStr = date.substr(6, 4);
    
    // Check if all are digits
    for(char c : dayStr) if(!isdigit(c)) return false;
    for(char c : monthStr) if(!isdigit(c)) return false;
    for(char c : yearStr) if(!isdigit(c)) return false;
    
    // Convert to integers
    int day = stoi(dayStr);
    int month = stoi(monthStr);
    int year = stoi(yearStr);
    
    // Basic validation
    if(year < 1900 || year > 2100) return false;
    if(month < 1 || month > 12) return false;
    
    // Check days in month
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Adjust for leap year
    if(month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if(isLeap) daysInMonth[1] = 29;
    }
    
    if(day < 1 || day > daysInMonth[month-1]) return false;
    
    return true;
}

// Function to validate HH:MM format
bool validateTimeFormat(string time) {
    // Check length
    if(time.length() != 5) return false;
    
    // Check separator
    if(time[2] != ':') return false;
    
    // Extract parts
    string hourStr = time.substr(0, 2);
    string minuteStr = time.substr(3, 2);
    
    // Check if all are digits
    for(char c : hourStr) if(!isdigit(c)) return false;
    for(char c : minuteStr) if(!isdigit(c)) return false;
    
    // Convert to integers
    int hour = stoi(hourStr);
    int minute = stoi(minuteStr);
    
    // Validate ranges
    if(hour < 0 || hour > 23) return false;
    if(minute < 0 || minute > 59) return false;
    
    return true;
}

// Helper function to show hash distribution (for debugging)
void showHashDistribution(const CalendarSystem& calendar) {
    cout << "\n[DEBUG] Hash Table Bucket Distribution:\n";
    cout << "----------------------------------------\n";
    
    // This would require making getTable() public or adding a display method
    // For now, we'll rely on the displayStructures() method
    cout << "Use option 9 (Display Data Structures) to see hash distribution.\n";
}

int main() {
    CalendarSystem calendar;
    int choice;
    
    // Auto-load on startup (ONLY if file exists and is not empty)
    struct STAT buffer;
    if (STAT("calendar_data.txt", &buffer) == 0) {
        if (buffer.st_size > 0) {
            cout << "\n[INFO] Loading saved data from calendar_data.txt...\n";
            loadEventsFromFile("calendar_data.txt", calendar);
            cout << "[INFO] Data loaded successfully!\n";
        } else {
            cout << "\n[INFO] calendar_data.txt exists but is empty. Starting with empty calendar.\n";
        }
    } else {
        cout << "\n[INFO] No saved data found. Starting with empty calendar.\n";
    }
    
    // Pause briefly to show the info message
    #ifdef _WIN32
        Sleep(1500);
    #else
        usleep(1500000);
    #endif

    do {
        showMenu();
        cin >> choice;
        cin.ignore(); // Clear newline buffer

        switch(choice) {
            case 1: {
                printHeader("ADD NEW EVENT");
                string title, date, start, desc;
                int dur, prio;
                bool validDate = false;
                
                cout << "Event Title : "; getline(cin, title);
                
                // Date input with validation
                while(!validDate) {
                    cout << "Date (DD:MM:YYYY) : "; getline(cin, date);
                    validDate = validateDateFormat(date);
                    if(!validDate) {
                        cout << "Invalid date format! Please use DD:MM:YYYY format.\n";
                    }
                }
                
                // Time input with validation
                bool validTime = false;
                while(!validTime) {
                    cout << "Start Time (HH:MM) : "; getline(cin, start);
                    validTime = validateTimeFormat(start);
                    if(!validTime) {
                        cout << "Invalid time format! Please use HH:MM format (24-hour).\n";
                    }
                }
                
                cout << "Duration (minutes) : "; cin >> dur;
                
                // Priority validation
                bool validPriority = false;
                while(!validPriority) {
                    cout << "Priority (1-5) : "; cin >> prio;
                    if(prio >= 1 && prio <= 5) {
                        validPriority = true;
                    } else {
                        cout << "Priority must be between 1 and 5!\n";
                        cout << "Priority (1-5) : ";
                    }
                }
                
                cin.ignore();
                cout << "Description (optional) : "; getline(cin, desc);
                
                calendar.addEvent(title, date, start, dur, prio, desc);
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 2: {
                printHeader("VIEW EVENTS BY DATE");
                string date;
                bool validDate = false;
                
                while(!validDate) {
                    cout << "Enter date (DD:MM:YYYY): "; getline(cin, date);
                    validDate = validateDateFormat(date);
                    if(!validDate) {
                        cout << "Invalid date format! Please use DD:MM:YYYY format.\n";
                    }
                }
                
                calendar.viewEventsByDate(date);
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 3: {
                printHeader("SEARCH EVENTS");
                string q;
                cout << "Search term: "; getline(cin, q);
                calendar.searchEvents(q);
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 4: {
                printHeader("CHECK TIME CONFLICTS");
                string date;
                bool validDate = false;
                
                while(!validDate) {
                    cout << "Enter date to check conflicts (DD:MM:YYYY): "; getline(cin, date);
                    validDate = validateDateFormat(date);
                    if(!validDate) {
                        cout << "Invalid date format! Please use DD:MM:YYYY format.\n";
                    }
                }
                
                calendar.checkConflicts(date);
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 5: {
                printHeader("VIEW ALL EVENTS");
                calendar.viewUpcoming();
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 6: {
                printHeader("UPDATE EVENT");
                string id;
                cout << "Enter Event ID to update: "; getline(cin, id);
                calendar.updateEvent(id);
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 7: {
                printHeader("DELETE EVENT");
                
                // Show all events first so user can see IDs
                cout << "Current Events (for reference):\n";
                cout << "--------------------------------\n";
                calendar.viewUpcoming();
                
                string id;
                cout << "\nEnter Event ID to delete: "; getline(cin, id);
                calendar.deleteEvent(id);
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 8: {
                printHeader("SAVE/LOAD DATA");
                cout << "1. Save to file\n";
                cout << "2. Load from file\n";
                cout << "3. Export to formatted TXT\n";
                cout << "4. Clear all events from memory\n";
				cout << "5. Back to Main Menu\n";
                cout << "Choice: ";
                int fc; 
                cin >> fc; 
                cin.ignore();

                switch(fc) {
                    case 1:
                        saveEventsToFile("calendar_data.txt", calendar.getBST());
                        break;
                        
                    case 2: {
                        // Clear current data before loading
                        cout << "\n[WARNING] This will replace all current events. Continue? (y/n): ";
                        char confirm;
                        cin >> confirm;
                        cin.ignore();
                        
                        if(confirm == 'y' || confirm == 'Y') {
                            CalendarSystem newCalendar;
                            calendar = newCalendar;
                            loadEventsFromFile("calendar_data.txt", calendar);
                        } else {
                            cout << "Load cancelled.\n";
                        }
                        break;
                    }
                    
                    case 3:
                        calendar.exportFormattedTXT("calendar_export.txt");
                        break;
                        
                    case 4: {
                        cout << "\n[WARNING] This will delete ALL events from memory. Continue? (y/n): ";
                        char confirm;
                        cin >> confirm;
                        cin.ignore();
                            
                        if(confirm == 'y' || confirm == 'Y') {
                            // Create empty calendar
                            CalendarSystem newCalendar;
                            calendar = newCalendar;
                            cout << "[SUCCESS] All events cleared from memory.\n";
                        } else {
                            cout << "Clear cancelled.\n";
                        }
                        break;
                    }
                    
                    case 5:
                        break;
                        
                    default:
                        cout << "Invalid choice!\n";
                        break;
                }

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 9: {
                printHeader("DATA STRUCTURES STATUS");
                calendar.displayStructures();
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 0: {
                printHeader("EXITING CALENDAR SYSTEM");
                cout << "Saving data...\n";
                saveEventsToFile("calendar_data.txt", calendar.getBST());
                cout << "Goodbye!\n";
                break;
            }
            
            default: {
                cout << "Invalid choice! Please try again.\n";
                cout << "Press Enter to continue...";
                cin.get();
                break;
            }
        }
    } while (choice != 0);
    
    return 0;
}