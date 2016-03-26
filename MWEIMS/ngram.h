#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "Limonp\Logger.hpp"
#include "Limonp\StringUtil.hpp"
#include "Unicode.h"

typedef std::unordered_map<Unicode, long long> trie_t;
typedef std::unordered_map<Unicode, double> pmi_t;
typedef std::unordered_map<Unicode, double> ent_t;

trie_t global_unigram;
trie_t global_bigram;
trie_t global_trigram;
pmi_t global_pmi;
ent_t global_le, global_re, global_ent;

template<typename T>
double log2(T v) {
	return log(v) / log(2);
}

template<typename TDICT, typename TKEY, typename TVALUE>
TVALUE setdefault(TDICT dict, TKEY key, TVALUE value) {
	if (dict.find(key) != dict.end()) {
		value = dict[key];
	}
	return value;
}

std::string make_bigram(std::string s1, std::string s2, std::string delim = "¡ú") {
	return s1 + delim + s2;
}
Unicode make_bigram(Unicode s1, Unicode s2, Unicode delim = u"¡ú") {
	return s1 + delim + s2;
}
std::string make_trigram(std::string s1, std::string s2, std::string s3, std::string delim = "¡ú") {
	return s1 + delim + s2 + delim + s3;
}
Unicode make_trigram(Unicode s1, Unicode s2, Unicode s3, Unicode delim = u"¡ú") {
	return s1 + delim + s2 + delim + s3;
}

int init_ngram(std::vector<std::string> filenames, trie_t *punigram = &global_unigram,
				trie_t *pbigram = &global_bigram, trie_t *ptrigram = &global_trigram) {
	LogInfo("Initing ngram started.");
	auto &unigram = *punigram;
	auto &bigram = *pbigram;
	auto &trigram = *ptrigram;
	unigram.clear();
	bigram.clear();
	trigram.clear();

	for (auto filename : filenames) {
		std::string buff;
		std::fstream fin(filename);
		if (!fin) {
			LogFatal("no such file %s", filename);
		}
		while (std::getline(fin, buff)) {
			std::vector<std::string>  words;
			Limonp::split(buff, words, " ");
			std::string s1 = "";
			std::string s2 = "";
			for (auto word : words) {
				unigram[gbk2Unicode(word)]++;
				if (s2 != "") {
					bigram[gbk2Unicode(make_bigram(s2, word))]++;
					if (s1 != "") trigram[gbk2Unicode(make_trigram(s1, s2, word))]++;
				}
				s1 = s2;
				s2 = word;
			}
		}
	}

	LogInfo("Initing ngram finished.");
	return 0;
}

int init_pmi(pmi_t *ppmi = &global_pmi, trie_t *punigram = &global_unigram, trie_t *pbigram = &global_bigram) {
	LogInfo("Calculating pmi started.");
	auto &unigram = *punigram;
	auto &bigram = *pbigram;
	auto &pmi = *ppmi;
	pmi.clear();

	long long tot_uni = 0, tot_bi = 0;
	for_each(unigram.begin(), unigram.end(), [&](auto x) {tot_uni += x.second; });
	for_each(bigram.begin(), bigram.end(), [&](auto x) {tot_bi += x.second; });
	LogDebug("unigram tot freq: %lld, bigram tot freq: %lld.", tot_uni, tot_bi);
	//´úÂëÐ§ÂÊÌ«µÍ£¬·ÏÆú
	//for (auto u1 : unigram) {
	//	auto w1 = u1.first;
	//	for (auto u2 : unigram) {
	//		auto w2 = u2.first;
	//		if (setdefault(bigram, make_bigram(w1, w2), (long long)0) == 0) continue;
	//		pmi[make_bigram(w1, w2)] = log2((bigram[make_bigram(w1, w2)]*tot_uni*tot_uni)/double(u1.second*u2.second*tot_bi));
	//	}
	//}

	for (auto b : bigram) {
		auto w1 = b.first.substr(0, b.first.find(u'¡ú'));
		auto w2 = b.first.substr(b.first.find(u'¡ú')+1);
		pmi[b.first] = log2((bigram[b.first] * tot_uni*tot_uni) / double(unigram[w1]* unigram[w2] *tot_bi));
	}

	LogInfo("Calculating pmi finished.");
	return 0;
}

int init_ent(ent_t *ple = &global_le, ent_t *pre = &global_re, ent_t *pent = &global_ent,
			trie_t *punigram = &global_unigram, trie_t *pbigram = &global_bigram, trie_t *ptrigram = &global_trigram) {
	LogInfo("Calculating entropy started.");
	auto &unigram = *punigram;
	auto &bigram = *pbigram;
	auto &trigram = *ptrigram;
	auto &le = *ple;
	auto &re = *pre;
	auto &ent = *pent;
	le.clear();
	re.clear();
	ent.clear();

	long long tot_bi = 0, tot_tri = 0;
	for_each(bigram.begin(), bigram.end(), [&](auto x) {tot_bi += x.second; });
	for_each(trigram.begin(), trigram.end(), [&](auto x) {tot_tri += x.second; });
	LogDebug("bigram tot freq: %lld, trigram tot freq: %lld.", tot_bi, tot_tri);
	//´úÂëÐ§ÂÊÌ«µÍ£¬·ÏÆú
	//for (auto b : bigram) {
	//	auto wb = b.first;
	//	double wle = 0.0, wre = 0.0;
	//	if (setdefault(bigram, wb, (long long)0) == 0) continue;
	//	for (auto u : unigram) {
	//		auto wu = u.first;
	//		if (setdefault(trigram, make_bigram(wu, wb), (long long)0) != 0) {
	//			double lp = (double(trigram[make_bigram(wu, wb)]) / tot_tri) /
	//						(double(bigram[wb]) / tot_bi);
	//			wle += lp * log2(lp);
	//		}
	//		if (setdefault(trigram, make_bigram(wb, wu), (long long)0) != 0) {
	//			double rp = (double(trigram[make_bigram(wb, wu)]) / tot_tri) /
	//						(double(bigram[wb]) / tot_bi);
	//			wre += rp * log2(rp);
	//		}
	//	}
	//	le[wb] = wle;
	//	re[wb] = wre;
	//}
	for (auto tr : trigram) {
		auto lu = tr.first.substr(0, tr.first.find(u'¡ú'));
		auto rb = tr.first.substr(tr.first.find(u'¡ú') + 1);
		auto lb = tr.first.substr(0, tr.first.rfind(u'¡ú'));
		auto ru = tr.first.substr(tr.first.rfind(u'¡ú') + 1);
		double lp = (double(trigram[tr.first]) / tot_tri) / (double(bigram[rb]) / tot_bi);
		double rp = (double(trigram[tr.first]) / tot_tri) / (double(bigram[lb]) / tot_bi);
		le[rb] -= lp * log2(lp);
		re[lb] -= rp * log2(rp);
		ent[rb] -= lp * log2(lp);
		ent[lb] -= rp * log2(rp);
	}

	LogInfo("Calculating entropy finished.");
	return 0;
}