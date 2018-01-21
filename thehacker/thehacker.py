from os import listdir
from os.path import join
from collections import Counter

text_dir = './text'

frequencies = [
' ',
'e',
't',
'a',
'o',
'i',
'n',
'h',
's',
'r',
'd',
'l',
'y',
'u',
'm',
'w',
'c',
'g',
'.',
'b',
'f',
'p',
',',
'k',
'\'',
'v',
'"',
'-',
'j',
'?',
'x',
'q',
'z',
'!',
'z',
'2',
'0',
'3',
'5',
'4',
'9',
'[',
']',
'8',
'6',
'7',
]

files = [join(text_dir, f) for f in listdir(text_dir)];


text = ''
with open('./text/TheHacker0.txt', 'r') as f: text = f.read()

crypto_freqs = map(lambda x: x[0], Counter(text).most_common())

decr_text = ''
for c in text:
  ind = crypto_freqs.index(c)
  decr_text += frequencies[ind]

print decr_text


