#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <sstream>
#include <cstdio>

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

// Run a command and capture the output
std::string execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

// Parse git log dates and count contributions per day
std::map<std::string, int> getGitContributions(const std::string& repo_path = ".") {
    std::map<std::string, int> contributions;
    std::string cmd = "git -C " + repo_path + " log --since=1.year --date=short --pretty=format:%ad";
    std::string output = execCommand(cmd);
    std::istringstream stream(output);
    std::string line;
    while (std::getline(stream, line)) {
        ++contributions[line];
    }
    return contributions;
}

void printGitActivity(const std::map<std::string, int>& activity) {
    // Get current date to align weeks
    time_t now = time(0);
    tm* today = localtime(&now);
    int start_weekday = today->tm_wday; // Sunday=0

    std::vector<std::vector<std::string>> grid(7); // rows: Sun to Sat
    std::vector<std::string> dates;

    // Generate last 365 dates and fill activity level
    for (int i = 364; i >= 0; --i) {
        time_t day = now - i * 86400;
        tm* timeinfo = localtime(&day);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        std::string date_str(buffer);

        int level = 0;
        if (activity.find(date_str) != activity.end()) {
            int count = activity.at(date_str);
            if (count >= 15) level = 5;
            else if (count >= 10) level = 4;
            else if (count >= 5) level = 3;
            else if (count >= 2) level = 2;
            else if (count >= 1) level = 1;
        }

        int weekday = timeinfo->tm_wday;
        grid[weekday].push_back(shades[level]);
    }

    // Print day labels
    const char* day_labels[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for (int i = 0; i < 7; ++i) {
        std::cout << day_labels[i] << " ";
        for (const auto& cell : grid[i]) {
            std::cout << cell << RESET;
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
    printGitActivity(contributions);
    return 0;
}
