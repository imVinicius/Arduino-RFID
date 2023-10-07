/*
Este exemplo ilustra como ler o valor de cada byte da EEPROM usando a função EEPROM.read() 
e como imprimir esses valores na janela serial do Software Arduino (IDE).

*/

/*
 * Leitura da EEPROM
 *
 * Lê o valor de cada byte da EEPROM e imprime
 * no computador.
 * Este código de exemplo está em domínio público.
 */

#include <EEPROM.h>

// começa a leitura a partir do primeiro byte (endereço 0) da EEPROM
int endereco = 0;
byte valor;

void setup()
{
    // inicializa a comunicação serial e aguarda a abertura da porta:
    Serial.begin(9600);
    while (!Serial)
    {
        ; // aguarda a conexão da porta serial. Necessário apenas para portas USB nativas
    }
}

void loop()
{
    // lê um byte do endereço atual da EEPROM
    valor = EEPROM.read(endereco);

    Serial.print(endereco);
    Serial.print("\t");
    Serial.print(valor, DEC);
    Serial.println();

    /***
      Avança para o próximo endereço, quando chega ao final, recomeça do início.

      Processadores AVR maiores têm tamanhos de EEPROM maiores, por exemplo:
      - Arduino Duemilanove: armazenamento de EEPROM de 512 B.
      - Arduino Uno: armazenamento de EEPROM de 1 kB.
      - Arduino Mega: armazenamento de EEPROM de 4 kB.

      Em vez de codificar o comprimento, você deve usar a função de comprimento pré-fornecida.
      Isso tornará seu código portátil para todos os processadores AVR.
    ***/
    endereco = endereco + 1;
    if (endereco == EEPROM.length())
    {
        endereco = 0;
    }

    /***
      Como os tamanhos da EEPROM são potências de dois, a prevenção de estouro
      de um endereço da EEPROM também pode ser feita por uma operação bitwise 'e' com o comprimento - 1.

      ++endereco &= EEPROM.length() - 1;
    ***/

    delay(500);
}

/*
Este código lê o valor de cada byte da EEPROM do Arduino 
e imprime esses valores na porta serial do computador.
Ele também explica como avançar para o próximo endereço da EEPROM 
    e como lidar com diferentes tamanhos de EEPROM em processadores AVR.

*/