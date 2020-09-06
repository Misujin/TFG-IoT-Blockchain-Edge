#include "utils.h"
using namespace std;

char* byteArrayToCharArray(uint8_t* bytes, uint8_t len) {

  char *ret = new char[len*2 + 1];
  char hexval[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  for(int j = 0; j < len; j++){
    ret[j*2] = hexval[((bytes[j] >> 4) & 0xF)];
    ret[(j*2) + 1] = hexval[(bytes[j]) & 0x0F];
  }
  ret[len*2] = '\0';
  return ret;
}

uint8_t* charArrayToByteArray(char* string) {

    if(string == NULL)
       return NULL;

    size_t slength = strlen(string);
    if((slength % 2) != 0)
       return NULL;

    size_t dlength = slength / 2;

    uint8_t* data = new uint8_t[dlength];
    memset(data, 0, dlength);

    size_t index = 0;
    while (index < slength) {
        char c = string[index];
        int value = 0;
        if(c >= '0' && c <= '9')
            value = (c - '0');
        else if (c >= 'A' && c <= 'F')
            value = (10 + (c - 'A'));
        else if (c >= 'a' && c <= 'f')
             value = (10 + (c - 'a'));
        else {
            delete[] data;
            return NULL;
        }

        data[(index/2)] += value << (((index + 1) % 2) * 4);

        index++;
    }

    return data;
}

void splitArray(uint8_t src[], uint8_t dest[], uint8_t from, uint8_t to)
{
    int i = 0;
  for(int ctr=from; ctr<to; ctr++)
  {
    dest[i] =  src[ctr];
    i++;
  }
}

uint8_t* getPublicKey(uint8_t* privatekey) {
    uint8_t *publickey = new uint8_t[64];
    uECC_compute_public_key(privatekey, publickey, uECC_secp256k1());

    return publickey;
}

int RNG(uint8_t *dest, unsigned size) {
  while (size) {
    uint8_t val = 0;
    for (unsigned i = 0; i < 30; ++i) {
      int init = hall_sensor_read();
      int count = 0;
      while (hall_sensor_read() == init) {
        ++count;
      }
      
      if (count == 0) {
         val = (val << 1) | (init & 0x01);
      } else {
         val = (val << 1) | (count & 0x01);
      }
    }
    *dest = val;
    ++dest;
    --size;
  }
  
  return 1;
}
