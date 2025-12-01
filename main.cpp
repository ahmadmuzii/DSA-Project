#include <iostream>
#include "calendar.h"
#include "file_manager.h"

using namespace std;

void showMenu() {
    cout << "\n=== CALENDAR SYSTEM (DSA Implementation) ===\n";
    cout << "1. Add New Event\n";
    cout << "2. View Events by Date\n";
    cout << "3. Search Events\n";
    cout << "4. Check Time Conflicts\n";
    cout << "5. View Upcoming Events\n";
    cout << "6. Save/Load Data\n";
    cout << "7. Display Data Structures\n";
    cout << "0. Exit\n";
    cout << "Choose option: ";
}

int main() {
    CalendarSystem calendar;
    int choice;
    
    // Auto-load on startup
    loadEventsFromFile("calendar_data.txt", calendar);

    do {
        showMenu();
        cin >> choice;
        cin.ignore(); // Clear newline buffer

        if (choice == 1) {
            string title, date, start;
            int dur, prio;
            
            cout << "\n=== ADD NEW EVENT ===\n";
            cout << "Enter event title: "; getline(cin, title);
            cout << "Enter date (YYYY-MM-DD): "; getline(cin, date);
            cout << "Enter start time (HH:MM): "; getline(cin, start);
            cout << "Enter duration (minutes): "; cin >> dur;
            cout << "Enter priority (1-5): "; cin >> prio;
            
            calendar.addEvent(title, date, start, dur, prio, "");

        } else if (choice == 2) {
            string date;
            cout << "Enter date (YYYY-MM-DD): "; getline(cin, date);
            calendar.viewEventsByDate(date);

        } else if (choice == 3) {
            string q;
            cout << "Search term: "; getline(cin, q);
            calendar.searchEvents(q);

        } else if (choice == 4) {
            string date;
            cout << "Enter date to check conflicts: "; getline(cin, date);
            calendar.checkConflicts(date);

        } else if (choice == 5) {
            calendar.viewUpcoming();

        } else if (choice == 6) {
            cout << "1. Save\n2. Load\nChoice: ";
            int fc; cin >> fc;
            if(fc == 1) saveEventsToFile("calendar_data.txt", calendar.getBST());
            else loadEventsFromFile("calendar_data.txt", calendar);

        } else if (choice == 7) {
            calendar.displayStructures();
        }

    } while (choice != 0);

    cout << "Exiting... Saving data...\n";
    saveEventsToFile("calendar_data.txt", calendar.getBST());
    
    return 0;
}