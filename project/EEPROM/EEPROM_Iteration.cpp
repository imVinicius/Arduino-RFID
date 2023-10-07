/*
O propósito deste exemplo é mostrar como percorrer todo o espaço de memória da EEPROM
 com abordagens diferentes. O código fornecido não é executado por si só,
    -> mas deve ser usado como uma fonte de trechos de código para serem utilizados
         em outros lugares.
*/

/***

    Exemplo de iteração na EEPROM.

    Um conjunto de trechos de exemplo destacando

    os métodos mais simples para percorrer a EEPROM.

    Não é necessário executar este esboço, isso é

    apenas para destacar certos métodos de programação.

    Escrito por Christopher Andrews 2015

    Liberado sob a licença MIT.

***/

#include <EEPROM.h>

void setup()
{

    /***

      Iterar na EEPROM usando um loop for.

    ***/

    for (int indice = 0; indice < EEPROM.length(); indice++)
    {

        // Adicione um a cada célula na EEPROM

        EEPROM[indice] += 1;
    }

    /***

      Iterar na EEPROM usando um loop while.

    ***/

    int indice = 0;

    while (indice < EEPROM.length())
    {

        // Adicione um a cada célula na EEPROM

        EEPROM[indice] += 1;

        indice++;
    }

    /***

      Iterar na EEPROM usando um loop do-while.

    ***/

    int idx = 0; // Usado 'idx' para evitar conflito de nome com 'indice' acima.

    do
    {

        // Adicione um a cada célula na EEPROM

        EEPROM[idx] += 1;

        idx++;

    } while (idx < EEPROM.length());

} // Fim da função setup.

void loop() {}

/*
Este código demonstra diferentes métodos de iteração na memória EEPROM do Arduino
    ->usando loops for, while e do-while.
Cada método incrementa o valor de todas as células da EEPROM em 1.
O código serve apenas como exemplo de programação e não é necessário executá-lo,
    ->pois seu objetivo principal é ilustrar os métodos de iteração.
*/