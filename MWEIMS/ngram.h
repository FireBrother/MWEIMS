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

trie_t global_unigram;
trie_t global_bigram;
trie_t global_trigram;
pmi_t global_pmi;

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
					if (s1 != "") bigram[gbk2Unicode(make_trigram(s1, s2, word))]++;
				}
				s1 = s2;
				s2 = word;
			}
		}
	}

	LogInfo("Initing ngram finished.");
	return 0;
}

int init_pmi(trie_t *punigram = &global_unigram, trie_t *pbigram = &global_bigram, pmi_t *ppmi = &global_pmi) {
	LogInfo("Calculating ngram started.");
	auto &unigram = *punigram;
	auto &bigram = *pbigram;
	auto &pmi = *ppmi;
	pmi.clear();

	long long tot = 0;
	for_each(unigram.begin(), unigram.end(), [&](auto x) {tot += x.second; });
	for (auto u1 : unigram) {
		for (auto u2 : unigram) {
			auto w1 = u1.first;
			auto w2 = u2.first;
			pmi[make_bigram(w1, w2)] = log((tot*bigram[make_bigram(w1, w2)])/double(u1.second*u2.second)) / log(2);
		}
	}

	LogInfo("Calculating ngram finished.");
	return 0;
}