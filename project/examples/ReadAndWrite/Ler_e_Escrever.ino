/**
 * ----------------------------------------------------------------------------
 * Este é um exemplo da biblioteca MFRC522; veja https://github.com/miguelbalboa/rfid
 * para mais detalhes e outros exemplos.
 *
 * NOTA: O arquivo de biblioteca MFRC522.h contém muitas informações úteis. Por favor, leia-o.
 *
 * Liberado para o domínio público.
 * ----------------------------------------------------------------------------
 * Este exemplo mostra como ler e gravar blocos de dados em um PICC MIFARE Classic
 * (= cartão/etiqueta).
 *
 * CUIDADO: Os dados serão gravados no PICC, no setor #1 (blocos #4 a #7).
 *
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

#define RST_PIN 9 // Configurável, veja o layout de pinos típico acima
#define SS_PIN 10 // Configurável, veja o layout de pinos típico acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crie uma instância MFRC522.

MFRC522::MIFARE_Key key;

/**
 * Inicialização.
 */
void setup()
{
    Serial.begin(9600); // Inicialize a comunicação serial com o PC
    while (!Serial)
        ;               // Não faça nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();        // Inicialize o barramento SPI
    mfrc522.PCD_Init(); // Inicialize o cartão MFRC522

    // Prepare a chave (usada tanto como chave A quanto como chave B)
    // usando FFFFFFFFFFFFh que é o padrão na entrega do chip de fábrica
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Escaneie um PICC MIFARE Classic para demonstrar leitura e gravação."));
    Serial.print(F("Usando a chave (para A e B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();

    Serial.println(F("CUIDADO: Os dados serão gravados no PICC, no setor #1"));
}

/**
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

    // Mostre alguns detalhes do PICC (ou seja: a etiqueta/cartão)
    Serial.print(F("UID do Cartão:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("Tipo PICC: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Verifique a compatibilidade
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Este exemplo só funciona com cartões MIFARE Classic."));
        return;
    }

    // Neste exemplo, usamos o segundo setor,
    // ou seja, setor #1, abrangendo os blocos #4 a #7
    byte setor = 1;
    byte blocoAddr = 4;
    byte dataBlock[] = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
        0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
    };
    byte blocoTrailer = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte tamanho = sizeof(buffer);

    // Autenticar usando chave A
    Serial.println(F("Autenticando usando chave A..."));
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blocoTrailer, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Falha em PCD_Authenticate(): "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Mostre o setor inteiro como está atualmente
    Serial.println(F("Dados atuais no setor:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, setor);
    Serial.println();

    // Leia dados do bloco
    Serial.print(F("Lendo dados do bloco "));
    Serial.print(blocoAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blocoAddr, buffer, &tamanho);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Falha em MIFARE_Read(): "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Dados no bloco "));
    Serial.print(blocoAddr);
    Serial.println(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();
    Serial.println();

    // Autenticar usando chave B
    Serial.println(F("Autenticando novamente usando chave B..."));
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blocoTrailer, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Falha em PCD_Authenticate(): "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Escrever dados no bloco
    Serial.print(F("Escrevendo dados no bloco "));
    Serial.print(blocoAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16);
    Serial.println();
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blocoAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Falha em MIFARE_Write(): "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();

    // Leia dados do bloco (novamente, deve ser o que escrevemos)
    Serial.print(F("Lendo dados do bloco "));
    Serial.print(blocoAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blocoAddr, buffer, &tamanho);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Falha em MIFARE_Read(): "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Dados no bloco "));
    Serial.print(blocoAddr);
    Serial.println(F(":"));
    dump_byte_array(buffer, 16);
    Serial.println();

    // Verifique se os dados no bloco são iguais ao que escrevemos
    // contando o número de bytes iguais
    Serial.println(F("Verificando o resultado..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++)
    {
        // Compare o buffer (= o que lemos) com dataBlock (= o que escrevemos)
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Número de bytes que coincidem = "));
    Serial.println(count);
    if (count == 16)
    {
        Serial.println(F("Sucesso :-)"));
    }
    else
    {
        Serial.println(F("Falha, sem correspondência :-("));
        Serial.println(F("  talvez a gravação não tenha funcionado corretamente..."));
    }
    Serial.println();

    // Desative o PICC
    mfrc522.PICC_HaltA();
    // Pare a criptografia no PCD
    mfrc522.PCD_StopCrypto1();
}

/**
 * Rotina auxiliar para exibir uma matriz de bytes como valores hexadecimais no Serial.
 */
void dump_byte_array(byte *buffer, byte tamanho)
{
    for (byte i = 0; i < tamanho; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
