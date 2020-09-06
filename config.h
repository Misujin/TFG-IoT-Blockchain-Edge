#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// ** CONSTANTES DEL PUNTO DE ACCESO WI-FI **
// SSID del punto de acceso
#define ENV_SSID     "xxxx"
// Contraseña del punto de acceso
#define ENV_WIFI_KEY "xxxx"

// ** CONSTANTES DE HASHING KECCAK256 Y FIRMADO ECDSA **
// Tamaño de la cadena que contiene el hash
#define HASH_LENGTH 32
// Tamaño de la cadena que contiene la firma
#define SIGNATURE_LENGTH 64

// ** CONSTANTES DIRECCIONES ETHEREUM **
// Direccion origen de la transaccion (ESP-32)
static const std::string ETH_FROM_ADDRESS = "xxxx";
// Direccion destino de la transaccion (SMART-CONTRACT)
#define ETH_CONTRACT_ADDRESS "xxxx"
// Nombre de la funcion codificada del smart contract para registrar ruidos que no superan el umbral legal (LEGALES)
#define REGISTER_LEGAL_NOISE_FUNCTION "xxxx"
// Nombre de la funcion codificada del smart contract para registrar ruidos que superan el umbral legal (ILEGALES)
#define REGISTER_ILLEGAL_NOISE_FUNCTION "xxxx"
// Chain ID (Rinkeby)
static const int CHAIN_ID = 4;
// Offset para el calculo de V
static const int CHAIN_ID_INC = 35;


#endif
