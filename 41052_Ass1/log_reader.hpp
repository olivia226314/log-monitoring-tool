#pragma once

#include "aho_corasick.hpp"
#include <string>
#include <vector>

struct LogMatch {
	std::size_t lineNumber;
	Match match;
};

class LogReader {
public:
	std::vector<LogMatch> scanFile(
		const std::string& filePath,
		const AhoCorasick& automaton
	) const;

	void followFile(
		const std::string& filePath,
		const AhoCorasick& automaton, 
		bool includeExisting
	) const;
};