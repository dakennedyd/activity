# Git Activity Visualization

![Screenshot](https://github.com/dakennedyd/activity/screenshot.jpg)

This program visualizes Git activity over the past year as a colorful grid.

## Get the code

```bash
git clone https://github.com/dakennedyd/activity
```

## Compilation

```bash
g++ -o activity main.cpp
```

## Usage

```
./activity [optional: repository path]
```

If no repository path is specified, it will use the current directory.


## How It Works

1. The program runs a Git command to fetch commit logs for the last year.
2. It parses the output to count commits per day.
3. A 7xN grid is created, with each cell representing a day.
4. Cells are colored based on the number of commits:
   - Level 1: 1-1 commit
   - Level 2: 2-4 commits
   - Level 3: 5-9 commits
   - Level 4: 10-14 commits
   - Level 5: 15+ commits

## Requirements

- Requires Git to be installed on the system

## Contributing

Feel free to fork and contribute! This project is open-source.
