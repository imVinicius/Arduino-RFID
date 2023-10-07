/**
 * --------------------------------------------------------------------------------------------------------------------
 * Exemplo de esboço/programa mostrando como ler dados de mais de um PICC (ou seja, uma etiqueta ou cartão RFID) para a porta serial.
 * --------------------------------------------------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; para mais detalhes e outros exemplos, consulte: https://github.com/miguelbalboa/rfid
 *
 * Aviso: Isso pode não funcionar! Vários dispositivos em um barramento SPI são difíceis e causam muitos problemas!! Habilidades de engenharia
 * e conhecimento são necessários!
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
 * SPI SS 1    SDA(SS)      ** personalizado, use um pino não utilizado, apenas HIGH/LOW necessário **
 * SPI SS 2    SDA(SS)      ** personalizado, use um pino não utilizado, apenas HIGH/LOW necessário **
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Mais layouts de pinos para outras placas podem ser encontrados aqui: https://github.com/miguelbalboa/rfid#pin-layout
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9   // Configurável, veja o layout típico dos pinos acima
#define SS_1_PIN 10 // Configurável, use um pino não utilizado, apenas HIGH/LOW necessário, deve ser diferente do SS 2
#define SS_2_PIN 8  // Configurável, use um pino não utilizado, apenas HIGH/LOW necessário, deve ser diferente do SS 1

#define NR_OF_READERS 2

byte ssPins[] = {SS_1_PIN, SS_2_PIN};

MFRC522 mfrc522[NR_OF_READERS]; // Crie instâncias MFRC522.

/**
 * Inicialização.
 */
void setup()
{

    Serial.begin(9600); // Inicialize a comunicação serial com o PC
    while (!Serial)
        ; // Não faça nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)

    SPI.begin(); // Inicialize o barramento SPI

    for (uint8_t leitor = 0; leitor < NR_OF_READERS; leitor++)
    {
        mfrc522[leitor].PCD_Init(ssPins[leitor], RST_PIN); // Inicialize cada cartão MFRC522
        Serial.print(F("Leitor "));
        Serial.print(leitor);
        Serial.print(F(": "));
        mfrc522[leitor].PCD_DumpVersionToSerial();
    }
}

/**
 * Loop principal.
 */
void loop()
{

    for (uint8_t leitor = 0; leitor < NR_OF_READERS; leitor++)
    {
        // Procure por novos cartões

        if (mfrc522[leitor].PICC_IsNewCardPresent() && mfrc522[leitor].PICC_ReadCardSerial())
        {
            Serial.print(F("Leitor "));
            Serial.print(leitor);
            // Mostrar alguns detalhes do PICC (ou seja, o tag/cartão)
            Serial.print(F(": UID do Cartão:"));
            dump_byte_array(mfrc522[leitor].uid.uidByte, mfrc522[leitor].uid.size);
            Serial.println();
            Serial.print(F("Tipo do PICC: "));
            MFRC522::PICC_Type piccType = mfrc522[leitor].PICC_GetType(mfrc522[leitor].uid.sak);
            Serial.println(mfrc522[leitor].PICC_GetTypeName(piccType));

            // Interromper PICC
            mfrc522[leitor].PICC_HaltA();
            // Parar a criptografia no PCD
            mfrc522[leitor].PCD_StopCrypto1();
        } // if (mfrc522[leitor].PICC_IsNewC
    }     // for(uint8_t leitor
}

/**
 * Rotina auxiliar para exibir uma matriz de bytes como valores hexadecimais no Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
