/*
 * Autor Inicial: ryand1011 (https://github.com/ryand1011)
 *
 * Lê dados gravados por um programa como "rfid write personal_data.ino"
 *
 * Veja: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
 *
 * Usa um cartão MIFARE RFID com leitor RFID-RC522
 * Usa a Biblioteca MFRC522
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
 * Mais layouts de pinos para outras placas podem ser encontrados aqui: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout típico dos pinos acima
#define SS_PIN 10 // Configurável, veja o layout típico dos pinos acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância MFRC522

//*****************************************************************************************//
void setup()
{
    Serial.begin(9600);                                         // Inicializa a comunicação serial com o PC
    SPI.begin();                                                // Inicializa o barramento SPI
    mfrc522.PCD_Init();                                         // Inicializa o cartão MFRC522
    Serial.println(F("Ler dados pessoais em um MIFARE PICC:")); // Mostra na serial que está pronto para ler
}

//*****************************************************************************************//
void loop()
{

    // Prepara a chave - todas as chaves são definidas como FFFFFFFFFFFFh na entrega do chip de fábrica.
    MFRC522::MIFARE_Key chave;
    for (byte i = 0; i < 6; i++)
        chave.keyByte[i] = 0xFF;

    // Algumas variáveis que precisamos
    byte bloco;
    byte len;
    MFRC522::StatusCode status;

    //-------------------------------------------

    // Reinicie o loop se não houver um novo cartão presente no sensor/leitor. Isso economiza o processo inteiro quando estiver inativo.
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Seleciona um dos cartões
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.println(F("**Cartão Detectado:**"));

    //-------------------------------------------

    mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); // Mostra alguns detalhes sobre o cartão

    // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      // Descomente isso para ver todos os blocos em hexadecimal

    //-------------------------------------------

    Serial.print(F("Nome: "));

    byte buffer1[18];

    bloco = 4;
    len = 18;

    //------------------------------------------- OBTER PRIMEIRO NOME
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &chave, &(mfrc522.uid)); // linha 834 do arquivo MFRC522.cpp
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Autenticação falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    status = mfrc522.MIFARE_Read(bloco, buffer1, &len);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Leitura falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // IMPRIME O PRIMEIRO NOME
    for (uint8_t i = 0; i < 16; i++)
    {
        if (buffer1[i] != 32)
        {
            Serial.write(buffer1[i]);
        }
    }
    Serial.print(" ");

    //---------------------------------------- OBTER ÚLTIMO NOME

    byte buffer2[18];
    bloco = 1;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &chave, &(mfrc522.uid)); // linha 834
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Autenticação falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    status = mfrc522.MIFARE_Read(bloco, buffer2, &len);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Leitura falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // IMPRIME O ÚLTIMO NOME
    for (uint8_t i = 0; i < 16; i++)
    {
        Serial.write(buffer2[i]);
    }

    //----------------------------------------

    Serial.println(F("\n**Fim da Leitura**\n"));

    delay(1000); // mude o valor se quiser ler os cartões mais rapidamente

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}
//*****************************************************************************************//
