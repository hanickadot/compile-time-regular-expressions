#ifndef CTFA__BASIC__DEBUG__HPP
#define CTFA__BASIC__DEBUG__HPP

#include "fa.hpp"

#include <string>
#include <map>
#include <set>
#include <sstream>
#include <tuple>
#include <iostream>


namespace ctfa {

template <const auto & Fa, typename TransitionCB, typename FinalCB> void debug(TransitionCB && tran, FinalCB && final) {
	for (const auto & t: Fa.transitions) {
		tran(t.source, t.target, t.cond, Fa.is_final(t.source));
	}
	for (const auto & f: Fa.final_states) {
		if (Fa.transitions.find(f) == Fa.transitions.end()) final(f);
	}
}

template <const auto & Fa, typename WriteCB> void print_json(WriteCB && cb) {
	cb("var diagram = {\n");
	cb("\"start\":\"0\",\n");
	
	//{"name":"_special_","invisible":true,"id":0}
	//,{"name":"A","final":false,"id":1,"start":true}
	//,{"name":"B","final":false,"id":2}
	//,{"name":"C","final":false,"id":3}
	//,{"name":"D","final":true,"id":4}
	//,{"name":"E","final":true,"id":5}
	//,{"name":"F","final":true,"id":6}
	//,{"name":"G","final":false,"id":7}
	//,{"name":"H","final":false,"id":8}
	//,{"name":"dec","repeat":true,"id":9,"weight":0}
	//,{"name":"dec","repeat":true,"id":10,"weight":0}
	//,{"name":"dec","repeat":true,"id":11,"weight":0}
	
	//"nodes":[{"name": "_special_", "invisible": true, "id":0}
	//,{"name": "0", "final":false, "id": 1, "start": true}
	//,{"name": "1", "final":false, "id": 2, }
	//,{"name": "2", "final":false, "id": 3, }
	//,{"name": "3", "final":true, "id": 4, }
	//,{"name": "4", "final":false, "id": 5, }
	//,{"name": "5", "final":false, "id": 6, }
	//,{"name": "6", "final":false, "id": 7, }
	//,{"name": "7", "final":false, "id": 8, }
	//,{"name": "8", "final":false, "id": 9, }
	//,{"name": "9", "final":true, "id": 10, }
	//,{"name":"'a'-'z'", "repeat": true, "id": 14, "weight": 1}], 
	
	std::map<ctfa::state, bool> known_state;
	for (const auto & t: Fa.transitions) {
		known_state.insert({t.source, false});
		known_state.insert({t.target, false});
	}
	for (state s: Fa.final_states) {
		auto pair = known_state.insert({s, true});
		if (pair.first != known_state.end()) {
			pair.first->second = true;
		}
	}
	
	cb("\"nodes\":[");
	
	cb("{\"name\": \"_special_\", \"invisible\": true, \"id\":0}");
	
	for (const auto & s: known_state) {
		cb(",{\"name\": \"",s.first.id,"\", \"final\":",(s.second?"true":"false"),", \"id\": ",(s.first.id+1),", ",(s.first==start_state ? "\"start\": true":"") ,"}");
	}
	
	
	auto condition_debug = [&](const impl::range & rng, auto && cb) {
		if (rng.low == std::numeric_limits<char32_t>::min()) return;
		if (rng.high == std::numeric_limits<char32_t>::max()) return;
		
		if (rng.low != rng.high) return;
		
		if (rng.low == std::numeric_limits<char32_t>::min() && rng.high == std::numeric_limits<char32_t>::max()) {
			cb("*");
			return;
		}
	
		if (rng.low == std::numeric_limits<char32_t>::min()); //cb("min");
		else if (rng.low == '"') cb("\\\"");
		else if (rng.low >= 32 && rng.low <= 126) cb(static_cast<char>(rng.low));
		else cb(rng.low);
		
		if (rng.low != rng.high) {
			cb("..");
			if (rng.high == std::numeric_limits<char32_t>::max());// cb("max");
			else if (rng.high == '"') cb("\\\"");
			else if (rng.high >= 32 && rng.high <= 126) cb(static_cast<char>(rng.high));
			else cb(rng.high);
		}
	};
	
	int counter = static_cast<int>(known_state.size()+1);
	for (const auto & t: Fa.transitions) {
		if (t.source == t.target) {
			//{"name":"dec","repeat":true,"id":11,"weight":0}
			cb(",{\"name\":\"");
			
			condition_debug(t.cond.r, cb);
			
			cb("\", \"repeat\": true, \"id\": ",counter,", \"weight\": 1}");
			counter++;
		}
		
	}
	
	cb("], \n");
	
	cb("\"links\":[");
	
	//"links":[{"source":0,"target":1,"start":true},
	//{"source":1,"target":4,"term":"dec","ident":false,"replace":"","multi":0,"simple":false,"id":"A_D"},
	//{"source":1,"target":2,"term":"+-","ident":false,"replace":"","multi":0,"simple":false,"id":"A_B"},
	//{"source":2,"target":4,"term":"dec","ident":false,"replace":"","multi":0,"simple":false,"id":"B_D"},
	//{"source":3,"target":6,"term":"dec","ident":false,"replace":"","multi":0,"simple":false,"id":"C_F"},
	//{"source":3,"target":8,"term":"+-","ident":false,"replace":"","multi":0,"simple":false,"id":"C_H"},
	//{"source":4,"target":7,"term":".","ident":false,"replace":"","multi":0,"simple":false,"id":"D_G"},
	//{"source":4,"target":3,"term":"e","ident":false,"replace":"","multi":0,"simple":false,"id":"D_C"},
	//{"source":4,"target":9,"term":"dec","ident":false,"replace":"","multi":0,"simple":true,"id":"D_D"},
	//{"source":5,"target":3,"term":"e","ident":false,"replace":"","multi":0,"simple":false,"id":"E_C"},
	//{"source":5,"target":10,"term":"dec","ident":false,"replace":"","multi":0,"simple":true,"id":"E_E"},
	//{"source":6,"target":11,"term":"dec","ident":false,"replace":"","multi":0,"simple":true,"id":"F_F"},
	//{"source":7,"target":5,"term":"dec","ident":false,"replace":"","multi":0,"simple":false,"id":"G_E"},
	//{"source":8,"target":6,"term":"dec","ident":false,"replace":"","multi":0,"simple":false,"id":"H_F"}]}
	
	
	//"links":[{"source": 0, "target": 1, "start": true},
	//{"source": 1, "target": 6, "simple": false, "term": "'a'"},
	//{"source": 1, "target": 4, "simple": false, "term": "'a'-'z'"},
	//{"source": 2, "target": 4, "simple": false, "term": "'a'-'z'"},
	//{"source": 3, "target": 4, "simple": false, "term": "'a'-'z'"},
	//{"source": 4, "target": 14, "term": "yyy", "simple": true},
	//{"source": 5, "target": 6, "simple": false, "term": "'a'"},
	//{"source": 6, "target": 7, "simple": false, "term": "'l'"},
	//{"source": 7, "target": 8, "simple": false, "term": "'o'"},
	//{"source": 8, "target": 9, "simple": false, "term": "'h'"},
	//{"source": 9, "target": 10, "simple": false, "term": "'a'"}]}
	cb("{\"source\": 0, \"target\": 1, \"start\": true}");
	
	counter = static_cast<int>(known_state.size()+1);
	
	std::set<std::pair<state, state>> transitions;
	std::map<std::tuple<state, state, std::string>, bool> duplicit;
	
	for (const auto & t: Fa.transitions) {
		if (t.source != t.target) transitions.insert({t.source, t.target});
		std::ostringstream ss;
		condition_debug(t.cond.r, [&](auto && ... in){
			(ss << ... << in);
		});
		duplicit.insert({{t.source, t.target, std::move(ss).str()}, true});
	}
	
	for (const auto & t: Fa.transitions) {
		bool multi = (t.target !=  t.source) && (transitions.find(std::pair<state, state>{t.target, t.source}) != transitions.end());
		
		std::ostringstream ss;
		condition_debug(t.cond.r, [&](auto && ... in){
			(ss << ... << in);
		});
		std::string term = std::move(ss).str();
		
		auto it = duplicit.find(std::tuple{t.source, t.target, term});
		if (it != duplicit.end()) {
			if (it->second) {
				it->second = false;
			} else {
				std::cerr << "\nskip: " << t.source.id << " -> " << t.target.id << "\n";
				continue;
			}
		}
		
		
		if (t.source == t.target) {
			// {"source":4,"target":9,"term":"dec","ident":false,"replace":"","multi":0,"simple":true,"id":"D_D"},
			cb(",{\"source\": ",t.source.id+1,", \"target\": ",counter,", \"simple\": true, \"term\": \"",term,"\"}");
			counter++;
		} else {
			cb(",{\"source\": ",t.source.id+1,", \"target\": ",t.target.id+1,", \"simple\": false, \"term\": \"",term,"\"");
			if (multi) {
				cb(", \"multi\": 1}");
			} else {
				cb("}");
			}
		}
		
		
	}
	
	cb("]");
	
	cb("};\n");
	
}


}

#endif
