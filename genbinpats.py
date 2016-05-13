"""This small script will help you generate binary inputs from datasets.

It took 3 mandatory arguments:
- an input file: This is the dataset. It must contains one pattern per line
      and they must be comma seperated. Every patterns must have the same 
      length. A pattern attribute name can contains any character except 
      comma (wich is used to seperate attributes) and lf / cr of course.
- an output file: This is where binary patterns will be saved.
- a skip value: This value indicate the number of attributes which should be 
      ignored at the begining of every patterns (positive value) or at the end
      (negative value). For instance in the mushrooms
      dataset a pattern looks like:
          p,x,s,n,t,p,f,c,n,k,e,e,s,s,w,w,p,w,o,p,k,s,u
      where the first attribute is the edible class of the mushroom and we
      don't want to represent it in the binary patterns so we can let the
      ART1 network guess it. Then we call this program with 1 as the skip
      value and the first attribute will be ignored.

Note: when generating binary patterns for the mushrooms dataset:
The generated attribList contains a total of 117 values distributed in 22
attribute sub-lists. According to the dataset description it should have 126
attribute values but in fact some attribute values are never used:
- the "gill-attachment" attribute can only hold 2 values (f, a) instead of 4
- the "gill-spacing" can only hold 2 values (c, w) instead of 3
- the "stalk-root" can only hold 5 values (e, c, b, r, ?) instead of 7
- the "veil-type" can only one value (p) instead of 2 and is therefor useless
- the "ring-type" can only hold 5 values (p, e, l, f, n) instead of 8
So there is 126 - 117 = 9 "missing" bits in the generated patterns. This is
impactless as these bits would always be 0 for every patterns.
"""

import sys
from pprint import pprint

def gen_binary_input(line):
    out = ''
    for i in range(len(attribList)):
        for j in range(len(attribList[i])):
            out += '1,' if line[i] is attribList[i][j] else '0,'
    return out


if __name__ == '__main__':
    attribList = []
    
    if len(sys.argv) < 4:
        sys.stderr.write('ERROR: Please give me at least an input file\n')
        sys.exit(1)
    
    outf = open(sys.argv[2],'w')
    
    with open(sys.argv[1]) as inf:  # first pass: create the attributes list
        firstPat = True
        ln = 0
        for line in inf:
            ln += 1
            if int(sys.argv[3]) > 0:
                line = line[:-1].split(',')[int(sys.argv[3]):]
            else:
                line = line[:-1].split(',')[:int(sys.argv[3])]
            print line
            if firstPat:
                patLen = len(line)
                print 'Pattern length is', patLen, '\n'
                firstPat = False
                for i in range(patLen): attribList.append([])
            if patLen != len(line):
                print("ERROR: pattern %lu length (%lu) is different from previous patterns length (%lu)" % (ln, len(line), patLen))
                sys.exit(1)
            for i in range(patLen):
                if line[i] in attribList[i]: continue
                else: attribList[i].append(line[i])
                
    print('List of values collected for each %lu attributes:' % len(attribList))
    pprint(attribList)
    
    with open(sys.argv[1]) as inf:  # second pass: write the binary patterns
        for line in inf:
            if(int(sys.argv[3])) > 0:
                outf.write(line[0] + ',')
            else:
                outf.write(line[-2] + ',')
            if int(sys.argv[3]) > 0:
                line = line[:-1].split(',')[int(sys.argv[3]):]
            else:
                line = line[:-1].split(',')[:int(sys.argv[3])]
            outf.write(gen_binary_input(line))
            outf.write('\n')
    outf.close()
