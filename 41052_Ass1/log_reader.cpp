#include "log_reader.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

// Search every line in an existing log file
std::vector<LogMatch> LogReader::scanFile(
	const std::string& filePath,
	const AhoCorasick& automaton
) const {
	std::vector<LogMatch> logMatches;
	std::ifstream file(filePath);

	if (!file.is_open()) {
		throw std::runtime_error("Could not open log file: " + filePath);
	}

	std::string line;
	std::size_t lineNumber = 1;

	while (std::getline(file, line)) {
		std::vector<Match> matches = automaton.search(line);
		for (const Match& match : matches) {
			logMatches.push_back({ lineNumber, match });
		}

		lineNumber++;
	}
	return logMatches;
}

// Check a log file repeatedly for appended lines
void LogReader::followFile(
	const std::string& filePath,
	const AhoCorasick& automaton, 
	bool includeExisting
) const {
	std::streamoff readPosition = 0;
	
	// Validate the file & choose where monitoring begins
	{
		std::ifstream file(filePath, std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Could not open log file: " + filePath);
		}

		if (!includeExisting) {
			file.seekg(0, std::ios::end);
			readPosition = file.tellg();
		}
	} // file close here

	// Begin continuous monitoring
	while (true) {
		std::ifstream file(filePath, std::ios::binary);

		if (file.is_open()) {
			// check if the log was truncated
			file.seekg(0, std::ios::end);
			std::streamoff currentSize = file.tellg();

			if (currentSize < readPosition) {
				readPosition = 0;
			}

			file.seekg(readPosition);
			std::string line;

			while (std::getline(file, line)) {
				// Count the bytes before removing '\r'
				std::streamoff bytesRead = static_cast<std::streamoff>(line.size());

				// getline removes '\n', so include it when presnt
				if (!file.eof()) {
					bytesRead++;
				}

				readPosition += bytesRead;

				if (!line.empty() && line.back() == '\r') {
					line.pop_back();
				}

				std::vector<Match> matches = automaton.search(line);

				for (const Match& match : matches) {
					std::cout
						<< "[ALERT] Pattern: \""
						<< automaton.getPattern(match.patternID)
						<< "\", position: "
						<< match.position
						<< '\n';
				}
			}
		} 
		file.close();  // file closes here

		// Pause before checking for more data
		std::this_thread::sleep_for(
			std::chrono::milliseconds(500)
		);
	}
}