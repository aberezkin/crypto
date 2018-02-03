import hashlib
import sys
import argparse

HASH_SIZE = 128
CHUNK_SIZE = 2048

def hash_chunk(chunk, prev_hash = b''):
  return hashlib.sha512(chunk + prev_hash).hexdigest().encode('utf-8');

def generate_hashed_chunk(in_file, prev_hash=b'', chunk_size = CHUNK_SIZE):
  chunk = in_file.read(chunk_size)
  
  if len(chunk) == 0: return False

  h = hash_chunk(chunk, prev_hash);
  return chunk + h

def read_chunk(file, chunk_size = CHUNK_SIZE):
  chunk_hash = file.read(chunk_size + HASH_SIZE)
  if (len(chunk_hash) == 0): return False, False

  chunk = chunk_hash[0:-HASH_SIZE] 
  hash  = chunk_hash[-HASH_SIZE:]

  return chunk, hash

def unsign_file(in_filename, out_filename, chunk_size = CHUNK_SIZE):
  content = b''
  with open(in_filename, 'rb') as file:
    chunk, h = read_chunk(file, chunk_size)
    prev_hash = b''
    # i = 0
    while chunk != False and h != False:
      manual_hash = hashlib.sha512(chunk + prev_hash).hexdigest().encode('utf-8')
      # print(chunk)
      # print(h)
      # print(prev_hash)
      # i += 1
      if h != manual_hash:
        # print(i)
        print('Error: looks like data is corrupted')
        return False
      prev_hash = h
      content += chunk
      chunk, h = read_chunk(file, chunk_size)

  with open(out_filename, 'wb') as file: file.write(content)


def sign_file(in_filename, out_filename, chunk_size = CHUNK_SIZE):
  with open(in_filename, 'rb') as file:
    h = generate_hashed_chunk(file, chunk_size=chunk_size)
    res = b''

    while h != False:
      res += h
      h = generate_hashed_chunk(file, h[chunk_size:], chunk_size)

  with open(out_filename, 'wb') as file: file.write(res)


# def main():
#   if len(sys.argv) <= 1:
#     print('Specify a filename')
#     return

#   filename = sys.argv[1]

#   print(filename[:-4])
#   # sign_file(filename, '{0}.out'.format(filename))
#   unsign_file(filename, filename[-4:])


if __name__ == '__main__':
  parser = argparse.ArgumentParser(description='Sign/Unsign file for streaming')

  parser.add_argument(
    'filename', 
    action='store', 
    metavar='filename', 
    help='A file to be signed or unsigned'
  )

  parser.add_argument(
    '-o', '--output',
    action='store', 
    metavar='Output', 
    dest='output',
    default='signature.out',
    help='A file signed or unsigned'
  )

  parser.add_argument(
    '-u', '--unsign',
    action='store_const', 
    dest='signature',
    const=unsign_file,
    default=sign_file,
    help='Unsign file'
  )

  parser.add_argument(
    '-s', '--chunk-size',
    action='store',
    dest='size',
    default=CHUNK_SIZE,
    help='Unsign file'
  )

  args = parser.parse_args()
  
  args.signature(args.filename, args.output, args.size)  