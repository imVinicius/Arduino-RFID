/*
 * --------------------------------------------------------------------------------------------------------------------
 * Exemplo para alterar o UID do cartão MIFARE mutável.
 * --------------------------------------------------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; para mais detalhes e outros exemplos, consulte: https://github.com/miguelbalboa/rfid
 *
 * Este exemplo mostra como definir o UID em um cartão MIFARE mutável.
 *
 * @author Tom Clement
 * @license Disponibilizado para o domínio público.
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
 * Mais layouts de pinos para outros boards podem ser encontrados aqui: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout típico de pinos acima
#define SS_PIN 10 // Configurável, veja o layout típico de pinos acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância do MFRC522

/* Defina o novo UID aqui! */
#define NEW_UID                \
    {                          \
        0xDE, 0xAD, 0xBE, 0xEF \
    }

MFRC522::MIFARE_Key chave;

void setup()
{
    Serial.begin(9600); // Inicializa a comunicação serial com o PC
    while (!Serial)
        ;               // Não faz nada se a porta serial não estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();        // Inicializa o barramento SPI
    mfrc522.PCD_Init(); // Inicializa o módulo MFRC522
    Serial.println(F("Aviso: este exemplo sobrescreve o UID do seu cartão mutável, use com cuidado!"));

    // Prepara a chave - todas as chaves são definidas como FFFFFFFFFFFFh na entrega do chip de fábrica.
    for (byte i = 0; i < 6; i++)
    {
        chave.keyByte[i] = 0xFF;
    }
}

// Definir o UID pode ser tão simples quanto isso:
// void loop() {
//  byte novoUid[] = NEW_UID;
//  if ( mfrc522.MIFARE_SetUid(novoUid, (byte)4, true) ) {
//    Serial.println("Novo UID gravado no cartão.");
//  }
//  delay(1000);
//}

// Mas, claro, esta é uma abordagem mais adequada
void loop()
{

    // Reinicia o loop se nenhum cartão novo estiver presente no sensor/leitor. Isso economiza o processo inteiro quando está inativo. E se presente, seleciona um.
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        delay(50);
        return;
    }

    // Agora um cartão está selecionado. O UID e SAK estão em mfrc522.uid.

    // Despeje o UID
    Serial.print(F("UID do Cartão:"));
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Despeje o tipo de PICC
    //  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    //  Serial.print(F("Tipo de PICC: "));
    //  Serial.print(mfrc522.PICC_GetTypeName(piccType));
    //  Serial.print(F(" (SAK "));
    //  Serial.print(mfrc522.uid.sak);
    //  Serial.print(")\r\n");
    //  if (  piccType != MFRC522::PICC_TYPE_MIFARE_MINI
    //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
    //    &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    //    Serial.println(F("Este exemplo só funciona com cartões MIFARE Classic."));
    //    return;
    //  }

    // Defina o novo UID
    byte novoUid[] = NEW_UID;
    if (mfrc522.MIFARE_SetUid(novoUid, (byte)4, true))
    {
        Serial.println(F("Novo UID gravado no cartão."));
    }

    // Pare o PICC e selecione-o novamente para que o DumpToSerial não fique confuso
    mfrc522.PICC_HaltA();
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    // Despeje o novo conteúdo da memória
    Serial.println(F("Novo UID e conteúdo:"));
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

    delay(2000);
}
