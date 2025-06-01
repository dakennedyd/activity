# Git Activity Visualization

This program visualizes Git activity over the past year as a colorful grid.

## Usage

To run the program:

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

## Limitations

- Requires Git to be installed on the system
- Performance may degrade for very large repositories

## Contributing

Feel free to fork and contribute! This project is open-source.

## License

MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
