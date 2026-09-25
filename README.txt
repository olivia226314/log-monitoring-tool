Log Monitoring Tool Using Aho-Corasick
==========================

This C++ console program searches log entries for user-supplied patterns using the Aho-Corasick algorithm. It can scan an existing log file or monitor a log file for new entries.


Requirements
-------------------
A c++20-compatible compiler is needed to build the program. Visual Studio is not required to run it once compiled.


Build and Run with Visual Studio
---------------------------------------
1. Open 41052_Ass1.slnx in Visual Studio
2. Select Build > Build Solution
3. Select Debug > Start Without Debugging (Ctrl+F5)
4. Follow the prompts in the console


You can also open Command Prompt in the folder containing the build 41052_Ass1.exe and run:
41052_Ass1.exe


Build and Run with g++
----------------------------
Open Command Prompt or PowerShell in the folder containing main.cpp, aho_corasick.cpp, and log_reader.cpp. 

Compile:
g++ -std=c++20 main.cpp aho_corasick.cpp log_reader.cpp -o LogMonitor.exe

Run in Command Prompt:
LogMonitor.exe

Run in PowerShell: 
.\LogMonitor.exe

This option requires g++ to be installed and available in Command Prompt.


How to Use
----------------
1. Enter patterns manually or provide a text file with one pattern per line
2. Choose whether to scan an existing log file or monitor new entries
3. Enter the path to the log file when prompted
4. If monitoring, append new lines to that file while the program runs. Press Ctrl+C to stop

For a simple test, use the patterns "cat" and "at" and a log line containing "A cat appeared". The program should find both patterns in that line.


Source Files
----------------
main.cpp				Console menus and program flow
aho_corasick.hpp/.cpp		Pattern trie, failure links, and matching
log_reader.hpp/.cpp		Existing file scanning and live monitoring

The .slnx and .vcxproj files let Visual Studio open and build the project.


Notes
--------
The program checks each log line for text patterns. A match does not, by itself, prove that an event is malicious. Live monitoring checks the file periodically, so a new entry may take a short time to appear.
