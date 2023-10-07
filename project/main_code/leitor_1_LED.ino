#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_PIN 8

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key chave;

void setup()
{
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return;

    // Serial.print(F("Tipo PICC: "));
    MFRC522::PICC_Type tipoPICC = rfid.PICC_GetType(rfid.uid.sak);
    // Serial.println(rfid.PICC_GetTypeName(tipoPICC));
    // Verifica se o PICC é do tipo MIFARE Classic
    if (tipoPICC != MFRC522::PICC_TYPE_MIFARE_MINI &&
        tipoPICC != MFRC522::PICC_TYPE_MIFARE_1K &&
        tipoPICC != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Seu cartão não é do tipo MIFARE Classic."));
        return;
    }

    String strID = "";
    for (byte i = 0; i < 4; i++)
    {
        strID += (rfid.uid.uidByte[i] < 0x10 ? "0" : "")
         + String(rfid.uid.uidByte[i], HEX)
         + (i != 3 ? ":" : "");
    }

    strID.toUpperCase();
    Serial.print("Aproxime o cartão-chave: ");
    Serial.println(strID);

    if (strID.indexOf("20:6B:18:83") >= 0)
    {
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
