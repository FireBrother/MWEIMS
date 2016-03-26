#define _CRT_SECURE_NO_WARNINGS
#include "Unicode.h"
#include "ngram.h"
using namespace std;

enum debug_mode_t { debug_mode_pmi };
void debug_shell(debug_mode_t debug_mode) {
	cout << "debug shell" << endl;
	switch (debug_mode) {
	case debug_mode_pmi:
		while (true) {
			string a, b;
			cin >> a >> b;
			printf("unigram %s: %lld\n", a.c_str(), global_unigram[gbk2Unicode(a)]);
			printf("unigram %s: %lld\n", b.c_str(), global_unigram[gbk2Unicode(b)]);
			printf("bigram %s: %lld\n", make_bigram(a, b).c_str(), global_bigram[gbk2Unicode(make_bigram(a, b))]);
			printf("pmi %s: %lf\n\n", make_bigram(a, b).c_str(), global_pmi[gbk2Unicode(make_bigram(a, b))]);
		}
	}
	
}

template<typename TDICT>
void save(string filename, const TDICT &dict) {
	LogInfo("Saving %s started.", filename.c_str());
	ofstream fout;
	fout.open(filename, ios::out);
	vector<pair<Unicode, double> > vec(dict.begin(), dict.end());
	sort(vec.begin(), vec.end(), [](auto x, auto y) { return x.second > y.second; });
	for_each(vec.begin(), vec.end(), [&](auto x) {
		if (global_bigram[x.first] > 5)
			fout << x.first << '\t' << x.second << '\t' << global_bigram[x.first] << endl;
	});
	fout.close();
	LogInfo("Saving %s finished.", filename.c_str());
}

int main() {
	init_ngram({"data\\PeopleDaily_seg.txt"});
	init_pmi();

	ofstream fout;

	init_ent();

	save("result\\pmi.txt", global_pmi);
	save("result\\le.txt", global_le);
	save("result\\re.txt", global_re);
	save("result\\ent.txt", global_ent);

}