#include "aho_corasick.hpp"
#include "log_reader.hpp"

#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <fstream>

// Ask user for an int input until they enter value within range
int getMenuChoice(int minimum, int maximum) {
	int choice;

	while (
		!(std::cin >> choice) ||
		choice < minimum ||
		choice > maximum
		) {
		std::cout
			<< "Invalid input. Enter a number from "
			<< minimum << " to " << maximum << ": ";

		std::cin.clear();
		std::cin.ignore(
			std::numeric_limits<std::streamsize>::max(),
			'\n'
		);
	}

	std::cin.ignore(
		std::numeric_limits<std::streamsize>::max(),
		'\n'
	);

	return choice;
}

void removeSurroundingQuotes(std::string& path) {
	if (
		path.size() >= 2 &&
		path.front() == '"' &&
		path.back() == '"'
	) {
		path = path.substr(1, path.size() - 2);
	}
}

void configurePatterns(AhoCorasick& automaton) {
	std::cout
		<< "How will you provide patterns:\n"
		<< "   1. Type them now\n"
		<< "   2. Load a .txt file\n"
		<< "Selection (1 or 2): ";
	int patternMode = getMenuChoice(1, 2);

	if (patternMode == 1) {
		std::cout
			<< "Enter one pattern per line.\n"
			<< "Enter a blank line when finished:\n";

		std::string pattern;

		while (true) {
			std::cout << "Pattern: ";
			std::getline(std::cin, pattern);

			if (pattern.empty()) {
				break;
			}
			// Add to the trie
			automaton.addPattern(pattern);
		}
	}
	else {
		std::cout << "Path to patterns .txt file: ";

		std::string patternFilePath;
		std::getline(std::cin, patternFilePath);
		removeSurroundingQuotes(patternFilePath);

		std::ifstream patternFile(patternFilePath);

		if (!patternFile.is_open()) {
			throw std::runtime_error("Could not open pattern file: " + patternFilePath);
		}

		std::string pattern;

		while (std::getline(patternFile, pattern)) {
			if (!pattern.empty()) {
				// Add to the trie
				automaton.addPattern(pattern);
			}
		}
	}
	// Build failure function
	automaton.buildFailureLinks();
}


int main() {
	try {
		AhoCorasick automaton;
		LogReader logReader;

		// 1. Ask for pattern input method
		configurePatterns(automaton);

		// 2. Ask reading log mode
		std::cout
			<< "Choose monitoring mode:\n"
			<< "   1. Scan an existing log file, then exit\n"
			<< "   2. Monitor a live server log\n"
			<< "Selection (1 or 2): ";
		int monitoringMode = getMenuChoice(1, 2);

		// 2-1. Read existing log file
		if (monitoringMode == 1) {
			// run program
			std::cout << "Path to log file: ";
			std::string logFilePath;
			std::getline(std::cin, logFilePath);
			removeSurroundingQuotes(logFilePath);

			std::cout << "Scanning existing entries...\n";
			std::vector<LogMatch> logMatches = logReader.scanFile(logFilePath, automaton);

			if (logMatches.empty()) {
				std::cout << "No matching patterns found.\n";
			}
			else {
				for (const LogMatch& logMatch : logMatches) {
					const Match& match = logMatch.match;

					std::cout
						<< "[MATCH] Pattern: \""
						<< automaton.getPattern(match.patternID)
						<< "\", line: " << logMatch.lineNumber
						<< ", position: " << match.position
						<< '\n';
				}
			}
		}

		// 2-2. Read live server log
		else {
			std::cout << "Path to the running server's access log: ";
			std::string logFilePath;
			std::getline(std::cin, logFilePath);
			removeSurroundingQuotes(logFilePath);

			std::cout
				<< "Choose live monitoring mode:\n"
				<< "   1. Scan existing log entries, then monitor new entries\n"
				<< "   2. Monitor new entries only\n"
				<< " Selection (1 or 2): ";
			int liveMode = getMenuChoice(1, 2);
			bool includeExisting = (liveMode == 1);

			std::cout
				<< "Monitoring log entries. Press Ctrl+C to stop.\n";

			logReader.followFile(logFilePath, automaton, includeExisting);
		}
	}
	catch (const std::exception& error){
		std::cerr << "Error: " << error.what() << '\n';
		return 1;
	}
	return 0;
}