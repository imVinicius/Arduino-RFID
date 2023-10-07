/*
-> Um CRC (Cyclic Redundancy Check)
    é uma forma simples de verificar se os dados foram alterados ou corrompidos.
-> Este exemplo calcula um valor CRC diretamente nos valores da EEPROM.
-> Este CRC funciona como uma assinatura e qualquer alteração no CRC
    calculado significa uma alteração nos dados armazenados
-> O objetivo deste exemplo é destacar como o objeto EEPROM 
    pode ser usado da mesma forma que um array.
*/

/***

    Escrito por Christopher Andrews.

    Algoritmo CRC gerado pelo pycrc, licença MIT (https://github.com/tpircher/pycrc).

    Um CRC é uma maneira simples de verificar se os dados foram alterados ou corrompidos.

    Este exemplo calcula um valor CRC diretamente nos valores da EEPROM.

    O objetivo deste exemplo é destacar como o objeto EEPROM pode ser usado da mesma forma que um array.

***/



#include <Arduino.h>
#include <EEPROM.h>

void setup()
{

    // Iniciar comunicação serial

    Serial.begin(9600);

    while (!Serial)
    {

        ; // aguardar a conexão da porta serial. Necessário apenas para portas USB nativas
    }

    // Imprimir o comprimento dos dados a serem usados no cálculo do CRC.

    Serial.print("Comprimento da EEPROM: ");

    Serial.println(EEPROM.length());

    // Imprimir o resultado da chamada a eeprom_crc()

    Serial.print("CRC32 dos dados da EEPROM: 0x");

    Serial.println(eeprom_crc(), HEX);

    Serial.print("\n\nConcluído!");
}

void loop()
{

    /* Loop vazio */
}

unsigned long eeprom_crc(void)
{

    const unsigned long crc_table[16] = {

        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,

        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,

        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,

        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c

    };

    unsigned long crc = ~0L;

    for (int index = 0; index < EEPROM.length(); ++index)
    {

        crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);

        crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);

        crc = ~crc;
    }

    return crc;
}
/*
Este código realiza um cálculo de CRC (Cyclic Redundancy Check) nos valores da EEPROM do Arduino
    -> verificando se os dados foram alterados ou corrompidos. 
O código utiliza a biblioteca EEPROM do Arduino
    -> e inclui um algoritmo CRC predefinido para realizar esse cálculo. 
Ele também imprime os resultados na porta serial para visualização.
*/