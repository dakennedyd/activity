#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L // Ensure consistent time functions (for localtime_r, mktime, etc.)
#endif

#include <iostream>
#include <vector>
#include <string>
#include <ctime>      // for time_t, tm, localtime_r, mktime, strftime
#include <map>
#include <cstdlib>
#include <sstream>
#include <array>
#include <cstdio>     // for popen, pclose, FILE, fgets

// Terminal color codes for shades of green
const std::vector<std::string> shades = {
    "\033[48;5;232m ", // No activity
    "\033[48;5;22m ",  // Light green
    "\033[48;5;28m ",
    "\033[48;5;34m ",
    "\033[48;5;40m ",  // Darker green
    "\033[48;5;46m "   // Most active
};

const std::string RESET = "\033[0m";

// Run a command and capture the output (stdout + stderr)
std::string execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;

    // Append "2>&1" so that stderr is merged into stdout, allowing us to detect Git errors
    std::string fullCmd = cmd + " 2>&1";
    FILE* pipe = popen(fullCmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: popen() failed\n";
        return "";
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }

    int returnCode = pclose(pipe);
    if (returnCode != 0) {
        // Git or some other command failed; return the combined output so the caller can inspect
        return result;
    }

    return result;
}

// Parse git log dates and count contributions per day
std::map<std::string, int> getGitContributions(const std::string& repo_path = ".") {
    // Escape single quotes in path so we can safely wrap it in '…'
    std::string safe_path = "'";
    for (char c : repo_path) {
        if (c == '\'') safe_path += "'\\''";
        else safe_path += c;
    }
    safe_path += "'";

    // Use only "--date=short" (which prints "YYYY-MM-DD" in local time) and a stable "1 year ago" syntax
    std::string cmd =
        "git -C " + safe_path +
        " log --since=\"1 year ago\" --date=short --pretty=format:%ad";

    std::string output = execCommand(cmd);
    if (output.rfind("fatal:", 0) == 0) {
        // Git reported a fatal error (e.g. not a Git repository)
        std::cerr << output;
        return {};
    }

    std::istringstream stream(output);
    std::string line;
    std::map<std::string, int> contributions;
    while (std::getline(stream, line)) {
        if (!line.empty()) {
            ++contributions[line];
        }
    }
    return contributions;
}

// Print a year’s worth of Git activity as a 7×N grid, colored by number of commits
void printGitActivity(const std::map<std::string, int>& activity) {
    // Compute "today at local midnight"
    time_t now = time(nullptr);
    tm today_tm;
    localtime_r(&now, &today_tm);
    today_tm.tm_hour = 0;
    today_tm.tm_min  = 0;
    today_tm.tm_sec  = 0;
    time_t today_midnight = mktime(&today_tm);

    // Compute "one year ago at the same local midnight"
    tm one_year_tm = today_tm;
    one_year_tm.tm_year -= 1;           // subtract one year
    time_t one_year_midnight = mktime(&one_year_tm);

    // Determine how many days are in that span (could be 365 or 366)
    int total_days = static_cast<int>((today_midnight - one_year_midnight) / 86400) + 1;

    // Compute number of weeks needed (ceiling of total_days/7)
    int total_weeks = (total_days + 6) / 7;

    // Initialize a 7×total_weeks grid with "no activity" shade (shades[0])
    std::vector<std::vector<std::string>> grid(7, std::vector<std::string>(total_weeks, shades[0]));

    // Track where each month label should start
    std::vector<int> month_starts;
    std::vector<std::string> month_labels;
    int prev_month = -1;

    // Walk day-by-day, from one_year_midnight up to today_midnight
    tm cur = one_year_tm;  // this is one_year_midnight in tm form (normalized by mktime)
    for (int day_index = 0; day_index < total_days; ++day_index) {
        // Format current date as YYYY-MM-DD
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &cur);
        std::string date_str(buffer);

        // Lookup how many commits on that date
        int level = 0;
        auto it = activity.find(date_str);
        if (it != activity.end()) {
            int count = it->second;
            if (count >= 15)        level = 5;
            else if (count >= 10)   level = 4;
            else if (count >= 5)    level = 3;
            else if (count >= 2)    level = 2;
            else /* count >= 1 */   level = 1;
        }

        // Determine weekday (0=Sunday..6=Saturday) and week index (0..total_weeks-1)
        int weekday = cur.tm_wday;
        int week_index = day_index / 7;
        if (week_index < total_weeks) {
            grid[weekday][week_index] = shades[level];
        }

        // If this is within the first 7 days of a month, and the month changed, record the label
        if (cur.tm_mday <= 7 && (prev_month == -1 || cur.tm_mon != prev_month)) {
            month_starts.push_back(week_index);
            char month_name[4]; // "Jan", "Feb", etc.
            strftime(month_name, sizeof(month_name), "%b", &cur);
            month_labels.push_back(month_name);
            prev_month = cur.tm_mon;
        }

        // Move cur forward by one calendar day (handles DST, month/year roll)
        cur.tm_mday += 1;
        mktime(&cur); // normalize fields (tm_wday, tm_yday, carry months/years, etc.)
    }

    // Print month labels across the top
    // print 4 spaces of left padding, then position each month label above its week column.
    std::cout << "    ";
    int col = 0;
    for (size_t i = 0; i < month_starts.size(); ++i) {
        int pos = month_starts[i];
        // Move from current printed column to the target week column
        while (col < pos) {
            std::cout << " ";
            ++col;
        }
        // Print the 3-letter month name
        std::cout << month_labels[i];
        col += static_cast<int>(month_labels[i].length());
    }
    std::cout << "\n";

    // Print each weekday line, followed by the grid cells for all weeks
    const char* day_labels[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for (int r = 0; r < 7; ++r) {
        std::cout << day_labels[r] << " ";
        for (int c = 0; c < total_weeks; ++c) {
            std::cout << grid[r][c] << RESET;
        }
        std::cout << "\n";
    }
    std::cout << RESET;
}

int main(int argc, char* argv[]) {
    std::string path = ".";
    if (argc > 1) {
        path = argv[1];
    }

    auto contributions = getGitContributions(path);
    if (contributions.empty()) {
        std::cerr << "Warning: No commits found in the past year or Git command failed.\n";
    }

    printGitActivity(contributions);
    return 0;
}
