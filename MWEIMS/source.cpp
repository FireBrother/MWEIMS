#define _CRT_SECURE_NO_WARNINGS
#include "Unicode.h"
#include "ngram.h"
using namespace std;

int main() {
	Unicode u = u"≤‚ ‘";
	string s = "≤‚ ‘";
	cout << make_bigram(s, s) << endl;
}