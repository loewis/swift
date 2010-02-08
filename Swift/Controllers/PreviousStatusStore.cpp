#include "PreviousStatusStore.h"

#include "Swiften/Base/foreach.h"

namespace Swift {

PreviousStatusStore::PreviousStatusStore() {

}

PreviousStatusStore::~PreviousStatusStore() {

}

void PreviousStatusStore::addStatus(StatusShow::Type status, const String& message) {
	//FIXME: remove old entries
	store_.push_back(TypeStringPair(status, message));
}

std::vector<TypeStringPair> PreviousStatusStore::exactMatchSuggestions(StatusShow::Type status, const String& message) {
	std::vector<TypeStringPair> suggestions;
	suggestions.push_back(TypeStringPair(status, message));
	return suggestions;
}

std::vector<TypeStringPair> PreviousStatusStore::getSuggestions(const String& message) {
	std::vector<TypeStringPair> suggestions;
	foreach (TypeStringPair status, store_) {
		if (status.second == message) {
			suggestions.clear();
			suggestions.push_back(status);
			break;
		} else if (status.second.contains(message)) {
			suggestions.push_back(status);
		}
	}
	if (suggestions.size() == 0) {
		TypeStringPair suggestion(StatusShow::Online, message);
		suggestions.push_back(suggestion);
	}
	if (suggestions.size() == 1) {
		suggestions = exactMatchSuggestions(suggestions[0].first, suggestions[0].second);
	}
	return suggestions;
}

}