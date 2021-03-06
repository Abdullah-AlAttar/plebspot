#include "util.h"

#include <fmt/core.h>

using namespace std;

namespace util{

string trim(const string &s){
	string::const_iterator it = s.begin();
	while (it != s.end() && isspace(*it))
		it++;

	string::const_reverse_iterator rit = s.rbegin();
	while (rit.base() != it && isspace(*rit))
		rit++;

	return string(it, rit.base());
}

map<string, string> parse_pairs(const string& kv_pairs){
	map<string, string> output;
	size_t line_start_idx = 0;
	size_t line_end_idx = 0;//kv_pairs.npos;
	size_t colon_idx = 0;//kv_pairs.npos;
	while( line_end_idx+2 < kv_pairs.size() ) {
		line_start_idx = line_end_idx == 0 ? line_end_idx : line_end_idx+1;
		colon_idx = kv_pairs.find(':', line_start_idx);
		line_end_idx = kv_pairs.find('\n', colon_idx);
		if(line_end_idx == kv_pairs.npos){
			line_end_idx = kv_pairs.size();
		}
		string key = kv_pairs.substr(line_start_idx, colon_idx-line_start_idx);
		string value = kv_pairs.substr(colon_idx+1, line_end_idx-colon_idx);
		output[trim(key)] = trim(value);
	}
	return output;
}

}