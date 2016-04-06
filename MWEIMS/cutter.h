#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include "Limonp\Logger.hpp"
#include "Limonp\StringUtil.hpp"
#include "Unicode.h"

namespace cutter {
	typedef std::unordered_map<Unicode, double> trie_t;

	trie_t global_dict, global_weight;

	int init_dict(std::vector<std::string> filenames, trie_t *pdict = &global_dict) {
		LogInfo("Initing dict started.");
		auto &dict = *pdict;
		dict.clear();

		for (auto filename : filenames) {
			std::fstream fin(filename);
			if (!fin) {
				LogFatal("no such file %s", filename.c_str());
			}
			std::string word, tag;
			double freq;
			while (fin >> word >> freq >> tag) {
				dict[gbk2Unicode(word)] = freq;
			}
		}

		LogInfo("Initing dict finished.");
		return 0;
	}

	int init_weight(std::vector<std::string> filenames, trie_t *pweight = &global_weight) {
		LogInfo("Initing weight started.");
		auto &weight = *pweight;
		weight.clear();

		for (auto filename : filenames) {
			std::fstream fin(filename);
			if (!fin) {
				LogFatal("no such file %s", filename);
			}
			std::string word;
			double w;
			while (fin >> word >> w) {
				weight[gbk2Unicode(word)] = w;
			}
		}

		LogInfo("Initing weight finished.");
		return 0;
	}

	struct tuple {
		size_t u; const tuple* prev; double w;
		tuple(size_t _u, const tuple* _prev, double _w) :u(_u), prev(_prev), w(_w) {}
	};
	class cmp { public: bool operator () (const tuple*a, const tuple*b) { return a->w < b->w; } };
	int ksp(std::vector<std::vector<int> >& path, std::vector<std::vector<double> >& adjmat, int K) {
		path.resize(K);
		int N = adjmat.size();
		for_each(path.begin(), path.end(), [&](auto &x) {x.resize(N); });

		int k = 0;
		std::vector<tuple*> del_vec;
		std::priority_queue<tuple*, std::deque<tuple*>, cmp> pq;
		std::vector<double> dis;
		dis.resize(N);
		for_each(dis.begin(), dis.end(), [](auto &x) {x = -1000000000; });
		tuple* tmp = new tuple(0, NULL, 0);
		pq.push(tmp);
		del_vec.push_back(tmp);
		while (!pq.empty() && k < K) {
			const tuple *now = pq.top();
			size_t u = pq.top()->u;
			dis[u] = pq.top()->w;
			const tuple* prev = pq.top()->prev;
			pq.pop();
			if (u == N - 1) {
				LogDebug("Find a %d-path with weight of %lf", k, dis[u]);
				size_t t = N;
				while (prev && t != 0) {
					path[k][prev->u] = t;
					t = prev->u;
					prev = prev->prev;
				}
				k++;
				continue;
			}
			for (int nextPos = u + 1; nextPos < N; nextPos++) {
				double val = adjmat[u][nextPos];
				tuple *tmp = new tuple(nextPos, now, dis[u] + val);
				pq.push(tmp);
				del_vec.push_back(tmp);
			}
		}
		for (auto p : del_vec) {
			delete p;
		}
		return 0;
	}

	std::vector<std::vector<Unicode> > cut(Unicode sentence, int K = 5, trie_t *pdict = &global_dict) {
		K = min(K, int(pow(2, sentence.size() - 1)));
		auto &dict = *pdict;
		std::vector<std::vector<double> > adjmat;
		int N = sentence.size() + 1;
		double min_dict_value = 2000000000;
		adjmat.resize(N);
		for_each(adjmat.begin(), adjmat.end(), [&](auto &x) {x.resize(N); });
		for_each(dict.begin(), dict.end(), [&](auto x) {min_dict_value = min(min_dict_value, x.second); });
		for (int i = 0; i < N; i++) {
			for (int j = i + 1; j < N; j++) {
				Unicode unicode = sentence.substr(i, j - i);
				adjmat[i][j] = get(dict, unicode, min_dict_value) * (j - i == 1 ? 0.002 : 1);
			}
		}

		std::vector<std::vector<int> > path;
		std::vector<std::vector<Unicode> > vecWords;
		ksp(path, adjmat, K);
		vecWords.resize(K);
		for (int i = 0; i < K; i++) {
			int pos = 0;
			while (pos != N) {
				vecWords[i].push_back(sentence.substr(pos, path[i][pos] - pos));
				pos = path[i][pos];
			}
		}
		return vecWords;
	}

	std::vector<std::vector<std::string> > cut(std::string sentence, int K = 5, trie_t *pdict = &global_dict) {
		K = min(K, int(pow(2, gbk2Unicode(sentence).size() - 1)));
		std::vector<std::vector<std::string> > vecWords;
		std::vector<std::vector<Unicode> > vecUniWords = cut(gbk2Unicode(sentence), K, pdict);
		vecWords.resize(K);
		for (int i = 0; i < K; i++) {
			for_each(vecUniWords[i].begin(), vecUniWords[i].end(), [&](auto x) {
				vecWords[i].push_back(Unicode2gbk(x));
			});
		}
		return vecWords;
	}
}