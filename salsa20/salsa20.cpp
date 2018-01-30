#include <iostream>
#include <iomanip>
#include <cassert>
#include <cstring>

const int VECTOR_SIZE = 16;
const int BLOCK_SIZE = 64;
const int KEY_SIZE = 32;
const int IV_SIZE = 8;
const int FULL_KEY_SIZE = KEY_SIZE + IV_SIZE;
const int IV_OFFSET = KEY_SIZE;
const int BLOCKS_PER_CHUNK = 8192;
const int CHUNK_SIZE = BLOCKS_PER_CHUNK * BLOCK_SIZE;

typedef unsigned int ui;
typedef unsigned char uc;
const char constants[] = "expand 32-byte k";

ui vec[VECTOR_SIZE];

ui little_endian(const uc* arr) {
  return (
    (static_cast<ui>(arr[0]) << 0)  |
    (static_cast<ui>(arr[1]) << 8)  |
    (static_cast<ui>(arr[2]) << 16) |
    (static_cast<ui>(arr[3]) << 24) 
  );
}

ui r(ui v, ui b) {
  return (v << b) | (v >> (32 - b));
}


void setk(uc* key) {
  vec[0] = little_endian(reinterpret_cast<const uc*>(&constants[0]));
  vec[1] = little_endian(&key[0]);
  vec[2] = little_endian(&key[4]);
  vec[3] = little_endian(&key[8]);
  vec[4] = little_endian(&key[12]);
  vec[5] = little_endian(reinterpret_cast<const uc*>(&constants[4]));
  
  vec[10] = little_endian(reinterpret_cast<const uc*>(&constants[8]));
  vec[11] = little_endian(&key[16]);
  vec[12] = little_endian(&key[20]);
  vec[13] = little_endian(&key[24]);
  vec[14] = little_endian(&key[28]);
  vec[15] = little_endian(reinterpret_cast<const uc*>(&constants[12]));
}

void setiv(uc* iv) {
  vec[6] = little_endian(&iv[0]);
  vec[7] = little_endian(&iv[1]);
  vec[8] = vec[9] = 0;
}

void incr() {
  ++vec[8];
  vec[9] += 0 == vec[8] ? 1 : 0;
}

void reset() {
  vec[8] = vec[9] = 0;
}

void reverse(ui value, uc* array) {
  array[0] = (uc)(value >> 0);
  array[1] = (uc)(value >> 8);
  array[2] = (uc)(value >> 16);
  array[3] = (uc)(value >> 24);
}

void get_stream(uc out[BLOCK_SIZE]) {
  ui x[VECTOR_SIZE];
  std::memcpy(x, vec, sizeof(vec));

  for (ui i = 20; i > 0; i -= 2) {
    x[ 4] ^= r((ui)(x[ 0] + x[12]),  7);
    x[ 8] ^= r((ui)(x[ 4] + x[ 0]),  9);
    x[12] ^= r((ui)(x[ 8] + x[ 4]), 13);
    x[ 0] ^= r((ui)(x[12] + x[ 8]), 18);
    x[ 9] ^= r((ui)(x[ 5] + x[ 1]),  7);
    x[13] ^= r((ui)(x[ 9] + x[ 5]),  9);
    x[ 1] ^= r((ui)(x[13] + x[ 9]), 13);
    x[ 5] ^= r((ui)(x[ 1] + x[13]), 18);
    x[14] ^= r((ui)(x[10] + x[ 6]),  7);
    x[ 2] ^= r((ui)(x[14] + x[10]),  9);
    x[ 6] ^= r((ui)(x[ 2] + x[14]), 13);
    x[10] ^= r((ui)(x[ 6] + x[ 2]), 18);
    x[ 3] ^= r((ui)(x[15] + x[11]),  7);
    x[ 7] ^= r((ui)(x[ 3] + x[15]),  9);
    x[11] ^= r((ui)(x[ 7] + x[ 3]), 13);
    x[15] ^= r((ui)(x[11] + x[ 7]), 18);
    x[ 1] ^= r((ui)(x[ 0] + x[ 3]),  7);
    x[ 2] ^= r((ui)(x[ 1] + x[ 0]),  9);
    x[ 3] ^= r((ui)(x[ 2] + x[ 1]), 13);
    x[ 0] ^= r((ui)(x[ 3] + x[ 2]), 18);
    x[ 6] ^= r((ui)(x[ 5] + x[ 4]),  7);
    x[ 7] ^= r((ui)(x[ 6] + x[ 5]),  9);
    x[ 4] ^= r((ui)(x[ 7] + x[ 6]), 13);
    x[ 5] ^= r((ui)(x[ 4] + x[ 7]), 18);
    x[11] ^= r((ui)(x[10] + x[ 9]),  7);
    x[ 8] ^= r((ui)(x[11] + x[10]),  9);
    x[ 9] ^= r((ui)(x[ 8] + x[11]), 13);
    x[10] ^= r((ui)(x[ 9] + x[ 8]), 18);
    x[12] ^= r((ui)(x[15] + x[14]),  7);
    x[13] ^= r((ui)(x[12] + x[15]),  9);
    x[14] ^= r((ui)(x[13] + x[12]), 13);
    x[15] ^= r((ui)(x[14] + x[13]), 18);
  }

  for (size_t i = 0; i < VECTOR_SIZE; ++i) {
    x[i] += vec[i];
    reverse(x[i], &out[4 * i]);
  }

  incr();
}

void encrypt(const uc* input, uc* output, size_t bytes_count) {
  uc stream[BLOCK_SIZE];
  size_t bytes;

  while (bytes_count != 0) {
    get_stream(stream);
    bytes = bytes_count >= BLOCK_SIZE ? BLOCK_SIZE : bytes_count;

    for (size_t i = 0; i < bytes; i++, bytes_count--) {
      *(output++) = stream[i] ^ *(input++);
    }
  }
}

void pretty_print(uc* output, int len) {
  int col = 0;
  int row = 0;
  for (size_t i = 0; i < len; i++) {

    std::cout << std::setfill('0') << std::setw(2) << std::hex << (ui) output[i];
    col = (col + 1) % 2;
    if (col == 0) {
      std::cout << ' ';
      row = (row + 1) % 8;
      if (row == 0) std::cout << '\n';
    }
  }
}

int main() {
  uc key[KEY_SIZE] = {0};
  uc iv[IV_SIZE] = {0};
  uc byte_key[FULL_KEY_SIZE];

  for (int i = 0; i < KEY_SIZE; i++) byte_key[i] = key[i];
  for (int i = 0; i < IV_SIZE; i++) byte_key[KEY_SIZE + i - 1] = iv[i];

  setk(byte_key);
  setiv(&byte_key[IV_OFFSET]);

  // TEST STREAM GENERATION
  // uc stream[BLOCK_SIZE];
  // get_stream(stream);

  // for (size_t i = 0; i < BLOCK_SIZE; i++) {
  //   std::cout << std::hex << (ui) stream[i];
  // }

  // TEST ENCRYPTION
  const size_t len = 512;
  uc input[len];
  for (int i = 0; i < len; i++) input[i] = 0;

  uc output[len];

  encrypt(input, output, len);

  pretty_print(output, len);

  reset();

  // Decrypt
  encrypt(output, output, len);

  pretty_print(output, len);

  std::cout << std::endl;
  return 0;
}
