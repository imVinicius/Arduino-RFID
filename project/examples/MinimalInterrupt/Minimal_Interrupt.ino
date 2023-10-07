/*
 * Este é um exemplo de biblioteca MFRC522; veja https://github.com/miguelbalboa/rfid
 * para mais detalhes e outros exemplos.
 *
 * NOTA: O arquivo de biblioteca MFRC522.h contém muitas informações úteis. Por favor, leia-o.
 *
 * Liberado para o domínio público.
 * ----------------------------------------------------------------------------
 * Exemplo mínimo de como usar interrupções para ler o UID de um MIFARE Classic PICC
 * (= cartão/etiqueta).
 *
 * Layout típico de pinos usado:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Leitor/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Sinal       Pino         Pino          Pino      Pino       Pino             Pino
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5      RST
 * SPI SS      SDA(SS)      10            53        D10        3                 10
 * IRQ         ?            ?             ?         ?          2                 10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4            16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1            14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3            15
 *
 * Mais layouts de pinos para outras placas podem ser encontrados aqui: https://github.com/miguelbalboa/rfid#pin-layout
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout de pinos típico acima
#define SS_PIN 3  // Configurável, veja o layout de pinos típico acima
#define IRQ_PIN 2 // Configurável, depende do hardware

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância MFRC522.

MFRC522::MIFARE_Key key;

volatile bool bNovaInterrupcao = false;
byte regVal = 0x7F;
void ativarRecepcao(MFRC522 mfrc522);
void limparInterrupcao(MFRC522 mfrc522);

/**
 * Inicialização.
 */
void setup()
{
    Serial.begin(115200); // Inicializa a comunicação serial com o PC
    while (!Serial)
        ;        // Não faça nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin(); // Inicializa o barramento SPI

    mfrc522.PCD_Init(); // Inicializa o cartão MFRC522

    /* leia e imprima a versão do MFRC522 (valores válidos 0x91 e 0x92) */
    Serial.print(F("Ver: 0x"));
    byte readReg = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
    Serial.println(readReg, HEX);

    /* configura o pino de IRQ */
    pinMode(IRQ_PIN, INPUT_PULLUP);

    /*
     * Permita que a interrupção... seja propagada para o pino IRQ
     * Para fins de teste, propague o IdleIrq e loAlert
     */
    regVal = 0xA0; // rx irq
    mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);

    bNovaInterrupcao = false; // bandeira de interrupção

    /* Ative a interrupção */
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), lerCartao, FALLING);

    do
    { // limpar uma interrupção espúria no início
        ;
    } while (!bNovaInterrupcao);
    bNovaInterrupcao = false;

    Serial.println(F("Configuração concluída"));
}

/**
 * Loop principal.
 */
void loop()
{
    if (bNovaInterrupcao)
    { // nova interrupção de leitura
        Serial.print(F("Interrupção. "));
        mfrc522.PICC_ReadCardSerial(); // leia os dados do cartão
        // Mostre alguns detalhes do PICC (ou seja, o cartão)
        Serial.print(F("UID do cartão:"));
        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
        Serial.println();

        limparInterrupcao(mfrc522);
        mfrc522.PICC_HaltA();
        bNovaInterrupcao = false;
    }

    // O bloco receptor precisa ser acionado regularmente (informar à tag que ela deve transmitir??)
    // (mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);)
    ativarRecepcao(mfrc522);
    delay(100);
} // loop()

/**
 * Rotina auxiliar para imprimir uma matriz de bytes como valores hexadecimais no Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
/**
 * Rotina de atendimento à interrupção do MFRC522
 */
void lerCartao()
{
    bNovaInterrupcao = true;
}

/*
 * A função envia para o MFRC522 os comandos necessários para ativar a recepção
 */
void ativarRecepcao(MFRC522 mfrc522)
{
    mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
    mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
    mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
}

/*
 * A função para limpar os bits de interrupção pendentes após a rotina de atendimento à interrupção
 */
void limparInterrupcao(MFRC522 mfrc522)
{
    mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
}
