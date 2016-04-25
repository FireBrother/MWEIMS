#define _CRT_SECURE_NO_WARNINGS
#include "Unicode.h"
#include "ngram.h"
#include "cutter.h"
using namespace std;

enum debug_mode_t { debug_mode_bigram, debug_mode_dict};
void debug_shell(debug_mode_t debug_mode) {
	cout << "debug shell" << endl;
	switch (debug_mode) {
	case debug_mode_bigram:
		while (true) {
			string a, b;
			cin >> a >> b;
			printf("unigram %s: %lld\n", a.c_str(), global_unigram[gbk2Unicode(a)]);
			printf("unigram %s: %lld\n", b.c_str(), global_unigram[gbk2Unicode(b)]);
			printf("bigram %s: %lld\n", make_bigram(a, b).c_str(), global_bigram[gbk2Unicode(make_bigram(a, b))]);
			printf("pmi %s: %lf\n", make_bigram(a, b).c_str(), global_pmi[gbk2Unicode(make_bigram(a, b))]);
			printf("le %s: %lf\n", a.c_str(), global_le[gbk2Unicode(a)]);
			printf("le %s: %lf\n", b.c_str(), global_le[gbk2Unicode(b)]);
			printf("re %s: %lf\n", a.c_str(), global_re[gbk2Unicode(a)]);
			printf("re %s: %lf\n", b.c_str(), global_re[gbk2Unicode(b)]);
			printf("le %s: %lf\n", make_bigram(a, b).c_str(), global_le[gbk2Unicode(make_bigram(a, b))]);
			printf("re %s: %lf\n\n", make_bigram(a, b).c_str(), global_re[gbk2Unicode(make_bigram(a, b))]);
		}
	case debug_mode_dict:
		while (true) {
			string s;
			cin >> s;
			printf("dict %s: %lf\n", s.c_str(), cutter::global_dict[gbk2Unicode(s)]);
			printf("weight %s: %lf\n", s.c_str(), cutter::global_weight[gbk2Unicode(s)]);
		}
	}
}

template<typename TDICT>
void save(string filename, const TDICT &dict,long long thresh = 5) {
	LogInfo("Saving %s started.", filename.c_str());
	ofstream fout;
	fout.open(filename, ios::out);
	vector<pair<Unicode, double> > vec(dict.begin(), dict.end());
	sort(vec.begin(), vec.end(), [](auto x, auto y) { return x.second > y.second; });
	for_each(vec.begin(), vec.end(), [&](auto x) {
		if (get(global_bigram, x.first, (long long)0) > thresh) {
			auto lu = x.first.substr(0, x.first.find(u'→'));
			auto ru = x.first.substr(x.first.find(u'→') + 1);
			fout << x.first << '\t' << x.second << endl;
		}
	});
	fout.close();
	LogInfo("Saving %s finished.", filename.c_str());
}

void calc_statistic() {
	init_ngram({ "data\\PeopleDaily_seg.txt" });
	cout << experiment::dict.size() << endl;
	init_pmi();
	init_ent();
	ent_t ent, diff_le, diff_re;
	double min_le = 2000000000, min_re = 2000000000;
	for_each(global_le.begin(), global_le.end(), [&](auto x) {min_le = min(min_le, x.second); });
	for_each(global_re.begin(), global_re.end(), [&](auto x) {min_re = min(min_re, x.second); });
	for_each(global_le.begin(), global_le.end(), [&](auto x) {
		ent[x.first] = global_le[x.first] + get(global_re, x.first, min_re);
	});
	for_each(global_re.begin(), global_re.end(), [&](auto x) {
		ent[x.first] = global_re[x.first] + get(global_le, x.first, min_le);
	});
	for_each(global_le.begin(), global_le.end(), [&](auto x) {
		if (get(global_bigram, x.first, (long long)0) > 5) {
			auto lu = x.first.substr(0, x.first.find(u'→'));
			diff_le[x.first] = global_le[x.first] - global_le[lu];
		}
	});
	for_each(global_re.begin(), global_re.end(), [&](auto x) {
		if (get(global_bigram, x.first, (long long)0) > 5) {
			auto ru = x.first.substr(x.first.find(u'→') + 1);
			diff_re[x.first] = global_re[x.first] - global_re[ru];
		}
	});
	ent_t weight;
	for_each(global_bigram.begin(), global_bigram.end(), [&](auto x) {
		if (get(global_bigram, x.first, (long long)0) > 5) {
			// weight[x.first] = double(2 * ent[x.first] * global_pmi[x.first]) / double(ent[x.first] + global_pmi[x.first]);
			weight[x.first] = double(ent[x.first] + global_pmi[x.first]) / 2.0;
		}
	});

	//save("result\\pmi.txt", global_pmi);
	//save("result\\le.txt", global_le);
	//save("result\\re.txt", global_re);
	//save("result\\ent.txt", ent);
	//save("result\\diff_le.txt", diff_le);
	//save("result\\diff_re.txt", diff_re);
	//save("result\\weight.txt", weight);

	string pmis[] = { "pmi_exact", "pmi_high", "pmi_laohu", "pmi_shy1", "pmi_shy2" };
	save("experiment\\pmi_exact.txt", experiment::pmi_exact);
	save("experiment\\pmi_high.txt", experiment::pmi_high);
	save("experiment\\pmi_laohu.txt", experiment::pmi_laohu);
	save("experiment\\pmi_shy1.txt", experiment::pmi_shy1);
	save("experiment\\pmi_shy2.txt", experiment::pmi_shy2);

	save("experiment\\dict.txt", experiment::dict, -1);
	
}

int main() {
	 calc_statistic();
	//cutter::init_dict({ "data\\jieba.dict" });
	//cutter::init_weight({ "result\\weight.txt" });
	//string sentence = "继续推进经济体制和经济增长方式的根本转变，坚定不移地沿着有中国特色社会主义道路前进";
	////while (getline(cin, sentence)) 
	//{
	////if (sentence == "EXIT") break;
	//	for (auto a : cutter::cut(sentence)) {
	//		cout << Limonp::join(a.begin(), a.end(), " ") << endl;
	//		for (auto w : a)
	//			if (get(cutter::global_weight, gbk2Unicode(w), -10.0) != -10.0)
	//				cout << w << endl;
	//	}
	//}
	debug_shell(debug_mode_dict);
}