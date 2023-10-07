/*
 * --------------------------------------------------------------------------------------------------------------------
 * Exemplo de esboço/programa para testar o firmware do seu módulo leitor MFRC522.
 * --------------------------------------------------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; para mais detalhes e outros exemplos, veja: https://github.com/miguelbalboa/rfid
 *
 * Este exemplo testa o firmware do seu módulo leitor MFRC522, apenas versões conhecidas podem ser verificadas. Se o teste passar,
 * isso não significa que o seu módulo não tenha defeitos! Alguns módulos têm antenas ruins ou quebradas ou o PICC está com defeito.
 *
 * @autor Rotzbua
 * @licença Liberado para o domínio público.
 *
 * Layout de pinos típico usado:
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

#define RST_PIN 9 // Configurável, veja o layout de pinos típico acima
#define SS_PIN 10 // Configurável, veja o layout de pinos típico acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância MFRC522

/**
 * Verifica o firmware apenas uma vez na inicialização
 */
void setup()
{
    Serial.begin(9600); // Inicializa a comunicação serial com o PC
    while (!Serial)
        ;               // Não faz nada se a porta serial não estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();        // Inicializa o barramento SPI
    mfrc522.PCD_Init(); // Inicializa o módulo MFRC522

    Serial.println(F("*****************************"));
    Serial.println(F("Teste de autoteste digital do MFRC522"));
    Serial.println(F("*****************************"));
    mfrc522.PCD_DumpVersionToSerial(); // Mostra a versão do PCD - Leitor de Cartões MFRC522
    Serial.println(F("-----------------------------"));
    Serial.println(F("Apenas versões conhecidas são suportadas"));
    Serial.println(F("-----------------------------"));
    Serial.println(F("Realizando o teste..."));
    bool resultado = mfrc522.PCD_PerformSelfTest(); // realiza o teste
    Serial.println(F("-----------------------------"));
    Serial.print(F("Resultado: "));
    if (resultado)
        Serial.println(F("OK"));
    else
        Serial.println(F("DEFEITO ou DESCONHECIDO"));
    Serial.println();
}

void loop() {} // nada a fazer
