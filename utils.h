/*
  utils.h - Funciones auxiliares parsing/strings
*/
#ifndef utils_H
#define utils_H

#include <stdio.h>
#include <string>
#include <cstring>
#include <driver/adc.h>
#include "uECC.h"

char* byteArrayToCharArray(uint8_t* bytes, uint8_t len);
uint8_t* charArrayToByteArray(char* string);
void splitArray(uint8_t src[], uint8_t dest[], uint8_t from, uint8_t to);
uint8_t* getPublicKey(uint8_t* privatekey);
int RNG(uint8_t *dest, unsigned size);

#endif
