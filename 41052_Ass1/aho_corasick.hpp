#pragma once

#include <string>
#include <vector>
#include <map>

// Represents one detection of a pattern
struct Match {
	int patternID;
	std::size_t position;
};

class AhoCorasick {
private:
	// node = state
	struct Node {
		// transitions
		std::map<char, int> goTo;
	};

	// Data structures representing three functions - goto(), failure(), output()
	// (index = state)
	std::vector<Node> nodes;
	std::vector<int> failure;
	std::vector<std::vector<int>> output;

	// Store every patterns in vector (index = patternID)
	std::vector<std::string> patterns;

public:
	AhoCorasick();

	void addPattern(const std::string& pattern);
	void buildFailureLinks();
	std::vector<Match> search(const std::string& text) const;

	const std::string& getPattern(int patternID) const;
};