/*
 * Escreve dados pessoais em um cartão RFID MIFARE usando um leitor RFID-RC522
 * Usa a biblioteca MFRC522 para usar o KIT MÓDULO ARDUINO RFID 13,56 MHZ COM TAGS SPI W E R BY COOQROBOT.
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Leitor/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Sinal       Pino         Pino          Pino      Pino       Pino             Pino
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Mais layouts de pinos para outros placas podem ser encontrados aqui: https://github.com/miguelbalboa/rfid#pin-layout
 *
 * Hardware necessário:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): Um cartão ou tag usando a interface ISO 14443A, por exemplo, Mifare ou NTAG203.
 * O leitor pode ser encontrado no eBay por cerca de 5 dólares. Procure por "mf-rc522" em ebay.com.
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout típico dos pinos acima
#define SS_PIN 10 // Configurável, veja o layout típico dos pinos acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância MFRC522

void setup()
{
    Serial.begin(9600); // Inicializa comunicações seriais com o PC
    SPI.begin();        // Inicializa o barramento SPI
    mfrc522.PCD_Init(); // Inicializa o cartão MFRC522
    Serial.println(F("Escreva dados pessoais em um MIFARE PICC"));
}

void loop()
{

    // Prepara a chave - todas as chaves são definidas como FFFFFFFFFFFFh na entrega da fábrica.
    MFRC522::MIFARE_Key chave;
    for (byte i = 0; i < 6; i++)
        chave.keyByte[i] = 0xFF;

    // Reinicia o loop se não houver um novo cartão presente no sensor/leitor. Isso economiza o processo inteiro quando estiver inativo.
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Seleciona um dos cartões
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.print(F("UID do cartão:")); // Mostra o UID
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.print(F(" Tipo PICC: ")); // Mostra o tipo do PICC
    MFRC522::PICC_Type tipoPICC = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(tipoPICC));

    byte buffer[34];
    byte bloco;
    MFRC522::StatusCode status;
    byte len;

    Serial.setTimeout(20000L); // Aguarda até 20 segundos para entrada serial
    // Solicita dados pessoais: Sobrenome
    Serial.println(F("Digite o sobrenome, terminando com #"));
    len = Serial.readBytesUntil('#', (char *)buffer, 30); // Lê o sobrenome da porta serial
    for (byte i = len; i < 30; i++)
        buffer[i] = ' '; // Preenche com espaços

    bloco = 1;
    // Serial.println(F("Autenticando usando chave A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &chave, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else
        Serial.println(F("PCD_Authenticate() com sucesso: "));

    // Escreve no bloco
    status = mfrc522.MIFARE_Write(bloco, buffer, 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Write() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else
        Serial.println(F("MIFARE_Write() com sucesso: "));

    bloco = 2;
    // Serial.println(F("Autenticando usando chave A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &chave, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Escreve no bloco
    status = mfrc522.MIFARE_Write(bloco, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Write() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else
        Serial.println(F("MIFARE_Write() com sucesso: "));

    // Solicita dados pessoais: Nome
    Serial.println(F("Digite o nome, terminando com #"));
    len = Serial.readBytesUntil('#', (char *)buffer, 20); // Lê o nome da porta serial
    for (byte i = len; i < 20; i++)
        buffer[i] = ' '; // Preenche com espaços

    bloco = 4;
    // Serial.println(F("Autenticando usando chave A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &chave, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Escreve no bloco
    status = mfrc522.MIFARE_Write(bloco, buffer, 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Write() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else
        Serial.println(F("MIFARE_Write() com sucesso: "));

    bloco = 5;
    // Serial.println(F("Autenticando usando chave A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &chave, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Escreve no bloco
    status = mfrc522.MIFARE_Write(bloco, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Write() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    else
        Serial.println(F("MIFARE_Write() com sucesso: "));

    Serial.println(" ");
    mfrc522.PICC_HaltA();      // Finaliza o PICC
    mfrc522.PCD_StopCrypto1(); // Interrompe a criptografia no PCD
}
