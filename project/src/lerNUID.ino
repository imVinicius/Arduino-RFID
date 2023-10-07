/*
 * --------------------------------------------------------------------------------------------------------------------
 * Exemplo de esboço/programa mostrando como ler um novo NUID de um PICC para a porta serial.
 * --------------------------------------------------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; para mais detalhes e outros exemplos, consulte: https://github.com/miguelbalboa/rfid
 *
 * Este esboço demonstra como ler dados de um PICC (ou seja: uma etiqueta ou cartão RFID) usando um leitor RFID baseado em MFRC522
 * na interface SPI do Arduino.
 *
 * Quando o Arduino e o módulo MFRC522 estão conectados (veja o layout dos pinos abaixo), carregue este esboço no Arduino IDE,
 * compile e carregue-o. Para visualizar a saída, use a ferramenta "Monitor Serial" do IDE (pressione Ctrl+Shft+M).
 * Quando você apresenta um PICC (ou seja: uma etiqueta ou cartão RFID) na distância de leitura do Leitor/PCD MFRC522,
 * a saída serial mostrará o tipo e o NUID se um novo cartão for detectado. Observe: você pode ver mensagens de "Tempo limite na comunicação"
 * ao retirar o PICC da distância de leitura muito cedo.
 *
 * @license Liberado para o domínio público.
 *
 * Layout típico de pinos usado:
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

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Instância da classe

MFRC522::MIFARE_Key key;

// Inicializa a matriz que armazenará o novo NUID
byte nuidPICC[4];

void setup()
{
    Serial.begin(9600);
    SPI.begin();     // Inicializa o barramento SPI
    rfid.PCD_Init(); // Inicializa o MFRC522

    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Este código lê o NUID do MIFARE Classic."));
    Serial.print(F("Usando a seguinte chave:"));
    printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop()
{

    // Reinicia o loop se nenhum novo cartão estiver presente no sensor/leitor. Isso economiza todo o processo quando estiver inativo.
    if (!rfid.PICC_IsNewCardPresent())
        return;

    // Verifica se o NUID foi lido
    if (!rfid.PICC_ReadCardSerial())
        return;

    Serial.print(F("Tipo PICC: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    // Verifica se o PICC é do tipo Classic MIFARE
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Sua etiqueta não é do tipo MIFARE Classic."));
        return;
    }

    if (rfid.uid.uidByte[0] != nuidPICC[0] ||
        rfid.uid.uidByte[1] != nuidPICC[1] ||
        rfid.uid.uidByte[2] != nuidPICC[2] ||
        rfid.uid.uidByte[3] != nuidPICC[3])
    {
        Serial.println(F("Um novo cartão foi detectado."));

        // Armazena o NUID na matriz nuidPICC
        for (byte i = 0; i < 4; i++)
        {
            nuidPICC[i] = rfid.uid.uidByte[i];
        }

        Serial.println(F("O NUID da etiqueta é:"));
    Serial.print(F"No formato hexadecimal: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F"No formato decimal: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    }
    else
        Serial.println(F("Cartão lido anteriormente."));

    // Interrompe o PICC
    rfid.PICC_HaltA();

    // Interrompe a criptografia no PCD
    rfid.PCD_StopCrypto1();
}

/**
 * Rotina auxiliar para exibir uma matriz de bytes como valores hexadecimais no Serial.
 */
void printHex(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Rotina auxiliar para exibir uma matriz de bytes como valores decimais no Serial.
 */
void printDec(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(' ');
        Serial.print(buffer[i], DEC);
    }
}
