/*
 * Este exemplo demonstra como você pode autenticar-se com os NTAG213, 215, 216. Por padrão, as tags não estão protegidas e, para protegê-las, precisamos escrever 4 valores diferentes:
 * Usando mfrc522.MIFARE_Ultralight_Write(PáginaNum, Dados, #Databytes))
 * 1.- Precisamos escrever a senha de 32 bits na página 0xE5! Para o NTAG 213 e 215, a página é diferente, consulte a documentação da NXP!
 * 2.- Agora escreva os 16 bits pACK na página 0xE6, use os 2 bytes mais significativos assim: pACKH + pACKL + 00 + 00, após uma autenticação, a tag retornará esses bytes secretos
 * 3.- Agora precisamos escrever a primeira página que queremos proteger, que é um dado de 1 byte na página 0xE3, precisamos escrever 00 + 00 + 00 + primeiraPágina, todas as páginas após esta estarão protegidas contra escrita
 *     Agora a PROTEÇÃO de escrita está ATIVADA, então precisamos nos autenticar para escrever os últimos dados
 * 4.- Finalmente, precisamos escrever um registro de acesso para PROTEGER a leitura do cartão, esta etapa é opcional, apenas se você quiser proteger também a leitura, escreva 80 + 00 + 00 + 00 na página 0xE4
 * Após concluir todas essas etapas, você precisará autenticar-se primeiro para ler ou escrever em qualquer página após a primeira página que você selecionou para proteger.
 * Para desativar a proteção, basta escrever na página (0xE3) 00 + 00 + 00 + FF, que removerá toda a proteção.
 *
 * @author GARGANTUA de RoboCreators.com & paradoxalabs.com
 * @license Liberado para o domínio público.
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Pino de reset
#define SS_PIN 10 // Pino SS

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância MFRC522

void setup()
{
    Serial.begin(9600); // Inicializa a comunicação serial com o PC
    while (!Serial)
        ;               // Não faça nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();        // Inicializa o barramento SPI
    mfrc522.PCD_Init(); // Inicializa o MFRC522
    Serial.println(F("Escaneie PICC para ver UID, tipo e blocos de dados..."));
}

void loop()
{
    // Reinicia o loop se nenhum novo cartão estiver presente no sensor/leitor. Isso economiza o processo inteiro quando estiver inativo.
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return;
    }

    // Seleciona um dos cartões
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return;
    }

    byte PSWBuff[] = {0xFF, 0xFF, 0xFF, 0xFF}; // Senha de 32 bits padrão FFFFFFFF.
    byte pACK[] = {0, 0};                      // ACK da senha de 16 bits retornado pelo NFCtag.

    Serial.print("Autenticação: ");
    Serial.println(mfrc522.PCD_NTAG216_AUTH(&PSWBuff[0], pACK)); // Solicita autenticação; se retornar STATUS_OK, estamos prontos.

    // Imprimir PassWordACK
    Serial.print(pACK[0], HEX);
    Serial.println(pACK[1], HEX);

    byte WBuff[] = {0x00, 0x00, 0x00, 0x04};
    byte RBuff[18];

    // Serial.print("CHG BLK: ");
    // Serial.println(mfrc522.MIFARE_Ultralight_Write(0xE3, WBuff, 4));  // Como escrever em uma página.

    mfrc522.PICC_DumpMifareUltralightToSerial(); // Este é um dump modificador; basta alterar o círculo "for" para < 232 em vez de < 16 para ver todas as páginas no NTAG216.

    delay(3000);
}
