#include "event.h"
#include <string>

using namespace std;

// Implementation of the getEndTime function
string Event::getEndTime() const {
    if (startTime.empty()) {
        return "00:00";
    }

    // Parse "HH:MM" manually or using stoi
    int h = stoi(startTime.substr(0, 2));
    int m = stoi(startTime.substr(3, 2));

    // Add duration
    m += durationMins;

    // Handle overflow (minutes to hours)
    h += m / 60;
    m %= 60;

    // Track day overflow instead of silently wrapping
    int overflowDays = 0;
    while (h >= 24) {
        h -= 24;
        overflowDays++;
    }

    // Format back to string "HH:MM"
    string hStr = (h < 10 ? "0" : "") + to_string(h);
    string mStr = (m < 10 ? "0" : "") + to_string(m);

    string result = hStr + ":" + mStr;
    if (overflowDays > 0) {
        result += " +" + to_string(overflowDays) + "d";
    }
    return result;
}