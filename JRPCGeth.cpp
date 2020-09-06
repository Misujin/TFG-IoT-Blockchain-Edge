#include "JRPCGeth.h"
#include "config.h"

//Llamada JRPC al nodo geth
String callRPCGethAPI(String inputJSON, String rpcMethod, bool &success) 
{
  HTTPClient http;
  String JSONResult = "";
  int httpCode = -1;

  //Serial.println("[GETH HTTP] HTTP request to " + rpcMethod + " with body " + inputJSON);

  http.begin("http://192.168.1.40:8545/");
  http.addHeader("Content-Type", "application/json");

  httpCode = http.POST(inputJSON);

  if (httpCode > 0) { //Check for the returning code
    JSONResult = http.getString();
    success = true;
    if(!rpcMethod.equals("eth_sendRawTransaction")){
      Serial.println("[GETH HTTP] HTTP request code " + String(httpCode) + " on " + rpcMethod);
    }
    //Si la llamada realizada es para procesar la transaccion, comprobamos que no haya error
    if(rpcMethod.equals("eth_sendRawTransaction") && JSONResult.indexOf("error") >= 0){
      success = false;
    }
  }
  else {
    success = false;
    Serial.println("[GETH HTTP] Error on HTTP request calling method: " + rpcMethod);
  }

  http.end();
  
  return JSONResult;
}

//Devuelve la version actual del cliente web3 al que se ha conectado
String web3_clientVersion(){
  //Serializamos los datos a JSON para realizar la llamada
  StaticJsonDocument<100> doc;
  String builtJSON;
  bool success = false;

  doc["jsonrpc"] = "2.0";
  doc["method"] = "web3_clientVersion";
  doc["params"] = serialized("[]");
  doc["id"] = 0;

  serializeJson(doc, builtJSON);
  String JSONResponse = callRPCGethAPI(builtJSON, "web3_clientVersion", success);

  //Deserializamos la respuesta en JSON obtenida (version) por el cliente GETH
  const int capacity = JSON_OBJECT_SIZE(3) + 100;
  StaticJsonDocument<capacity> doc2;
  
  DeserializationError err = deserializeJson(doc2, JSONResponse);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  }
  const char* version = doc2["result"];

  return version;
}

String eth_getTransactionCount(){
  //Serializamos los datos a JSON para realizar la llamada
  StaticJsonDocument<100> doc;
  String builtJSON;
  bool success = false;

  doc["jsonrpc"] = "2.0";
  doc["method"] = "eth_getTransactionCount";
  doc["params"] = serialized("[\"0x522Ea81a2cB594b4D453cf6B521987a0DD4341CA\",\"latest\"]");
  doc["id"] = 0;

  serializeJson(doc, builtJSON);
  String JSONResponse = callRPCGethAPI(builtJSON, "eth_getTransactionCount", success);

  //Deserializamos la respuesta en JSON obtenida (version) por el cliente GETH
  const int capacity = JSON_OBJECT_SIZE(4) + 100;
  StaticJsonDocument<capacity> doc2;
  
  DeserializationError err = deserializeJson(doc2, JSONResponse);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  }
  const char* nonce = doc2["result"];

  return nonce;
}

String eth_gasPrice(){
  //Serializamos los datos a JSON para realizar la llamada
  StaticJsonDocument<100> doc;
  String builtJSON;
  bool success = false;

  doc["jsonrpc"] = "2.0";
  doc["method"] = "eth_gasPrice";
  doc["params"] = serialized("[]");
  doc["id"] = 0;

  serializeJson(doc, builtJSON);
  String JSONResponse = callRPCGethAPI(builtJSON, "eth_gasPrice", success);

  //Deserializamos la respuesta en JSON obtenida (version) por el cliente GETH
  const int capacity = JSON_OBJECT_SIZE(4) + 100;
  StaticJsonDocument<capacity> doc2;
  
  DeserializationError err = deserializeJson(doc2, JSONResponse);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  }
  const char* gas = doc2["result"];

  return gas;
}

boolean eth_sendRawTransaction(std::string rawTX) {
  StaticJsonDocument<1000> doc;
  String builtJSON; 
  bool success = false;

  //Serial.println("HOLA");
  //Serial.printf("%s\n",rawTX.c_str());
  
  doc["jsonrpc"] = "2.0";
  doc["method"] = "eth_sendRawTransaction";
  doc["params"] = serialized("[\"0x" + rawTX + "\"]");
  doc["id"] = 0;

  serializeJson(doc, builtJSON);

  callRPCGethAPI(builtJSON, "eth_sendRawTransaction", success);

  return success;
}
