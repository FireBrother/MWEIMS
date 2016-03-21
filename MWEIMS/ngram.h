#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "Limonp\Logger.hpp"
#include "Limonp\StringUtil.hpp"

typedef std::unordered_map<std::string, long long> unigram_t;
typedef std::unordered_map<std::string, std::unordered_map<std::string, long long> > bigram_t;
typedef std::unordered_map<std::string, std::unordered_map<std::string, double> > pmi_t;

unigram_t global_unigram;
bigram_t global_bigram;
pmi_t global_pmi;

int init_ngram(std::vector<std::string> filenames, unigram_t *punigram = &global_unigram, bigram_t *pbigram = &global_bigram) {
	LogInfo("Initing ngram started.");
	auto &unigram = *punigram;
	auto &bigram = *pbigram;
	unigram.clear();
	bigram.clear();

	for (auto filename : filenames) {
		std::string buff;
		std::fstream fin(filename);
		if (!fin) {
			LogFatal("no such file %s", filename);
		}
		while (std::getline(fin, buff)) {
			std::vector<std::string>  words;
			Limonp::split(buff, words, " ");
			std::string prev = "";
			for (auto word : words) {
				unigram[word]++;
				if (prev != "") bigram[prev][word]++;
				prev = word;
			}
		}
	}

	LogInfo("Initing ngram finished.");
	return 0;
}

int init_pmi(unigram_t *punigram = &global_unigram, bigram_t *pbigram = &global_bigram, pmi_t *ppmi = &global_pmi) {
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
			pmi[w1][w2] = log((tot*bigram[w1][w2])/u1.second*u2.second) / log(2);
		}
	}

	LogInfo("Calculating ngram finished.");
	return 0;
}