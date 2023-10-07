// Bibliotecas para o Módulo RFID
#include <SPI.h>
#include <MFRC522.h>

// Biblioteca para o Display I2C
#include <LiquidCrystal_I2C.h>

#define endereco 0x27 // Endereços comuns: 0x27, 0x3F
#define colunas 16
#define linhas 2

LiquidCrystal_I2C lcd(endereco, colunas, linhas); // INSTANCIANDO OBJETOS

#define verde 2
#define vermelho 3
#define SS_PIN 10
#define RST_PIN 9

String tagID = "";
bool acessoliberado = false;

// ALTERE ESSES VALORES PARA OS DA SUA TAG/CARTÃO
String TagsLiberadas[] = {"ID1", "ID2"};
MFRC522 RFID(SS_PIN, RST_PIN);

void setup()
{
    Serial.begin(9600); // Comunicação Serial
    // lcd.init();         // Inicia a Comunicação no Display
    // lcd.backlight();    // Liga a Iluminação no Display
    SPI.begin();     // Comunicação SPI
    RFID.PCD_Init(); // Leitor RFID
    pinMode(verde, OUTPUT);
    pinMode(vermelho, OUTPUT);
    // lcd.print(" Aproxime a TAG  ");
}

void loop()
{
    // Verifica se existem tags/cartões
    if (!RFID.PICC_IsNewCardPresent() || !RFID.PICC_ReadCardSerial())
    {
        return;
    }

    tagID = "";

    // Aqui será feita a coleta das tags pela função uid
    for (byte i = 0; i < RFID.uid.size; i++)
    {
        tagID.concat(String(RFID.uid.uidByte[i], HEX));
    }

    // Aqui será feita a comparação das Tags analisadas com as cadastradas
    for (int i = 0; i < (sizeof(TagsLiberadas) / sizeof(String)); i++)
    {
        if (tagID.equalsIgnoreCase(TagsLiberadas[i]))
        {
            acessoliberado = true; // Libera tag
        }
    }
    if (acessoliberado == true)
    {
        Liberado();
        Serial.println("Acesso Liberado para a TAG: " + tagID);
        lcd.clear();
        lcd.print(" Acesso Liberado  ");
        delay(5000);
        lcd.clear();
        lcd.print(" Aproxime a TAG  ");
    }
    else
    {
        Negado();
        Serial.println("Acesso Negado para a TAG: " + tagID);
        lcd.clear();
        lcd.print(" Acesso Negado  ");
        delay(5000);
        lcd.clear();
        lcd.print(" Aproxime a TAG  ");
    }
    delay(1000);
}

void Liberado()
{
    int avisos = 1;
    for (int j = 0; j < avisos; j++)
    {
        digitalWrite(verde, HIGH);
        delay(3000);
        digitalWrite(verde, LOW);
        delay(1000);
    }
    acessoliberado = false;
}

void Negado()
{
    int avisos = 2;
    for (int j = 0; j < avisos; j++)
    {

        digitalWrite(vermelho, HIGH);
        delay(3000);
        digitalWrite(vermelho, LOW);
        delay(1000);
    }
}