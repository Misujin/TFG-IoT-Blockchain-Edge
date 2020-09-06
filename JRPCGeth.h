#ifndef JRPCGETH_H
#define JRPCGETH_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

//Realiza la llamada HTTP al nodo/cliente geth
String callRPCGethAPI(String inputJSON, String rpcMethod, bool *success);

//Obtiene la version actual del cliente
String web3_clientVersion();

//Devuelve el valor del numero de transacciones pendientes o nonce
String eth_getTransactionCount();

//Devuelve el precio del gas en ese momento
String eth_gasPrice();

//Procesa una transaccion ya firmada con el contenido raw de esta. Devuelve si se ha enviado con exito o no.
boolean eth_sendRawTransaction(std::string rawTX);

#endif
