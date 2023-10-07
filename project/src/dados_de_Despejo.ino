/*
 * --------------------------------------------------------------------------------------------------------------------
 * Exemplo de esboço/programa mostrando como ler dados de um PICC para serial.
 * --------------------------------------------------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; para mais detalhes e outros exemplos, veja: https://github.com/miguelbalboa/rfid
 *
 * Esboço/programa de exemplo que mostra como ler dados de um PICC (ou seja, um RFID Tag ou Cartão) usando um Leitor RFID baseado em MFRC522 na interface SPI do Arduino.
 *
 * Quando o Arduino e o módulo MFRC522 estão conectados (veja o layout dos pinos abaixo), carregue este esboço no IDE do Arduino,
 * verifique/compile e faça o upload. Para ver a saída: use Ferramentas, Monitor Serial do IDE (pressione Ctrl+Shft+M).
 * Quando você apresenta um PICC (ou seja, um RFID Tag ou Cartão) a uma distância de leitura do Leitor/PCD MFRC522,
 * a saída serial mostrará o ID/UID, o tipo e quaisquer blocos de dados que ele possa ler. Observe que você pode ver mensagens
 * de "Timeout in communication" ao remover o PICC da distância de leitura muito cedo.
 *
 * Se o seu leitor suportar, este esboço/programa lerá todos os PICCs apresentados (ou seja, leitura de vários tags).
 * Portanto, se você empilhar dois ou mais PICCs um em cima do outro e apresentá-los ao leitor, ele primeiro mostrará todos
 * os detalhes do primeiro e, em seguida, do próximo PICC. Observe que isso pode levar algum tempo, pois todos os blocos de dados
 * são exibidos, portanto, mantenha os PICCs a uma distância de leitura até a conclusão.
 *
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

void setup()
{
    Serial.begin(9600); // Inicializa a comunicação serial com o PC
    while (!Serial)
        ;                              // Não faz nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();                       // Inicializa o barramento SPI
    mfrc522.PCD_Init();                // Inicializa o módulo MFRC522
    delay(4);                          // Atraso opcional. Alguns módulos precisam de mais tempo após a inicialização para estar pronto, consulte o Leia-me
    mfrc522.PCD_DumpVersionToSerial(); // Mostra os detalhes do PCD - Leitor de Cartões MFRC522
    Serial.println(F("Aproxime um PICC para ver UID, SAK, tipo e blocos de dados..."));
}

void loop()
{
    // Redefine o loop se nenhum novo cartão estiver presente no sensor/leitor. Isso economiza todo o processo quando ocioso.
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Seleciona um dos cartões
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    // Mostra informações de depuração sobre o cartão; PICC_HaltA() é chamado automaticamente
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
