/*
 * ----------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; consulte https://github.com/miguelbalboa/rfid
 * para mais detalhes e outros exemplos.
 *
 * NOTA: O arquivo de biblioteca MFRC522.h contém muitas informações úteis. Por favor, leia-o.
 *
 * Liberado para o domínio público.
 * ----------------------------------------------------------------------------
 * Exemplo de esboço/programa que tentará as chaves padrão mais usadas listadas em
 * https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys para ler o
 * bloco 0 de um cartão MIFARE RFID usando um leitor RFID-RC522.
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
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout típico dos pinos acima
#define SS_PIN 10 // Configurável, veja o layout típico dos pinos acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crie uma instância MFRC522.

// Número de chaves padrão conhecidas (codificadas)
// NOTA: Sincronize a definição NR_KNOWN_KEYS com a matriz defaultKeys[]
#define NR_KNOWN_KEYS 8
// Chaves conhecidas, veja: https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] = {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = padrão de fábrica
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};

/*
 * Inicialização.
 */
void setup()
{
    Serial.begin(9600); // Inicialize a comunicação serial com o PC
    while (!Serial)
        ;               // Não faça nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();        // Inicialize o barramento SPI
    mfrc522.PCD_Init(); // Inicialize o cartão MFRC522
    Serial.println(F("Tente as chaves padrão mais usadas para ler o bloco 0 de um MIFARE PICC."));
}

/*
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

/*
 * Tente usar o PICC (o tag/cartão) com a chave fornecida para acessar o bloco 0.
 * Em caso de sucesso, mostrará os detalhes da chave e os dados do bloco no Serial.
 *
 * @return true quando a chave fornecida funcionar, falso caso contrário.
 */
bool try_key(MFRC522::MIFARE_Key *key)
{
    bool result = false;
    byte buffer[18];
    byte block = 0;
    MFRC522::StatusCode status;

    // Serial.println(F("Autenticando usando chave A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        // Serial.print(F("PCD_Authenticate() falhou: "));
        // Serial.println(mfrc522.GetStatusCodeName(status));
        return false;
    }

    // Ler bloco
    byte byteCount = sizeof(buffer);
    status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522::STATUS_OK)
    {
        // Serial.print(F("MIFARE_Read() falhou: "));
        // Serial.println(mfrc522.GetStatusCodeName(status));
    }
    else
    {
        // Leitura bem-sucedida
        result = true;
        Serial.print(F("Sucesso com a chave:"));
        dump_byte_array((*key).keyByte, MFRC522::MF_KEY_SIZE);
        Serial.println();
        // Exibir dados do bloco
        Serial.print(F("Bloco "));
        Serial.print(block);
        Serial.print(F(":"));
        dump_byte_array(buffer, 16);
        Serial.println();
    }
    Serial.println();

    mfrc522.PICC_HaltA();      // Interromper PICC
    mfrc522.PCD_StopCrypto1(); // Parar a criptografia no PCD
    return result;
}

/*
 * Loop principal.
 */
void loop()
{
    // Reinicie o loop se nenhum novo cartão estiver presente no sensor/leitor. Isso economiza o processo inteiro quando estiver inativo.
    if (!mfrc522.PICC_IsNewCardPresent())
        return;

    // Selecione um dos cartões
    if (!mfrc522.PICC_ReadCardSerial())
        return;

    // Mostre alguns detalhes do PICC (ou seja, o tag/cartão)
    Serial.print(F("UID do Cartão:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("Tipo do PICC: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Tente as chaves padrão conhecidas
    MFRC522::MIFARE_Key key;
    for (byte k = 0; k < NR_KNOWN_KEYS; k++)
    {
        // Copie a chave conhecida na estrutura MIFARE_Key
        for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
        {
            key.keyByte[i] = knownKeys[k][i];
        }
        // Tente a chave
        if (try_key(&key))
        {
            // Chave encontrada e relatada, não é necessário tentar outras chaves para este PICC
            break;
        }

        // http://arduino.stackexchange.com/a/14316
        if (!mfrc522.PICC_IsNewCardPresent())
            break;
        if (!mfrc522.PICC_ReadCardSerial())
            break;
    }
}
