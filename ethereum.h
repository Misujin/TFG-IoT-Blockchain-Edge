/*
  ethereum.h - Funciones core del procesamiento de la transaccion
*/
#ifndef ethereum_H
#define ethereum_H

#include <string>
#include <cstring>
#include "config.h"
#include "utils.h"
#include "JRPCGeth.h"
#include "keccak256.h"
#include "uECC.h"
#include "RLP.h"
#include "TX.h"

void showEthereumAddressData();

void registerNoiseInEthereum(int noise, boolean isLegalNoise, int maxAttempts);

boolean processRawTransactionEthereum(int noise, boolean isLegalNoise);

#endif
