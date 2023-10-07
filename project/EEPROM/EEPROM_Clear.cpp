/*
 * Limpeza da EEPROM
 *
 * Define todos os bytes da EEPROM como 0.
 * Consulte eeprom_iteration para uma análise mais detalhada
 * de como percorrer a EEPROM.
 *
 * Este código de exemplo está em domínio público.
 */
/*
"Este exemplo ilustra como definir todos esses bytes como 0, 
inicializando-os para armazenar novas informações, usando a função EEPROM.write()."
*/
#include <EEPROM.h>

void setup()
{
    // inicializa o pino do LED como saída.
    pinMode(13, OUTPUT);

    /***
      Itera por cada byte do armazenamento da EEPROM.
      Processadores AVR maiores têm tamanhos de EEPROM maiores, por exemplo:
      - Arduino Duemilanove: armazenamento de EEPROM de 512 B.
      - Arduino Uno: armazenamento de EEPROM de 1 kB.
      - Arduino Mega: armazenamento de EEPROM de 4 kB.
      Em vez de codificar o comprimento diretamente, você deve usar a função de comprimento pré-fornecida.
      Isso tornará seu código portátil para todos os processadores AVR.
    ***/

    for (int i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.write(i, 0);
    }

    // ligue o LED quando terminarmos
    digitalWrite(13, HIGH);
}

void loop()
{
    /** Loop vazio. **/
}
