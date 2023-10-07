#include <SPI.h>
#include <MFRC522.h>
#include <SD.h> // Inclui a biblioteca SD.h


#define chipSelect 4 // Define o pino CS do cartão SD
#define SS_PIN 10  // está conectado como pino Slave Select (SS) em uma comunicação SPI (Serial Peripheral Interface)
#define RST_PIN 9  // o pino 9 do Arduino está sendo usado como o pino de reset para o módulo MFRC522, leitor de cartão RFID
#define VERDE 8    // Pino do Led verde
#define VERMELHO 7 // Pino do LED vermelho

MFRC522 rfid(SS_PIN, RST_PIN);
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

    // Inicializa a comunicação com o cartão SD
    if (!SD.begin(chipSelect))
    {
        Serial.println("Falha ao inicializar o cartão SD");
        return;
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
            (rfid.uid.uidByte[i] < 0x10 ? "0" : "")
            + String(rfid.uid.uidByte[i], HEX)
            + (i != 3 ? ":" : "");
    }

    strID.toUpperCase();
    Serial.print("Aproxime o cartão chave: ");
    Serial.println(strID);

    // Grava o ID no cartão SD
    File arquivo = SD.open("alunos.txt", FILE_WRITE);
    if (arquivo)
    {
        arquivo.println(strID);
        arquivo.close();
        Serial.println("ID gravado no cartão SD.");
    }
    else
    {
        Serial.println("Erro ao abrir o arquivo no cartão SD.");
    }

    // Verifica se o cartão é a chave correta
    if (strID.indexOf("20:6B:18:83") >= 0)
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

/*
Este código abrirá um arquivo chamado "alunos.txt"
no cartão SD e escreverá os IDs dos cartões RFID nele.
Certifique-se de que o cartão SD
esteja formatado corretamente e inserido no leitor antes de executar o código.
*/
