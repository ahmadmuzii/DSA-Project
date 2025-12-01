#include "event.h"
#include <string>

using namespace std;

// Implementation of the getEndTime function
string Event::getEndTime() const {
    if (startTime.empty()) return "00:00";

    // Parse "HH:MM" manually or using stoi
    int h = stoi(startTime.substr(0, 2));
    int m = stoi(startTime.substr(3, 2));

    // Add duration
    m += durationMins;

    // Handle overflow (minutes to hours)
    h += m / 60;
    m %= 60;

    // Handle day overflow (optional, usually wraps around 24h)
    h %= 24;

    // Format back to string "HH:MM"
    string hStr = (h < 10 ? "0" : "") + to_string(h);
    string mStr = (m < 10 ? "0" : "") + to_string(m);

    return hStr + ":" + mStr;
}