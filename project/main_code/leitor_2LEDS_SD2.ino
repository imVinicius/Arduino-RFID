#include <SPI.h>
#include <MFRC522.h>
#include <SD.h> // Inclui a biblioteca SD.h

#define chipSelect 4 // Define o pino CS do cartão SD
#define SS_PIN 10    // está conectado como pino Slave Select (SS) em uma comunicação SPI (Serial Peripheral Interface)
#define RST_PIN 9    // o pino 9 do Arduino está sendo usado como o pino de reset para o módulo MFRC522, leitor de cartão RFID
#define VERDE 8      // Pino do Led verde
#define VERMELHO 7   // Pino do LED vermelho

MFRC522 rfid(SS_PIN, RST_PIN);
File arquivo; // Declaração global do objeto de arquivo

// Defina as chaves permitidas como uma matriz de strings
String chavesPermitidas[] = {
    "20:6B:18:83",
    // Adicione outras chaves permitidas aqui
};

void setup()
{
    // Inicializa a comunicação serial
    Serial.begin(9600);

    // Inicializa o SPI
    SPI.begin();

    // Inicializa o módulo RFID
    rfid.PCD_Init();

    // Configura o pino do LED verde como saída
    pinMode(VERDE, OUTPUT);

    // Configura o pino do LED vermelho como saída
    pinMode(VERMELHO, OUTPUT);

    // Inicializa a comunicação com o cartão SD com tratamento de exceções
    try
    {
        if (!SD.begin(chipSelect))
        {
            throw "Falha ao inicializar o cartão SD";
        }

        // Abre o arquivo no cartão SD uma vez no início do programa
        arquivo = SD.open("alunos.txt", FILE_WRITE);
        if (!arquivo)
        {
            throw "Erro ao abrir o arquivo no cartão SD.";
        }
    }
    catch (const char *error)
    {
        Serial.println(error);
        while (true)
        {
            // Fica em um loop infinito em caso de erro crítico
        }
    }
}

void loop()
{
    // Verifica se há um novo cartão
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return;

    // Obtém o tipo do cartão
    MFRC522::PICC_Type tipoPICC = rfid.PICC_GetType(rfid.uid.sak);

    // Verifica se o cartão é do tipo MIFARE Classic
    if (tipoPICC != MFRC522::PICC_TYPE_MIFARE_MINI &&
        tipoPICC != MFRC522::PICC_TYPE_MIFARE_1K &&
        tipoPICC != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Seu cartão não é do tipo MIFARE Classic."));
        return;
    }

    // Converte o UID do cartão para uma string !!
    String strID = "";
    for (byte i = 0; i < 4; i++)
    {
        strID +=
            (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX) + (i != 3 ? ":" : "");
    }

    strID.toUpperCase();
    Serial.print("Aproxime o cartão chave: ");
    Serial.println(strID);

    // Verifica se o ID lido está na lista de chaves permitidas
    bool chaveAutorizada = false;
    for (int i = 0; i < sizeof(chavesPermitidas) / sizeof(chavesPermitidas[0]); i++)
    {
        if (strID == chavesPermitidas[i])
        {
            chaveAutorizada = true;
            break;
        }
    }

    // Grava o ID no cartão SD com tratamento de exceções
    try
    {
        if (arquivo)
        {
            arquivo.println(strID);
            Serial.println("ID gravado no cartão SD.");
        }
        else
        {
            throw "Erro ao escrever no arquivo no cartão SD.";
        }
    }
    catch (const char *error)
    {
        Serial.println(error);
    }

    // Verifica se a chave é autorizada e atua com base nisso
    if (chaveAutorizada)
    {
        digitalWrite(VERDE, HIGH);   // Liga o LED verde
        digitalWrite(VERMELHO, LOW); // Desliga o LED vermelho
    }
    else
    {
        digitalWrite(VERDE, LOW);     // Desliga o LED verde
        digitalWrite(VERMELHO, HIGH); // Liga o LED vermelho
    }

    // Para a comunicação com o cartão
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
