#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "Limonp\Logger.hpp"
#include "Limonp\StringUtil.hpp"
#include "Unicode.h"

#include "experiment.h"

typedef std::unordered_map<Unicode, long long> trie_t;
typedef std::unordered_map<Unicode, double> pmi_t;
typedef std::unordered_map<Unicode, double> ent_t;

trie_t global_unigram;
trie_t global_bigram;
trie_t global_trigram;
pmi_t global_pmi;
ent_t global_le, global_re, global_ent;

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
			LogFatal("no such file %s", filename.c_str());
		}
		while (std::getline(fin, buff)) {
			std::vector<std::string>  words;
			Limonp::split(buff, words, " ");
			std::string s1 = "";
			std::string s2 = "";
			for (auto word : words) {
				if (gbk2Unicode(word).length() == 2) {
					experiment::dict[gbk2Unicode(word)]++;
				}
				else if (gbk2Unicode(word).length() != 1) {
					s1 = s2 = "";
					continue;
				}
				if (word == "¡ú") continue;
				if (is_P(word)) {
					s1 = s2 = "";
					continue;
				}
				Unicode uni = gbk2Unicode(word);
				for (auto c : uni) {
					Unicode unic(1, c);
					std::string cs = Unicode2gbk(unic);
					unigram[gbk2Unicode(cs)]++;
					if (s2 != "") {
						bigram[gbk2Unicode(make_bigram(s2, cs))]++;
						if (s1 != "") trigram[gbk2Unicode(make_trigram(s1, s2, cs))]++;
					}
					s1 = s2;
					s2 = cs;
				}
				//unigram[gbk2Unicode(word)]++;
				//if (s2 != "") {
				//	bigram[gbk2Unicode(make_bigram(s2, word))]++;
				//	if (s1 != "") trigram[gbk2Unicode(make_trigram(s1, s2, word))]++;
				//}
				//s1 = s2;
				//s2 = word;
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
		pmi[b.first] = log2((bigram[b.first] * tot_uni*tot_uni) / double(unigram[w1] * unigram[w2] * tot_bi));
		// pmi[b.first] = log2((bigram[b.first] * tot_uni*tot_uni * 0.8) / double(unigram[w1] * unigram[w2] * min(unigram[w1], unigram[w2])));
		// pmi[b.first] = log2(bigram[b.first]*(unigram[w1]+unigram[w2])/double(unigram[w1]* unigram[w1]* unigram[w2]* unigram[w2]));
		if (b.first.length() == 3) {
			experiment::pmi_exact[b.first] = log2((bigram[b.first] * tot_uni*tot_uni) / double(unigram[w1] * unigram[w2] * tot_bi));
			experiment::pmi_high[b.first] = log2((bigram[b.first] * tot_uni*tot_uni * 0.8) / double(unigram[w1] * unigram[w2] * max(unigram[w1], unigram[w2])));
			experiment::pmi_laohu[b.first] = log2(bigram[b.first] * (unigram[w1] + unigram[w2]) / double(unigram[w1] * unigram[w2]) *tot_uni*tot_uni /double(unigram[w1] * unigram[w2]));
			if (isnan(experiment::pmi_laohu[b.first])) {
				LogFatal("%s, %lld, %lld, %lld", Unicode2gbk(b.first).c_str(), bigram[b.first], unigram[w1], unigram[w2]);
			}
			experiment::pmi_shy1[b.first] = experiment::pmi_exact[b.first] - log2(unigram[w1]* unigram[w2]/double((unigram[w1]+ unigram[w2])*tot_uni));
			experiment::pmi_shy2[b.first] = experiment::pmi_exact[b.first] - log2(2 * unigram[w1] * unigram[w2] / double((unigram[w1] + unigram[w2])*tot_uni));
		}
	}

	LogInfo("Calculating pmi finished.");
	return 0;
}

int init_ent(ent_t *ple = &global_le, ent_t *pre = &global_re, trie_t *punigram = &global_unigram,
			trie_t *pbigram = &global_bigram, trie_t *ptrigram = &global_trigram) {
	LogInfo("Calculating entropy started.");
	auto &unigram = *punigram;
	auto &bigram = *pbigram;
	auto &trigram = *ptrigram;
	auto &le = *ple;
	auto &re = *pre;
	le.clear();
	re.clear();

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
		double lp = double(trigram[tr.first]) / double(bigram[rb]);
		double rp = double(trigram[tr.first]) / double(bigram[lb]);
		le[rb] -= lp * log2(lp);
		re[lb] -= rp * log2(rp);
	}

	for (auto b : bigram) {
		auto lu = b.first.substr(0, b.first.find(u'¡ú'));
		auto ru = b.first.substr(b.first.find(u'¡ú') + 1);
		double lp = double(bigram[b.first]) / double(unigram[ru]);
		double rp = double(bigram[b.first]) / double(unigram[lu]);
		le[ru] -= lp * log2(lp);
		re[lu] -= rp * log2(rp);
	}

	LogInfo("Calculating entropy finished.");
	return 0;
}