# coding=utf8

import codecs

pmi_types = ["pmi_exact", "pmi_high", "pmi_laohu", "pmi_shy1", "pmi_shy2"]
files = {}
error_files = {}
pmis = {}
dic = set()
for pt in pmi_types:
    files[pt] = codecs.open(pt+'.txt', 'r', 'gbk')
    error_files[pt] = codecs.open('error_'+pt+'.txt', 'w', 'utf8')
    pmis[pt] = {}
    for i, line in enumerate(files[pt].readlines()):
        line = line.strip()
        w, v = line.split()
        w = ''.join(w.split('→'.decode('utf8')))
        pmis[pt][w] = float(v)

for line in codecs.open('dict.txt', 'r', 'gbk'):
    line = line.strip()
    w, v = line.split()
    dic.add(w)

N = len(dic)
correct = {}
error = {}
precisoin = {}
recall = {}
fmeasure = {}

topKs = [1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000]

for pt in pmi_types:
    correct[pt] = {}
    error[pt] = {}
    precisoin[pt] = {}
    recall[pt] = {}
    fmeasure[pt] = {}
    for i, pair in enumerate(sorted(pmis[pt].iteritems(), key=lambda x: -x[1])):
        w, pmi = pair
        if w in dic:
            for topK in topKs:
                if i < topK:
                    v = correct[pt].get(topK, 0)
                    correct[pt][topK] = v + 1
        else:
            for topK in topKs:
                if i < topK:
                    v = error[pt].get(topK, 0)
                    error[pt][topK] = v + 1
            print >>error_files[pt], w, pmi

for pt in pmi_types:
    for topK in topKs:
        precisoin[pt][topK] = 1.0 * correct[pt][topK] / (correct[pt][topK] + error[pt][topK])
        recall[pt][topK] = 1.0 * correct[pt][topK] / N
        fmeasure[pt][topK] = 2.0 * precisoin[pt][topK] * recall[pt][topK] / (precisoin[pt][topK] + recall[pt][topK])

of = codecs.open('evaluation.csv', 'w', 'utf8')
output_pairs = [('precisoin', precisoin), ('recall', recall), ('f-measure', fmeasure)]
for op in output_pairs:
    of.write('{},\n'.format(op[0]))
    of.write('pmi_type,')
    for topK in topKs:
        of.write('topK={},'.format(topK))
    of.write('\n')
    for pt in pmi_types:
        of.write('{},'.format(pt))
        for topK in topKs:
            of.write('{:.4f},'.format(op[1][pt][topK]))
        of.write('\n')
    of.write('\n')

