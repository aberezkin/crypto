from os import listdir
from os.path import join
from collections import Counter
import sys

text_dir = './text'

frequencies = [
' ',
'e',
't',
'a',
'o',
'i',
'h',
'n',
's',
'd',
'r',
'l',
'u',
'y',
'w',
'g',
'm',
'c',
'b',
'"',
'.',
'f',
'\'',
'p',
'k',
'v',
'-',
',',
'1',
'q',
'j',
'x',
'2',
'z',
'0',
'!',
'?',
'3',
'5',
'4',
'9',
'[',
'A',
'C',
'H',
'T',
'#',
'&',
';',
']',
'8',
'6',
'7',
]



files = [join(text_dir, f) for f in listdir(text_dir)];


text = ''
filename = './text/TheHacker0.txt' if len(sys.argv) == 1 else sys.argv[1] 
with open(filename, 'r') as f: text = f.read()

crypto_freqs = map(lambda x: x[0], Counter(text).most_common())

# decr_text = ''
# for c in text:
#   ind = crypto_freqs.index(c)
#   decr_text += frequencies[ind]

print crypto_freqs


