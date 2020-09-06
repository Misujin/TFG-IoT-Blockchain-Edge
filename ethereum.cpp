#include "ethereum.h"

// Clave privada del dispositivo (ETH)
uint8_t PRIVATE_KEY[] = { xxxx };

void showEthereumAddressData(){
    Serial.println("[ETH INFO] *DATOS DE LA ADDRESS THE ETHEREUM*");
    Serial.printf("[ETH INFO] Private key: %s\n", byteArrayToCharArray(PRIVATE_KEY, 32));
    Serial.printf("[ETH INFO] Public key: %s\n\n", byteArrayToCharArray(getPublicKey(PRIVATE_KEY), 64));
}

void registerNoiseInEthereum(int noise, boolean isLegalNoise, int maxAttempts){
    boolean success = false;
    for(int i = 0; i < maxAttempts && !success; i++){
      success = processRawTransactionEthereum(noise, isLegalNoise); //TODO: PONER EL NOISE OBTENIDO DEL PIN ANALOG DEL SENSOR
      if(i < maxAttempts-1 && !success){
        Serial.println("[TRANSACTION] NO SE HA PODIDO REGISTRAR NI PROCESAR LA TRANSACCION. INTENTANDO DE NUEVO (TRY " + String(i+1) + ")");
        Serial.println();
      }
    }
    if(!success){
      Serial.println("[TRANSACTION] TRAS " + String(maxAttempts) + " INTENTOS, NO SE HA PODIDO REGISTRAR LA TRANSACCION EN LA BLOCKCHAIN");
      Serial.println();
    }
}

boolean processRawTransactionEthereum(int noise, boolean isLegalNoise){
    Serial.println("[TRANSACTION] ---------------------------------------------- PROCESANDO NUEVA TRANSACCION ----------------------------------------------");
    Serial.println();
    int startTransactionTime = millis();
    
    Serial.println("[GETH HTTP] *OBTENIENDO EL NONCE Y EL PRECIO DEL GAS DE LA NUEVA TRANSACCION*");
    String nonce = eth_getTransactionCount();
    //Si es menor de 15 hay que añadir un 0 de padding al hexa para que no de errores. Por ej. 0xa => 0x0a
    if(nonce.length() == 3){
      nonce = "0" + nonce;
      nonce.setCharAt(1, 'x');
      nonce.setCharAt(2, '0');
    }
    Serial.println("[GETH HTTP] Nonce: " + nonce);
    String gasPrice = eth_gasPrice();
    Serial.println("[GETH HTTP] Gas price: " + gasPrice);
    Serial.println("");
    //Preparamos para la transaccion el nivel de ruido obtenido en hexadecimal
    String paddedZeroes;
    String functionHexa = "0xcb3cc78e"; //Nombre funcion registerNoise en codificacion ABI
    String hexNoise = String(noise, HEX);
    for(int x = 0; x < (64 - hexNoise.length()); x++)
    {
      paddedZeroes = String(paddedZeroes + "0");
    }
    //Indicamos el nombre de la funcion del contrato codificada dependiendo de si es un nivel de ruido legal o no
    if(isLegalNoise) {
      hexNoise = String(REGISTER_LEGAL_NOISE_FUNCTION + paddedZeroes + hexNoise);
    } else {
      hexNoise = String(REGISTER_ILLEGAL_NOISE_FUNCTION + paddedZeroes + hexNoise);
    }

    Serial.println("[TRANSACTION] *CONSTRUYENDO TRANSACCION TX*");
    
    struct TX tx = {};
    tx.nonce = nonce.c_str();
    tx.gasPrice = gasPrice.c_str(); //Antes: "0x3b9aca00"
    tx.gasLimit = "0x186A00"; // FDE8 25896 (72 * 68 + 21000)
    tx.to = ETH_CONTRACT_ADDRESS; //ADDRESS METAMASK FIREFOX
    //tx.value = "0x38d7ea4c68000"; //0x11c37937e08000 0.005
    tx.value = "0x";
    tx.data = hexNoise.c_str(); //Funcion registerNoise del contrato (0xcb3cc78e) + temperatura en hexa
    tx.chainId = "0x04";

    Serial.printf("[TRANSACTION] TX.nonce: %s\n", tx.nonce.c_str());
    Serial.printf("[TRANSACTION] TX.gasPrice: %s\n", tx.gasPrice.c_str());
    Serial.printf("[TRANSACTION] TX.gasLimit: %s\n", tx.gasLimit.c_str());
    Serial.printf("[TRANSACTION] TX.to: %s\n", tx.to.c_str());
    Serial.printf("[TRANSACTION] TX.value: %s\n", tx.value.c_str());
    Serial.printf("[TRANSACTION] TX.data: %s\n\n", tx.data.c_str());
    Serial.printf("[TRANSACTION] TX.chainId: %s\n\n", tx.chainId.c_str());

    Serial.println("[RLP] *SERIALIZANDO TX MEDIANTE RLP*");

    RLP rlp;
    std::string enc = rlp.encode(tx, true);
    Serial.printf("[RLP] Raw TX encode: %s\n\n", rlp.bytesToHex(enc).c_str());

    Serial.println("[KECCAK256] *CALCULANDO HASH KECCAK256 (SHA-3)*");

    uint8_t *hashval = new uint8_t[HASH_LENGTH];
    keccak256((uint8_t*)(enc.c_str()), enc.size(), hashval);
    Serial.printf("[KECCAK256] Hash: %s\n\n", byteArrayToCharArray(hashval, HASH_LENGTH));

    Serial.println("[ECDSA] *CALCULANDO FIRMA ECDSA*");

    uECC_set_rng(&RNG);
    const struct uECC_Curve_t * curve = uECC_secp256k1();
    uint8_t* signature = new uint8_t[SIGNATURE_LENGTH];
    uECC_sign(PRIVATE_KEY, hashval, HASH_LENGTH, signature, curve);
    Serial.printf("[ECDSA] Signature: %s\n", byteArrayToCharArray(signature, 64));
    //DESCOMENTAR SOLO SI QUEREMOS COMPROBAR QUE LA FIRMA CALCULADA ES CORRECTA (DEBUG/TESTING)
    //int result = uECC_verify(getPublicKey(PRIVATE_KEY), hashval, 32, signature, curve);
    //Serial.printf("[ECDSA] Sig match: %s\n\n", result ? "true" : "false");

    Serial.println("[TRANSACTION] *RESTO*");

    uint8_t *r = new uint8_t[32];
    uint8_t *s = new uint8_t[32];
    splitArray(signature,r,0,32);
    splitArray(signature,s,32,64);

    tx.r = std::string("0x") + byteArrayToCharArray(r, 32);
    tx.s = std::string("0x") + byteArrayToCharArray(s, 32);
    Serial.printf("[TRANSACTION] r: %s\n", tx.r.c_str());
    Serial.printf("[TRANSACTION] s: %s\n", tx.s.c_str());
    Serial.println();
    Serial.println("[TIMESTAMP] *TIEMPO EMPLEADO EN CONSTRUIR Y PROCESAR LA TRANSACCION* -> " + String(millis() - startTransactionTime));
    Serial.println();

    std::string v_alternatives[] = {"0x2b", "0x2c", "0x2d", "0x2e"}; //Puede ser 43, 44, 45 o 46 dependiendo de la firma
    bool success = false;
    for(int i=0;i<4 && !success;i++){
      tx.v = v_alternatives[i];
      Serial.printf("[TRANSACTION] Intentando registrar transaccion con i=%d, v=%s\n", i, tx.v.c_str());
      std::string encoded = rlp.bytesToHex(rlp.encode(tx, false));
      Serial.printf("[TRANSACTION] TX raw obtenida: 0x%s\n", encoded.c_str());

      if(eth_sendRawTransaction(encoded)){
        Serial.printf("[TRANSACTION] EXITO. Transaccion registrada en Rinkeby con v=%s\n", tx.v.c_str());
        success = true;
      } else{
        Serial.printf("[TRANSACTION] ERROR. Transaccion incorrecta con v=%s\n", tx.v.c_str());
      }
      Serial.println();
    }
    
    Serial.println("[TRANSACTION] ---------------------------------------------- TRANSACCION PROCESADA ----------------------------------------------");
    Serial.println();

    return success;
}
