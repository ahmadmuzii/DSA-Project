#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <iostream>

using namespace std;

struct Event {
    string id;
    string title;
    string date;      // Format: YYYY-MM-DD
    string startTime; // Format: HH:MM
    int durationMins;
    int priority;     // 1 (High) to 5 (Low)

    // Constructor for easy initialization (Optional but helpful)
    Event() : durationMins(0), priority(0) {}

    // Declaration only - Implementation is in event.cpp
    string getEndTime() const;
};

#endif