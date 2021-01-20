#include <foundation/include/regex/regex.h>

#include <foundation/src/regex/regex2/re2.h>

namespace ws {

namespace regex {

bool match(
	const ustring& pattern, 
	const ustring& in, 
	bool fullMatch /*= false*/) {
	re2::RE2 p(pattern);
	assert(p.ok() && "incorrect regular expression");
	return fullMatch ? 
		re2::RE2::FullMatch(in, p) : 
		re2::RE2::PartialMatch(in, p);
}

bool match(
	const ustring& pattern,
	const ustring& source,
	std::vector<ustring>& subMatchs,
	bool fullMatch /*= false*/) {
	re2::RE2 p(pattern);
	assert(p.ok() && "incorrect regular expression");

	auto count = p.NumberOfCapturingGroups() + 1;
	auto matchs = new re2::StringPiece[count];
	auto anchor = fullMatch ? RE2::ANCHOR_BOTH : RE2::UNANCHORED;
	if(!p.Match(source, 0, source.size(), anchor, matchs, count)) {
		delete[] matchs;
		return false;
	}

	for(auto i = 1; i < count; ++i) {
		if(0 != matchs[i].size()) {
			subMatchs.push_back(matchs[i].ToString());
		}
	}

	delete[] matchs;
	return true;
}


}

}