#include <SPI.h>
#include <MFRC522.h>
/** 

Módulo MFRC522 (um módulo leitor de cartão RFID) está conectado como pino Slave Select (SS)
    e em uma comunicação SPI (Serial Peripheral Interface).
    O pino SS é usado para selecionar o dispositivo com o qual o Arduino
        está se comunicando quando há vários dispositivos SPI conectados.
    O pino 9 do Arduino está sendo usado como o pino de reset para o módulo MFRC522.
    O pino 10 do Arduino está sendo usado como o pino do LED verde.
    O pino 7 do Arduino está sendo usado como o pino do LED vermelho.
**/
#define SS_PIN 10 // está conectado como pino Slave Select (SS) em uma comunicação SPI (Serial Peripheral Interface)
#define RST_PIN 9 // o pino 9 do Arduino está sendo usado como o pino de reset para o módulo MFRC522, leitor de cartão RFID
#define VERDE 8 // Pino do Led verde
#define VERMELHO 7 // Pino do LED vermelho

MF3 ?RC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key chave;

void setup()
{
    // Inicializa a comunicação serial
    Serial.begin(9600);

    // Inicializa o SPI
    SPI.begin();

    // Inicializa o módulo RFID
    rfid.PCD_Init();

    // Configura o pino do LED verde como saída
    pinMode(VERDE, OUTPUT);

    // Configura o pino do LED vermelho como saída
    pinMode(VERMELHO, OUTPUT);
}

void loop()
{
    // Verifica se há um novo cartão
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return;

    // Obtém o tipo do cartão
    MFRC522::PICC_Type tipoPICC = rfid.PICC_GetType(rfid.uid.sak);

    // Verifica se o cartão é do tipo MIFARE Classic
    if (tipoPICC != MFRC522::PICC_TYPE_MIFARE_MINI &&
        tipoPICC != MFRC522::PICC_TYPE_MIFARE_1K &&
        tipoPICC != MFRC522::PICC_TYPE_MIFARE_4K)
    /*
    PICC_TIPO_MIFARE_MINI,		  // Protocolo MIFARE Classic, 320 bytes
    PICC_TIPO_MIFARE_1K,		  // Protocolo MIFARE Classic, 1KB
    PICC_TIPO_MIFARE_4K,		  // Protocolo MIFARE Classic, 4KB
    */
    {
        Serial.println(F("Seu cartão não é do tipo MIFARE Classic."));
        return;
    }

    // Converte o UID do cartão para uma string
    String strID = "";
    for (byte i = 0; i < 4; i++)
    {
        strID +=
            (rfid.uid.uidByte[i] < 0x10 ? "0" : "")
            + String(rfid.uid.uidByte[i], HEX)
            + (i != 3 ? ":" : "");
    }

    strID.toUpperCase();
    Serial.print("Aproxime o cartão chave: ");
    Serial.println(strID);

    // Verifica se o cartão é a chave correta
    if (strID.indexOf("20:6B:18:83") >= 0)
    {
        digitalWrite(VERDE, HIGH); // Liga o LED verde
        digitalWrite(VERMELHO, LOW); // Desliga o LED vermelho
    }
    else
    {
        digitalWrite(VERDE, LOW); // Desliga o LED verde
        digitalWrite(VERMELHO, HIGH); // Liga o LED vermelho
    }

    // Para a comunicação com o cartão
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}