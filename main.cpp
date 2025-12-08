#include <iostream>
#include <iomanip>
#include <sys/stat.h>  // For file existence check
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
    cout << "\n==============================================================\n";
    cout << "                 CALENDAR SYSTEM (DSA)                    \n";
    cout << "==============================================================\n";
    cout << " " << title << "\n";
    cout << "--------------------------------------------------------------\n\n";
}

void showMenu() {
    clearScreen();
    cout << "\n==============================================================\n";
    cout << "                 CALENDAR SYSTEM (DSA)                    \n";
    cout << "==============================================================\n";
    cout << "  1. Add New Event                                        \n";
    cout << "  2. View Events by Date                                  \n";
    cout << "  3. Search Events                                        \n";
    cout << "  4. Check Time Conflicts                                 \n";
    cout << "  5. View Upcoming Events                                 \n";
    cout << "  6. Update Event                                         \n";
    cout << "  7. Delete Event                                         \n";
    cout << "  8. Save/Load Data                                       \n";
    cout << "  9. Display Data Structures                              \n";
    cout << "  0. Exit                                                 \n";
    cout << "==============================================================\n";
    cout << "  Choose option: ";
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
                
                cout << "Event Title : "; getline(cin, title);
                cout << "Date (YYYY-MM-DD) : "; getline(cin, date);
                cout << "Start Time (HH:MM) : "; getline(cin, start);
                cout << "Duration (minutes) : "; cin >> dur;
                cout << "Priority (1-5) : "; cin >> prio;
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
                cout << "Enter date (YYYY-MM-DD): "; getline(cin, date);
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
                cout << "Enter date to check conflicts: "; getline(cin, date);
                calendar.checkConflicts(date);
                
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }
            
            case 5: {
                printHeader("VIEW UPCOMING EVENTS");
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
                string id;
                cout << "Enter Event ID to delete: "; getline(cin, id);
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
                cout << "4. Data Management\n";
                cout << "Choice: ";
                int fc; cin >> fc; cin.ignore();
                
                if(fc == 1) {
                    saveEventsToFile("calendar_data.txt", calendar.getBST());
                } else if(fc == 2) {
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
                } else if(fc == 3) {
                    calendar.exportFormattedTXT("calendar_export.txt");
                } else if(fc == 4) {
                    cout << "\n1. Clear all events from memory\n";
                    cout << "2. Clean/Repair data file (remove duplicates)\n";
                    cout << "Choice: ";
                    int subChoice; cin >> subChoice; cin.ignore();
                    
                    if(subChoice == 1) {
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
                    } else if(subChoice == 2) {
                        cout << "\n[INFO] Cleaning data file (removing duplicates)...\n";
                        // Load and save to remove duplicates
                        CalendarSystem tempCalendar;
                        loadEventsFromFile("calendar_data.txt", tempCalendar);
                        saveEventsToFile("calendar_data.txt", tempCalendar.getBST());
                        cout << "[SUCCESS] Data file cleaned!\n";
                        
                        // Reload cleaned data into current calendar
                        cout << "\nReload cleaned data into current calendar? (y/n): ";
                        char reload;
                        cin >> reload;
                        cin.ignore();
                        
                        if(reload == 'y' || reload == 'Y') {
                            CalendarSystem newCalendar;
                            loadEventsFromFile("calendar_data.txt", newCalendar);
                            calendar = newCalendar;
                            cout << "[SUCCESS] Cleaned data reloaded.\n";
                        }
                    }
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