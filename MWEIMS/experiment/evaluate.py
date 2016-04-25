# coding=utf8

import codecs

pmi_types = ["pmi_exact", "pmi_high", "pmi_laohu", "pmi_shy1", "pmi_shy2"]
files = {}
pmis = {}
dic = set()
for pt in pmi_types:
    files[pt] = codecs.open(pt+'.txt', 'r', 'gbk')
    pmis[pt] = {}
    for i, line in enumerate(files[pt].readlines()):
        line = line.strip()
        w, v = line.split()
        pmis[pt][w] = float(v)

for line in codecs.open('dict.txt', 'r', 'gbk')
    line = line.strip()
        w, v = line.split()
        dic.add(w)

for w in sorted(pmi_high.iteritems(), key = lambda x: -x[1]):
    if w[0] not in pmi_new.keys():
        print >>of2, w[0], w[1]
