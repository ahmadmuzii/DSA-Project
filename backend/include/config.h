#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    inline const std::string DATA_FILE = "calendar_data.txt";
    inline const std::string COUNTER_FILE = "event_counter.txt";
    inline const std::string EXPORT_FILE = "calendar_export.txt";
    inline const int DEFAULT_PORT = 8080;
}

#endif
