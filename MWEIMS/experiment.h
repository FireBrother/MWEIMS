#pragma once
#include "Unicode.h"

namespace experiment {
	typedef std::unordered_map<Unicode, double> pmi_t;
	typedef std::unordered_map<Unicode, long long> trie_t;
	pmi_t pmi_exact;
	pmi_t pmi_high;
	pmi_t pmi_shy2;
	pmi_t pmi_laohu;
	pmi_t pmi_shy1;

	trie_t dict;
}