/**
 * ----------------------------------------------------------------------------
 * Este é um exemplo de biblioteca MFRC522; veja https://github.com/miguelbalboa/rfid
 * para mais detalhes e outros exemplos.
 *
 * NOTA: O arquivo de biblioteca MFRC522.h possui muitas informações úteis. Por favor, leia-o.
 *
 * Liberado no domínio público.
 * ----------------------------------------------------------------------------
 * Este exemplo demonstra como configurar blocos em um PICC MIFARE Classic (= cartão/etiqueta)
 * no modo "Bloco de Valor": neste modo, as operações de Incremento/Decremento,
 * Restauração e transferência podem ser usadas.
 *
 * ATENÇÃO: Os dados serão gravados no PICC, no setor #1 (blocos #4 a #7).
 *
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
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // Configurável, veja o layout de pinos típico acima
#define SS_PIN 10 // Configurável, veja o layout de pinos típico acima

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria uma instância MFRC522.

MFRC522::MIFARE_Key key;

/**
 * Inicialização.
 */
void setup()
{
    Serial.begin(9600); // Inicializa a comunicação serial com o PC
    while (!Serial)
        ;               // Não faça nada se nenhuma porta serial estiver aberta (adicionado para Arduinos baseados no ATMEGA32U4)
    SPI.begin();        // Inicializa o barramento SPI
    mfrc522.PCD_Init(); // Inicializa o cartão MFRC522

    // Prepara a chave (usada tanto como chave A quanto como chave B)
    // usando FFFFFFFFFFFFh, que é o valor padrão de fábrica
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Passe um PICC MIFARE Classic para demonstrar o modo de Bloco de Valor."));
    Serial.print(F("Usando chave (para A e B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();

    Serial.println(F("CUIDADO: Os dados serão gravados no PICC, no setor #1"));
}

/**
 * Loop principal.
 */
void loop()
{
    // Reinicia o loop se nenhum novo cartão estiver presente no sensor/leitor. Isso economiza todo o processo quando estiver ocioso.
    if (!mfrc522.PICC_IsNewCardPresent())
        return;

    // Seleciona um dos cartões
    if (!mfrc522.PICC_ReadCardSerial())
        return;

    // Mostra alguns detalhes do PICC (ou seja, o cartão)
    Serial.print(F("UID do Cartão:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("Tipo de PICC: "));
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
    byte blocoValorA = 5;
    byte blocoValorB = 6;
    byte blocoTrailer = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte tamanho = sizeof(buffer);
    int32_t valor;

    // Autentica usando chave A
    Serial.println(F("Autenticando usando chave A..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blocoTrailer, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Mostra o setor inteiro conforme ele está atualmente
    Serial.println(F("Dados atuais no setor:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, setor);
    Serial.println();

    // Precisamos de um trailer de setor que defina os blocos 5 e 6 como Blocos de Valor e habilite a chave B
    // O último bloco em um setor (bloco #3 para Mifare Classic 1K) é o Trailer do Setor.
    // Veja http://www.nxp.com/documents/data_sheet/MF1S503x.pdf seções 8.6 e 8.7:
    //      Bytes 0-5:   Chave A
    //      Bytes 6-8:   Bits de Acesso
    //      Bytes 9:     Dados do usuário
    //      Bytes 10-15: Chave B (ou dados do usuário)
    byte trailerBuffer[] = {
        255, 255, 255, 255, 255, 255, // Mantenha a chave A padrão
        0, 0, 0,
        0,
        255, 255, 255, 255, 255, 255}; // Mantenha a chave B padrão
    // Os bits de acesso são armazenados de forma peculiar.
    // Existem quatro grupos:
    //      g[0]    Bits de acesso para bloco 0 (para setores 0-31)
    //              ou blocos 0-4 (para setores 32-39)
    //      g[1]    Bits de acesso para bloco 1 (para setores 0-31)
    //              ou blocos 5-9 (para setores 32-39)
    //      g[2]    Bits de acesso para bloco 2 (para setores 0-31)
    //              ou blocos 10-14 (para setores 32-39)
    //      g[3]    Bits de acesso para o Trailer do Setor: bloco 3 (para setores 0-31)
    //              ou bloco 15 (para setores 32-39)
    // Cada grupo possui bits de acesso [C1 C2 C3], neste código C1 é o MSB e C3 é o LSB.
    // Determine o padrão de bits necessário usando MIFARE_SetAccessBits:
    //      g0=0    bits de acesso para o bloco 0 (deste setor) usando [0 0 0] = 000b = 0
    //              o que significa que a chave A|B têm r/w para o bloco 0 deste setor
    //              que (neste exemplo) se traduz para o bloco #4 dentro do setor #1;
    //              esta é a configuração de transporte (na entrega de fábrica).
    //      g1=6    bits de acesso para o bloco 1 (deste setor) usando [1 1 0] = 110b = 6
    //              o que significa que o bloco 1 (deste setor) é usado como bloco de valor,
    //              que (neste exemplo) se traduz para o bloco #5 dentro do setor #1;
    //              onde a chave A|B têm r, a chave B tem w, a chave B pode incrementar,
    //              e a chave A|B pode decrementar, transferir e restaurar.
    //      g2=6    mesma coisa para o bloco 2 (deste setor): defina-o como bloco de valor;
    //              que (neste exemplo) se traduz para o bloco #6 dentro do setor #1;
    //      g3=3    bits de acesso para o Trailer do Setor (bloco 3 deste setor): aqui;
    //              usando [0 1 1] = 011b = 3, o que significa que apenas a chave B tem acesso de leitura/escrita
    //              para o Trailer do Setor (bloco 3 deste setor) a partir de agora
    //              que (neste exemplo) se traduz para o bloco #7 dentro do setor #1;
    mfrc522.MIFARE_SetAccessBits(&trailerBuffer[6], 0, 6, 6, 3);

    // Lê o trailer do setor conforme ele está atualmente armazenado no PICC
    Serial.println(F("Lendo o trailer do setor..."));
    status = mfrc522.MIFARE_Read(blocoTrailer, buffer, &tamanho);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Read() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    // Verifica se ele corresponde ao padrão de acesso desejado;
    // porque se corresponder, não precisamos gravá-lo novamente...
    if (buffer[6] != trailerBuffer[6] || buffer[7] != trailerBuffer[7] || buffer[8] != trailerBuffer[8])
    {
        // Eles não correspondem (ainda), então escreva no PICC
        Serial.println(F("Escrevendo novo trailer do setor..."));
        status = mfrc522.MIFARE_Write(blocoTrailer, trailerBuffer, 16);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("MIFARE_Write() falhou: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }
    }

    // Autentica usando chave B
    Serial.println(F("Autenticando novamente usando chave B..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, blocoTrailer, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("PCD_Authenticate() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Um bloco de valor possui um valor de 32 bits armazenado três vezes
    // e um endereço de 8 bits armazenado 4 vezes. Certifique-se de que blocoValorA
    // e blocoValorB tenham esse formato (observe que ele somente formatará
    // o bloco quando ele não estiver em conformidade com o formato esperado já).
    formatValueBlock(blocoValorA);
    formatValueBlock(blocoValorB);

    // Adicione 1 ao valor de blocoValorA e armazene o resultado em blocoValorA.
    Serial.print("Adicionando 1 ao valor do bloco ");
    Serial.println(blocoValorA);
    status = mfrc522.MIFARE_Increment(blocoValorA, 1);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Increment() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    status = mfrc522.MIFARE_Transfer(blocoValorA);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Transfer() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    // Mostra o novo valor de blocoValorA
    status = mfrc522.MIFARE_GetValue(blocoValorA, &valor);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_GetValue() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    Serial.print("Novo valor do bloco de valor ");
    Serial.print(blocoValorA);
    Serial.print(" = ");
    Serial.println(valor);

    // Subtrai 10 do valor de blocoValorB e armazene o resultado em blocoValorB.
    Serial.print("Subtraindo 10 do valor do bloco ");
    Serial.println(blocoValorB);
    status = mfrc522.MIFARE_Decrement(blocoValorB, 10);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Decrement() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    status = mfrc522.MIFARE_Transfer(blocoValorB);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Transfer() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    // Mostra o novo valor de blocoValorB
    status = mfrc522.MIFARE_GetValue(blocoValorB, &valor);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_GetValue() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
    Serial.print(F("Novo valor do bloco de valor "));
    Serial.print(blocoValorB);
    Serial.print(F(" = "));
    Serial.println(valor);
    // Verifica algum limite...
    if (valor <= -100)
    {
        Serial.println(F("Abaixo de -100, então resetando para 255 = 0xFF só por diversão..."));
        status = mfrc522.MIFARE_SetValue(blocoValorB, 255);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("MIFARE_SetValue() falhou: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
        }
    }

    // Exibe os dados do setor
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, setor);
    Serial.println();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

/**
 * Rotina auxiliar para mostrar uma matriz de bytes como valores hexadecimais no Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Garante que um determinado bloco esteja formatado como um Bloco de Valor.
 */
void formatValueBlock(byte blocoAddr)
{
    byte buffer[18];
    byte tamanho = sizeof(buffer);
    MFRC522::StatusCode status;

    Serial.print(F("Lendo bloco "));
    Serial.println(blocoAddr);
    status = mfrc522.MIFARE_Read(blocoAddr, buffer, &tamanho);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("MIFARE_Read() falhou: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    if ((buffer[0] == (byte)~buffer[4]) && (buffer[1] == (byte)~buffer[5]) && (buffer[2] == (byte)~buffer[6]) && (buffer[3] == (byte)~buffer[7])

        && (buffer[0] == buffer[8]) && (buffer[1] == buffer[9]) && (buffer[2] == buffer[10]) && (buffer[3] == buffer[11])

        && (buffer[12] == (byte)~buffer[13]) && (buffer[12] == buffer[14]) && (buffer[12] == (byte)~buffer[15]))
    {
        Serial.println(F("O bloco possui o formato de Bloco de Valor correto."));
    }
    else
    {
        Serial.println(F("Formatando como Bloco de Valor..."));
        byte blocoValor[] = {
            0, 0, 0, 0,
            255, 255, 255, 255,
            0, 0, 0, 0,
            blocoAddr, ~blocoAddr, blocoAddr, ~blocoAddr};
        status = mfrc522.MIFARE_Write(blocoAddr, blocoValor, 16);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("MIFARE_Write() falhou: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
        }
    }
}
