#include "aho_corasick.hpp"
#include <queue>
#include <algorithm>

// Creat root state
AhoCorasick::AhoCorasick() {
	nodes.push_back(Node{});
	failure.push_back(0);
	output.push_back({});
}

// Add pattern to the trie
void AhoCorasick::addPattern(const std::string& pattern) {
	if (pattern.empty()) {
		return;
	}

	// Ignore duplicate patterns
	auto it = std::find(patterns.begin(), patterns.end(), pattern);
	if (it != patterns.end()) {
		return;
	}
	
	patterns.push_back(pattern);


	int currentState = 0;
	std::size_t index = 0;
	
	// Follow transitions already presnt in the trie
	while (
		index < pattern.size() &&
		nodes[currentState].goTo.contains(pattern[index])
	) {
		currentState = nodes[currentState].goTo.at(pattern[index]);
		index++;
	}
	// Create states from the remaining characters
	for (std::size_t i = index; i < pattern.size(); i++) {
		int newState = static_cast<int>(nodes.size());
		
		nodes.push_back(Node{});
		failure.push_back(0);
		output.push_back({});
		nodes[currentState].goTo.insert({ pattern[i], newState });
		currentState = newState;
	}
	// Record the pattern at its ending state
	output[currentState].push_back(static_cast<int>(patterns.size()-1));
}

// Build failure links & complete output lists
void AhoCorasick::buildFailureLinks() {
	std::queue<int> q;

	// States one edge from the root fail back to the root
	for (auto const& [key, val] : nodes[0].goTo) {
		q.push(val);
		failure.at(val) = 0;
	}

	while (!q.empty()) {
		int r = q.front();
		q.pop();

		for (auto const& [key, val] : nodes[r].goTo) {
			q.push(val);
			// Follow failure links until 
			// - a transition for key is available
			// - search reaches the root
			int state = failure[r];
			while (
				state != 0 &&
				nodes[state].goTo.find(key) == nodes[state].goTo.end()
			) {
				state = failure[state];
			}

			// Use the matching transition, or fail back to the root
			auto it = nodes[state].goTo.find(key);
			if (it != nodes[state].goTo.end()) {
				failure[val] = it->second;
			}
			else {
				failure[val] = 0;
			}
			
			// Include patterns that end at the failure state
			int failureState = failure[val];
			output[val].insert(
				output[val].end(),
				output[failureState].begin(),
				output[failureState].end()
			);
		}
	}
}

// Find all pattern matches in the text
std::vector<Match> AhoCorasick::search(const std::string& text) const {
	std::vector<Match> matches;
	int state = 0;

	for (size_t i = 0; i < text.size(); i++) {
		char c = text[i];

		
		// Follow failure links if the current state has no transition for c
		while (
			state != 0 &&
			nodes[state].goTo.find(c) == nodes[state].goTo.end()
		) {
			state = failure[state];
		}

		// Take the transition if one exists; otherwise remain at root
		auto it = nodes[state].goTo.find(c);
		if (it != nodes[state].goTo.end()) {
			state = it->second;
		}
		

		// Record every pattern ending at this position
		for (int patternID : output[state]) {
			size_t patternLength = patterns[patternID].size();
			size_t startPosition = i - patternLength + 1;

			matches.push_back(
				{ patternID, startPosition }
			);
		}
	}
	return matches;
}

const std::string& AhoCorasick::getPattern(int patternID) const {
	return patterns.at(patternID);
}