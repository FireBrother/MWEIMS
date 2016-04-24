# coding=utf8

import codecs

f1 = codecs.open('pmi_exact.txt', 'r', 'gbk')
f2 = codecs.open('pmi_low.txt', 'r', 'gbk')
f3 = codecs.open('pmi_high.txt', 'r', 'gbk')
f4 = codecs.open('pmi_new.txt', 'r', 'gbk')

pmi_exact = {}
pmi_low = {}
pmi_high = {}
pmi_new = {}

pmi_ave = {}

pair = [(f1, pmi_exact), (f2, pmi_low), (f3, pmi_high), (f4, pmi_new)]

for p in pair:
    for i, line in enumerate(p[0].readlines()):
        if i > 10000:
            break
        line = line.strip()
        w, v = line.split()
        p[1][w] = float(v)

of1 = codecs.open('unique_new.txt', 'w', 'utf8')
of2 = codecs.open('unique_high.txt', 'w', 'utf8')

for w in sorted(pmi_new.iteritems(), key = lambda x: -x[1]):
    if w[0] not in pmi_high.keys():
        print >>of1, w[0], w[1]

for w in sorted(pmi_high.iteritems(), key = lambda x: -x[1]):
    if w[0] not in pmi_new.keys():
        print >>of2, w[0], w[1]
