/*
 * Gravação na EEPROM
 *
 * Armazena valores lidos da entrada analógica 0 na EEPROM.
 * Esses valores permanecerão na EEPROM quando a placa for
 * desligada e podem ser recuperados posteriormente por outro esboço.
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout típico dos pinos acima
#define SS_PIN 10 // Configurável, veja o layout típico dos pinos acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância MFRC522.

byte buffer[18];
byte bloco;
byte dados[64][16];
MFRC522::StatusCode status;

MFRC522::MIFARE_Key chave;

// Número de chaves padrão conhecidas (codificadas em duro)
// NOTA: Sincronize a definição NR_KNOWN_KEYS com a matriz defaultKeys[]
#define NR_KNOWN_KEYS 8
// Chaves conhecidas, consulte: https://code.google.com/p/mfcuk/wiki/MifareClassicDefaultKeys
byte chavesConhecidas[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] = {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF = padrão de fábrica
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};

char escolha;
/*
 * Inicialização.
 */
void setup()
{
  Serial.begin(9600); // Inicializa comunicação serial com o PC
  while (!Serial)
    ;                 // Não faz nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
  SPI.begin();        // Inicializa barramento SPI
  mfrc522.PCD_Init(); // Inicializa o cartão MFRC522
  Serial.println(F("Tente as chaves padrão mais usadas para ler o bloco 0 a 63 de um MIFARE PICC."));
  Serial.println("1.Ler cartão \n2.Escrever no cartão \n3.Copiar os dados.");

  for (byte i = 0; i < 6; i++)
  {
    chave.keyByte[i] = 0xFF;
  }
}

void exibir_array_de_bytes(byte *buffer, byte tamanhoBuffer)
{
  for (byte i = 0; i < tamanhoBuffer; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
// Lê bytes do buffer e exibe como ASCII no monitor serial

void exibir_array_de_bytes_como_ascii(byte *buffer, byte tamanhoBuffer)
{
  for (byte i = 0; i < tamanhoBuffer; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.write(buffer[i]);
  }
}

/*
 * Tenta usar o PICC (o tag/cartão) com a chave fornecida para acessar o bloco 0 a 63.
 * Em caso de sucesso, mostrará os detalhes da chave e exibirá os dados do bloco no Serial.
 *
 * @return true quando a chave fornecida funcionar, false caso contrário.
 */

bool tentar_chave(MFRC522::MIFARE_Key *chave)
{
  bool resultado = false;

  for (byte bloco = 0; bloco < 64; bloco++)
  {

    // Serial.println(F("Autenticando usando chave A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, chave, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print(F("Falha em PCD_Authenticate(): "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return false;
    }

    // Ler bloco
    byte contagemBytes = sizeof(buffer);
    status = mfrc522.MIFARE_Read(bloco, buffer, &contagemBytes);
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print(F("Falha em MIFARE_Read(): "));
      Serial.println(mfrc522.GetStatusCodeName(status));
    }
    else
    {
      // Leitura bem-sucedida
      resultado = true;
      Serial.print(F("Sucesso com a chave:"));
      dump_byte_array((*chave).keyByte, MFRC522::MF_KEY_SIZE);
      Serial.println();

      // Dump dos dados do bloco
      Serial.print(F("Bloco "));
      Serial.print(bloco);
      Serial.print(F(":"));
      dump_byte_array1(buffer, 16); // converter de hexadecimal para ASCII
      Serial.println();

      for (int p = 0; p < 16; p++) // Ler os 16 bits do bloco
      {
        dados[bloco][p] = buffer[p];
        Serial.print(dados[bloco][p]);
        Serial.print(" ");
      }
    }
  }
  Serial.println();

  Serial.println("1.Ler cartão \n2.Escrever no cartão \n3.Copiar os dados.");

  mfrc522.PICC_HaltA();      // Parar PICC
  mfrc522.PCD_StopCrypto1(); // Parar criptografia no PCD
  return resultado;

  start();
}

/*
 * Loop principal.
 */
void loop()
{
  iniciar();
}

void iniciar()
{
  choice = Serial.read();

  if (choice == '1')
  {
    Serial.println("Ler o cartão");
    escolha1();
  }
  else if (choice == '2')
  {
    Serial.println("Verificar o que está nas variáveis");
    escolha2();
  }
  else if (choice == '3')
  {
    Serial.println("Copiar os dados para o novo cartão");
    escolha3();
  }
}

void escolha2()
{ // Testar valores nos blocos

  for (block = 4; block <= 62; block++)
  {
    if (block == 7 || block == 11 || block == 15 || block == 19 || block == 23 || block == 27 || block == 31 || block == 35 || block == 39 || block == 43 || block == 47 || block == 51 || block == 55 || block == 59)
    {
      block++;
    }

    Serial.print(F("Escrevendo dados no bloco "));
    Serial.print(block);
    Serial.println("\n");

    for (int j = 0; j < 16; j++)
    {
      Serial.print(dados[block][j]);
      Serial.print(" ");
    }
    Serial.println("\n");
  }

  Serial.println("1. Ler o cartão \n2. Escrever no cartão \n3. Copiar os dados.");
  iniciar();
}

void escolha3()
{ // Copiar os dados no novo cartão
  Serial.println("Insira o novo cartão...");

  // Procurar por novos cartões
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // Selecionar um dos cartões
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  // Mostrar alguns detalhes do PICC (ou seja, o tag/cartão)
  Serial.print(F("UID do Cartão:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("Tipo PICC: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Tentar as chaves padrão conhecidas
  /*MFRC522::MIFARE_Key key;
  for (byte k = 0; k < NR_KNOWN_KEYS; k++) {
    // Copiar a chave conhecida para a estrutura MIFARE_Key
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
      key.keyByte[i] = knownKeys[k][i];
    }
  }*/
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  for (int i = 4; i <= 62; i++)
  { // Copiar os blocos 4 até 62, excluindo os blocos mencionados (pois são blocos de autenticação)
    if (i == 7 || i == 11 || i == 15 || i == 19 || i == 23 || i == 27 || i == 31 || i == 35 || i == 39 || i == 43 || i == 47 || i == 51 || i == 55 || i == 59)
    {
      i++;
    }
    block = i;

    // Autenticar usando a chave A
    Serial.println(F("Autenticando usando chave A..."));
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print(F("PCD_Authenticate() falhou: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Autenticar usando a chave B
    Serial.println(F("Autenticando novamente usando chave B..."));
    status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print(F("PCD_Authenticate() falhou: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Escrever dados no bloco
    Serial.print(F("Escrevendo dados no bloco "));
    Serial.print(block);
    Serial.println("\n");

    dump_byte_array(dados[block], 16);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(block, dados[block], 16);
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print(F("MIFARE_Write() falhou: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
    }

    Serial.println("\n");
  }

  mfrc522.PICC_HaltA();      // Encerrar o PICC
  mfrc522.PCD_StopCrypto1(); // Encerrar a criptografia no PCD

  Serial.println("1. Ler o cartão \n2. Escrever no cartão \n3. Copiar os dados.");
  iniciar();
}

void escolha1()
{ // Ler cartão
  Serial.println("Insira o cartão...");
  // Procurar por novos cartões
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // Selecionar um dos cartões
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  // Mostrar alguns detalhes do PICC (ou seja, o tag/cartão)
  Serial.print(F("UID do Cartão:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("Tipo PICC: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Tentar as chaves padrão conhecidas
  MFRC522::MIFARE_Key key;
  for (byte k = 0; k < NR_KNOWN_KEYS; k++)
  {
    // Copiar a chave conhecida para a estrutura MIFARE_Key
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
    {
      key.keyByte[i] = knownKeys[k][i];
    }
    // Tentar a chave
    if (tentar_chave(&key))
    {
      // Chave encontrada e relatada juntamente com o bloco,
      // não é necessário tentar outras chaves para este PICC
      break;
    }
  }
}
