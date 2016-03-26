import sys

def corpus_to_tokens(filename):
    f = open(filename)
    for line in f:
        line = line.strip()
        tokens = line.split()
        if line == '':
            continue
        #yield ('START', '<s>')
        for token in tokens:
            if token[0] == '[':
                token = token[1:]
            if ']' in token:
                token = token[:token.find(']')]
            token = token.split('/')
            try:
                yield (token[0], 'x')
            except IndexError:
                yield (token[0], 'x')
        #yield ('<s/>', '')
        yield ('', '')

def gen_seg_file():
    of = open('PeopleDaily_seg.txt', 'w')
    s = ''
    for token in corpus_to_tokens('PeopleDaily.txt'):
        word = token[0]
        tag = token[1]
        if word == '' and tag != '':
            continue
        if len(word) == 0 and len(tag) == 0:
            print >>of, s.strip()
            s = ''
        s += (word+' ').decode('utf8').encode('gbk')

if __name__=="__main__":
    eval(sys.argv[1])
