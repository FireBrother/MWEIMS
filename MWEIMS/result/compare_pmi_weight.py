import codecs

f1 = codecs.open("pmi.txt","r","GBK")
f2 = codecs.open("weight.txt","r","GBK")
of1 = codecs.open("pmi_unique.txt","w","UTF8")
of2 = codecs.open("weight_unique.txt","w","UTF8")
ws1 = set()
ws2 = set()

K = 1000

for i, line in enumerate(f1):
    if i >= K:
        break
    ws1.add(line.split()[0])

for i, line in enumerate(f2):
    if i >= K:
        break
    ws2.add(line.split()[0])

for w in ws1:
    if w not in ws2:
        print >>of1, w

for w in ws2:
    if w not in ws1:
        print >>of2, w
