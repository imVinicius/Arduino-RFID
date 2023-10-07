/*
Este exemplo ilustra como armazenar valores lidos da entrada analógica 0 na EEPROM
 usando a função EEPROM.write(). Esses valores permanecerão na
  EEPROM quando a placa for desligada e 
        podem ser recuperados posteriormente por outro esboço.

*/
/*

 * Gravação na EEPROM

 *

 * Armazena os valores lidos da entrada analógica 0 na EEPROM.

 * Esses valores permanecerão na EEPROM quando a placa for

 * desligada e podem ser recuperados posteriormente por outro esboço.

 */

#include <EEPROM.h>

/** o endereço atual na EEPROM (ou seja, qual byte escreveremos em seguida) **/
int endereco = 0;

void setup()
{

    /** Configuração vazia. **/
}

void loop()
{

    /***

      Precisamos dividir por 4 porque as entradas analógicas variam de

      0 a 1023 e cada byte da EEPROM só pode conter um

      valor de 0 a 255.

    ***/

    int val = analogRead(0) / 4;

    /***

      Escreve o valor no byte apropriado da EEPROM.

      esses valores permanecerão lá quando a placa for

      desligada.

    ***/

    EEPROM.write(endereco, val);

    /***

      Avança para o próximo endereço, quando chegar ao final, recomeça do início.

      Processadores AVR maiores têm tamanhos de EEPROM maiores, por exemplo:

      - Arduino Duemilanove: armazenamento de EEPROM de 512 bytes.

      - Arduino Uno: armazenamento de EEPROM de 1 kilobyte.

      - Arduino Mega: armazenamento de EEPROM de 4 kilobytes.

      Em vez de codificar o comprimento, você deve usar a função de comprimento pré-fornecida.

      Isso tornará seu código portátil para todos os processadores AVR.

    ***/

    endereco = endereco + 1;

    if (endereco == EEPROM.length())
    {

        endereco = 0;
    }

    /***

      Como os tamanhos da EEPROM são potências de dois, o

      endereço da EEPROM também pode ser feito através

      de uma operação bitwise 'E' com o comprimento - 1.

      ++endereco &= EEPROM.length() - 1;

    ***/

    delay(100);
}
/*
Este código lê valores da entrada analógica 0 do Arduino e os armazena na EEPROM.
 Esses valores permanecerão na EEPROM mesmo quando a placa estiver desligada
  e podem ser recuperados por outros programas posteriormente.
   O código também explica como avançar para o próximo endereço da EEPROM
    e como lidar com diferentes tamanhos de EEPROM em processadores AVR.
*/