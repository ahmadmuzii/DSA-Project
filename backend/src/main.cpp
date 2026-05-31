#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <ctime>
#include "calendar.h"
#include "file_manager.h"

#ifdef _WIN32
    #include <windows.h>
    #define STAT _stat
#else
    #include <unistd.h>
    #define STAT stat
#endif

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(const string& title) {
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

bool validateDateFormat(const string& date) {
    if (date.length() != 10) return false;
    if (date[2] != ':' || date[5] != ':') return false;

    string dayStr = date.substr(0, 2);
    string monthStr = date.substr(3, 2);
    string yearStr = date.substr(6, 4);

    for (char c : dayStr) if (!isdigit(c)) return false;
    for (char c : monthStr) if (!isdigit(c)) return false;
    for (char c : yearStr) if (!isdigit(c)) return false;

    int day = stoi(dayStr);
    int month = stoi(monthStr);
    int year = stoi(yearStr);

    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeap) daysInMonth[1] = 29;
    }
    if (day < 1 || day > daysInMonth[month - 1]) return false;

    return true;
}

bool validateTimeFormat(const string& time) {
    if (time.length() != 5) return false;
    if (time[2] != ':') return false;

    string hourStr = time.substr(0, 2);
    string minuteStr = time.substr(3, 2);

    for (char c : hourStr) if (!isdigit(c)) return false;
    for (char c : minuteStr) if (!isdigit(c)) return false;

    int hour = stoi(hourStr);
    int minute = stoi(minuteStr);

    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;

    return true;
}

string formatEventBox(const Event& e, int num) {
    string s;
    s += "\n+----------------- EVENT #" + to_string(num) + " -----------------+\n";
    s += "| Title    : " + left << setw(38) << e.title + " |\n";
    s += "| ID       : " + left << setw(38) << e.id + " |\n";
    s += "| Time     : " + left << setw(38) << (e.startTime + " - " + e.getEndTime()) + " |\n";
    s += "| Duration : " + left << setw(38) << (to_string(e.durationMins) + " minutes") + " |\n";
    s += "| Priority : " + left << setw(38) <<
         (to_string(e.priority) + " (" +
         (e.priority == 1 ? "Highest" :
          e.priority == 2 ? "High" :
          e.priority == 3 ? "Medium" :
          e.priority == 4 ? "Low" : "Lowest") + ")") + " |\n";
    if (!e.description.empty()) {
        s += "| Descrip. : " + left << setw(38) << e.description + " |\n";
    }
    s += "+-----------------------------------------------------+\n";
    return s;
}

void printEvents(const vector<Event>& events, const string& title) {
    cout << "\n-----------------------------------------------------------\n";
    cout << "                  " << left << setw(30) << title << "\n";
    cout << "-----------------------------------------------------------\n";

    if (events.empty()) {
        cout << "\n   No events found.\n";
        return;
    }

    for (size_t i = 0; i < events.size(); i++) {
        cout << formatEventBox(events[i], (int)(i + 1));
    }
    cout << "\nTotal Events: " << events.size() << "\n";
}

int main() {
    CalendarSystem calendar;
    int choice;

    struct STAT buffer;
    if (STAT("calendar_data.txt", &buffer) == 0) {
        if (buffer.st_size > 0) {
            cout << "\n[INFO] Loading saved data from calendar_data.txt...\n";
            int loaded = loadEventsFromFile("calendar_data.txt", calendar);
            if (loaded > 0) {
                cout << "[INFO] Data loaded successfully!\n";
            }
        } else {
            cout << "\n[INFO] calendar_data.txt exists but is empty. Starting with empty calendar.\n";
        }
    } else {
        cout << "\n[INFO] No saved data found. Starting with empty calendar.\n";
    }

    #ifdef _WIN32
        Sleep(1500);
    #else
        usleep(1500000);
    #endif

    do {
        showMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                printHeader("ADD NEW EVENT");
                string title, date, start, desc;
                int dur, prio;

                cout << "Event Title : "; getline(cin, title);

                while (true) {
                    cout << "Date (DD:MM:YYYY) : "; getline(cin, date);
                    if (validateDateFormat(date)) break;
                    cout << "Invalid date format! Please use DD:MM:YYYY format.\n";
                }

                while (true) {
                    cout << "Start Time (HH:MM) : "; getline(cin, start);
                    if (validateTimeFormat(start)) break;
                    cout << "Invalid time format! Please use HH:MM format (24-hour).\n";
                }

                cout << "Duration (minutes) : "; cin >> dur;

                while (true) {
                    cout << "Priority (1-5) : "; cin >> prio;
                    if (prio >= 1 && prio <= 5) break;
                    cout << "Priority must be between 1 and 5!\n";
                }

                cin.ignore();
                cout << "Description (optional) : "; getline(cin, desc);

                if (calendar.addEvent(title, date, start, dur, prio, desc)) {
                    cout << "\n-----------------------------------------------------------\n";
                    cout << "                    EVENT ADDED SUCCESSFULLY!             \n";
                    cout << "-----------------------------------------------------------\n";
                } else {
                    cout << "\n-----------------------------------------------------------\n";
                    cout << "                    CONFLICT DETECTED!                    \n";
                    cout << "-----------------------------------------------------------\n";
                    cout << "\n[WARNING] Event conflicts with an existing event on this date and time.\n";
                }

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }

            case 2: {
                printHeader("VIEW EVENTS BY DATE");
                string date;

                while (true) {
                    cout << "Enter date (DD:MM:YYYY): "; getline(cin, date);
                    if (validateDateFormat(date)) break;
                    cout << "Invalid date format! Please use DD:MM:YYYY format.\n";
                }

                auto events = calendar.getEventsByDate(date);
                if (events.empty()) {
                    cout << "\nNo events found for " << date << "\n";
                } else {
                    printEvents(events, "EVENTS FOR " + date);
                }

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }

            case 3: {
                printHeader("SEARCH EVENTS");
                string q;
                cout << "Search term: "; getline(cin, q);

                auto results = calendar.searchEvents(q);
                if (results.empty()) {
                    cout << "\nNo matches found for \"" << q << "\"\n";
                } else {
                    cout << "\nFound " << results.size() << " match(es):\n";
                    for (auto& e : results) {
                        cout << "\n* " << e.title << "\n";
                        cout << "  Date: " << e.date << " | Time: " << e.startTime << " | ID: " << e.id << "\n";
                    }
                }

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }

            case 4: {
                printHeader("CHECK TIME CONFLICTS");
                string date;

                while (true) {
                    cout << "Enter date to check conflicts (DD:MM:YYYY): "; getline(cin, date);
                    if (validateDateFormat(date)) break;
                    cout << "Invalid date format! Please use DD:MM:YYYY format.\n";
                }

                auto result = calendar.checkConflicts(date);
                if (result.conflicts.empty()) {
                    auto events = calendar.getEventsByDate(date);
                    cout << "\nNo overlapping events found for " << date << "\n";
                    cout << "Total events scheduled: " << events.size() << "\n";
                } else {
                    cout << "\n[CONFLICTS FOUND]:\n";
                    for (auto& pair : result.conflicts) {
                        cout << "\nConflict between:\n";
                        cout << "   * " << pair.a.title << " (" << pair.a.startTime << "-" << pair.a.getEndTime() << ")\n";
                        cout << "   * " << pair.b.title << " (" << pair.b.startTime << "-" << pair.b.getEndTime() << ")\n";
                    }
                }

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }

            case 5: {
                printHeader("VIEW ALL EVENTS");
                auto upcoming = calendar.viewUpcoming(20);
                if (upcoming.empty()) {
                    cout << "\n   No upcoming events\n";
                } else {
                    cout << "\nNext Upcoming Events:\n";
                    cout << "--------------------------------------------------------------\n";
                    for (auto& e : upcoming) {
                        cout << "\n  * " << e.date << " | " << e.startTime << " - " << e.getEndTime() << "\n";
                        cout << "    " << e.title << " (Priority: " << e.priority << ", ID: " << e.id << ")\n";
                        if (!e.description.empty()) {
                            cout << "    Description: " << e.description << "\n";
                        }
                    }
                }

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }

            case 6: {
                printHeader("UPDATE EVENT");
                string id;
                cout << "Enter Event ID to update: "; getline(cin, id);

                Event* ev = calendar.getEventById(id);
                if (!ev) {
                    cout << "\n[ERROR] Event with ID \"" << id << "\" not found!\n";
                    cout << "\nPress Enter to continue...";
                    cin.get();
                    break;
                }

                cout << "\nCurrent Event Details:\n";
                cout << "   +-------------------------------------+\n";
                cout << "   | Title    : " << setw(25) << ev->title << " |\n";
                cout << "   | Date     : " << setw(25) << ev->date << " |\n";
                cout << "   | Time     : " << setw(25) << ev->startTime << " |\n";
                cout << "   | Duration : " << setw(25) << to_string(ev->durationMins) + " mins" << " |\n";
                cout << "   | Priority : " << setw(25) << to_string(ev->priority) << " |\n";
                cout << "   | Descrip. : " << setw(25) << ev->description << " |\n";
                cout << "   +-------------------------------------+\n";

                cout << "\nEnter new details (press Enter to keep current value):\n";

                string newTitle, newDate, newTime, durInput, prioInput, newDescription;
                int newDuration, newPriority;

                cout << "   New Title [" << ev->title << "]: ";
                getline(cin, newTitle);
                if (newTitle.empty()) newTitle = ev->title;

                cout << "   New Date (DD:MM:YYYY) [" << ev->date << "]: ";
                getline(cin, newDate);
                if (newDate.empty()) newDate = ev->date;

                cout << "   New Start Time (HH:MM) [" << ev->startTime << "]: ";
                getline(cin, newTime);
                if (newTime.empty()) newTime = ev->startTime;

                cout << "   New Duration (minutes) [" << ev->durationMins << "]: ";
                getline(cin, durInput);
                try {
                    newDuration = durInput.empty() ? ev->durationMins : stoi(durInput);
                } catch (...) {
                    newDuration = ev->durationMins;
                }

                cout << "   New Priority (1-5) [" << ev->priority << "]: ";
                getline(cin, prioInput);
                try {
                    newPriority = prioInput.empty() ? ev->priority : stoi(prioInput);
                } catch (...) {
                    newPriority = ev->priority;
                }

                cout << "   New Description [" << ev->description << "]: ";
                getline(cin, newDescription);
                if (newDescription.empty()) newDescription = ev->description;

                if (calendar.updateEvent(id, newTitle, newDate, newTime,
                                         newDuration, newPriority, newDescription)) {
                    cout << "\n[SUCCESS] Event updated successfully!\n";
                } else {
                    cout << "\n[ERROR] Could not update event. Possible time conflict or event not found.\n";
                }

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }

            case 7: {
                printHeader("DELETE EVENT");

                cout << "Current Events (for reference):\n";
                cout << "--------------------------------\n";
                auto allEvents = calendar.viewUpcoming(50);
                for (auto& e : allEvents) {
                    cout << "  * " << e.id << " - " << e.title << " (" << e.date << ")\n";
                }

                string id;
                cout << "\nEnter Event ID to delete: "; getline(cin, id);

                Event* eventToDelete = calendar.getEventById(id);
                if (!eventToDelete) {
                    cout << "\n[ERROR] Event with ID \"" << id << "\" not found!\n";
                } else {
                    cout << "\nEvent to Delete:\n";
                    cout << "   * Title: " << eventToDelete->title << "\n";
                    cout << "   * Date: " << eventToDelete->date << "\n";
                    cout << "   * Time: " << eventToDelete->startTime << "\n";
                    cout << "   * ID: " << eventToDelete->id << "\n";

                    cout << "\nAre you sure you want to delete this event? (y/n): ";
                    char confirm;
                    cin >> confirm;
                    cin.ignore();

                    if (confirm == 'y' || confirm == 'Y') {
                        if (calendar.deleteEvent(id)) {
                            cout << "\n[SUCCESS] Event deleted successfully from all data structures!\n";
                        } else {
                            cout << "\n[WARNING] Event partially deleted. Data may be inconsistent.\n";
                        }
                    } else {
                        cout << "   Deletion cancelled.\n";
                    }
                }

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

                switch (fc) {
                    case 1:
                        saveEventsToFile("calendar_data.txt", calendar);
                        break;

                    case 2: {
                        cout << "\n[WARNING] This will replace all current events. Continue? (y/n): ";
                        char confirm;
                        cin >> confirm;
                        cin.ignore();

                        if (confirm == 'y' || confirm == 'Y') {
                            calendar.clearAllEvents();
                            loadEventsFromFile("calendar_data.txt", calendar);
                        } else {
                            cout << "Load cancelled.\n";
                        }
                        break;
                    }

                    case 3:
                        if (calendar.exportFormattedTXT("calendar_export.txt")) {
                            cout << "[SUCCESS] Calendar exported to calendar_export.txt with formatting!\n";
                        } else {
                            cout << "[ERROR] Error opening file for export!\n";
                        }
                        break;

                    case 4: {
                        cout << "\n[WARNING] This will delete ALL events from memory. Continue? (y/n): ";
                        char confirm;
                        cin >> confirm;
                        cin.ignore();

                        if (confirm == 'y' || confirm == 'Y') {
                            calendar.clearAllEvents();
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
                cout << calendar.displayStructures();

                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            }

            case 0: {
                printHeader("EXITING CALENDAR SYSTEM");
                cout << "Saving data...\n";
                saveEventsToFile("calendar_data.txt", calendar);
                cout << "Goodbye!\n";
                break;
            }

            default:
                cout << "Invalid choice! Please try again.\n";
                cout << "Press Enter to continue...";
                cin.get();
                break;
        }
    } while (choice != 0);

    return 0;
}
