/**
 * time_map.h — Maps system clock hour to the correct Anya image
 * 
 * WHAT: Contains the 13 hour-range→filename entries (ported from the
 *       original JS config). Given the current hour, returns which
 *       PNG to display. Also provides a formatted time string.
 * 
 * WHY:  Anya has different mood images for different times of day.
 *       This file is the single source of truth for which image
 *       shows when. To add/change time ranges, edit the table below.
 * 
 * FUNCTIONS:
 *   GetTimeMap()           → the raw table of {startHour, endHour, filename}
 *   GetImageForCurrentHour() → returns "moods/time/6am8am.png" etc.
 *   GetCurrentTimeString()   → returns "3:42 PM" for the time pill overlay
 */
#pragma once
#include <string>
#include <ctime>
#include <vector>

// Direct port of config/timeMapConfig.js
struct TimeEntry {
    int startHour;  // inclusive
    int endHour;    // exclusive
    const char* filename;
};

inline const std::vector<TimeEntry>& GetTimeMap() {
    static const std::vector<TimeEntry> map = {
        {  1,  6, "1am6am.png"    },
        {  6,  8, "6am8am.png"    },
        {  8, 10, "8am10am.png"   },
        { 10, 12, "10am12pm.png"  },
        { 12, 13, "12am1pm.png"   },
        { 13, 14, "1pm2pm.png"    },
        { 14, 16, "2pm4pm.png"    },
        { 16, 18, "4pm6pm.png"    },
        { 18, 19, "6pm7pm.png"    },
        { 19, 20, "7pm8pm.png"    },
        { 20, 23, "8pm11pm.png"   },
        { 23, 24, "11pm12am.png"  },
        {  0,  1, "12am1am.png"   },
    };
    return map;
}

// Returns the full relative path to the image for the current hour
inline std::string GetImageForCurrentHour() {
    std::time_t now = std::time(nullptr);
    std::tm local{};
    localtime_s(&local, &now);
    int hour = local.tm_hour;

    for (const auto& entry : GetTimeMap()) {
        if (hour >= entry.startHour && hour < entry.endHour) {
            return std::string("moods/time/") + entry.filename;
        }
    }
    // Fallback
    return "moods/extra/anya1.jpeg";
}

// Returns a formatted time string like "3:42 PM"
inline std::string GetCurrentTimeString() {
    std::time_t now = std::time(nullptr);
    std::tm local{};
    localtime_s(&local, &now);

    int hour12 = local.tm_hour % 12;
    if (hour12 == 0) hour12 = 12;
    const char* ampm = (local.tm_hour >= 12) ? "PM" : "AM";

    char buf[32];
    snprintf(buf, sizeof(buf), "%d:%02d %s", hour12, local.tm_min, ampm);
    return std::string(buf);
}
