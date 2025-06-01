#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <map>
#include <cstdlib>

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

// Simulated contribution data (0-5 for each day)
std::map<std::string, int> generateMockData(int days_back = 365) {
    std::map<std::string, int> data;
    time_t now = time(0);
    for (int i = 0; i < days_back; ++i) {
        time_t day = now - i * 86400;
        tm* timeinfo = localtime(&day);

        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);

        data[buffer] = rand() % 6; // mock contribution level 0-5
    }
    return data;
}

void printGitActivity(const std::map<std::string, int>& activity) {
    // Get current date to align weeks
    time_t now = time(0);
    tm* today = localtime(&now);
    int start_weekday = today->tm_wday; // Sunday=0

    std::vector<std::vector<std::string>> grid(7); // rows: Sun to Sat
    std::vector<std::string> dates;

    // Fill grid from oldest to newest date
    for (auto it = activity.rbegin(); it != activity.rend(); ++it) {
        dates.push_back(it->first);
    }

    int col = 0;
    for (int i = dates.size() - 1; i >= 0; --i) {
        tm tm_date = {};
        strptime(dates[i].c_str(), "%Y-%m-%d", &tm_date);
        int day = tm_date.tm_wday;
        while (grid.size() <= day) grid.emplace_back();
        while (grid[day].size() < col) grid[day].push_back(shades[0]);

        int level = activity.at(dates[i]);
        grid[day].push_back(shades[level]);

        if (day == 6) ++col;
    }

    // Print header (weeks)
    std::cout << "    ";
    for (size_t i = 0; i < grid[0].size(); ++i)
        std::cout << " ";
    std::cout << "\n";

    // Days
    const char* day_labels[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for (int i = 0; i < 7; ++i) {
        std::cout << day_labels[i] << " ";
        for (const auto& cell : grid[i])
            std::cout << cell << RESET;
        std::cout << "\n";
    }
    std::cout << RESET;
}

int main() {
    auto mockData = generateMockData();
    printGitActivity(mockData);
    return 0;
}
