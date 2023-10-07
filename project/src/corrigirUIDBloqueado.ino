/*
 * --------------------------------------------------------------------------------------------------------------------
 * Exemplo de esboço/programa para consertar cartões MIFARE com UID mutável quebrados
 * --------------------------------------------------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; para mais detalhes e outros exemplos, veja: https://github.com/miguelbalboa/rfid
 *
 * Este exemplo mostra como consertar cartões MIFARE com UID mutável quebrados que têm um setor 0 corrompido.
 *
 * @author Tom Clement
 * @license Liberado para o domínio público.
 *
 * Layout de pinagem típico usado:
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
 * Mais layouts de pinagem para outras placas podem ser encontrados aqui: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout de pinagem típico acima
#define SS_PIN 10 // Configurável, veja o layout de pinagem típico acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crie uma instância do MFRC522

MFRC522::MIFARE_Key key;

void setup()
{
    Serial.begin(9600); // Inicialize a comunicação serial com o PC
    while (!Serial)
        ;               // Não faça nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();        // Inicialize o barramento SPI
    mfrc522.PCD_Init(); // Inicialize o cartão MFRC522
    Serial.println(F("Aviso: este exemplo apaga o UID do seu cartão Mifare, use com cuidado!"));

    // Prepare a chave - todas as chaves são definidas como FFFFFFFFFFFFh na entrega da fábrica.
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }
}

void loop()
{
    if (mfrc522.MIFARE_UnbrickUidSector(false))
    {
        Serial.println(F("Setor 0 apagado, defina o UID como 1234. O cartão deve estar responsivo novamente agora."));
    }
    delay(1000);
}
// Certifique-se de entender que este exemplo apaga o UID do seu cartão Mifare
// e define-o como "1234".
// Use com cuidado!