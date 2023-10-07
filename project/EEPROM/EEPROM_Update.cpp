/*
O objetivo deste exemplo é mostrar o método EEPROM.update()
 que escreve dados apenas se forem diferentes do conteúdo anterior
  das localizações a serem escritas. Essa solução pode economizar tempo de execução,
   pois cada operação de escrita leva 3,3 ms;
    a EEPROM também possui um limite de 100.000 ciclos de escrita por localização única,
     portanto, evitar reescrever
    o mesmo valor em qualquer local aumentará a vida útil geral da EEPROM.
*/

/***
   Método de Atualização da EEPROM

   Armazena valores lidos da entrada analógica 0 na EEPROM.
   Esses valores permanecerão na EEPROM quando a placa estiver
   desligada e podem ser recuperados posteriormente por outro esboço.

   Se um valor não tiver mudado na EEPROM, ele não é sobrescrito
   o que reduziria desnecessariamente a vida útil da EEPROM.

   Liberado sob licença MIT.
 ***/

#include <EEPROM.h>

/** o endereço atual na EEPROM (ou seja, qual byte escreveremos em seguida) **/
int endereco = 0;

void setup()
{
    /** Configuração vazia **/
}

void loop()
{
    /***
      é necessário dividir por 4 porque as entradas analógicas variam de
      0 a 1023 e cada byte da EEPROM só pode conter um valor de 0 a 255.
    ***/
    int val = analogRead(0) / 4;

    /***
      Atualiza a célula específica da EEPROM.
      esses valores permanecerão lá quando a placa estiver
      desligada.
    ***/
    EEPROM.update(endereco, val);

    /***
      A função EEPROM.update(endereco, val) é equivalente ao seguinte:

      if( EEPROM.read(endereco) != val ){
        EEPROM.write(endereco, val);
      }
    ***/

    /***
      Avança para o próximo endereço, quando chegar ao final, recomeça do início.

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

    delay(100);
}

/*
Este código armazena valores lidos da entrada analógica 0 na EEPROM do Arduino.
 Os valores permanecerão na EEPROM quando a placa estiver desligada
  e podem ser recuperados por outros esboços.
  Ele usa o método EEPROM.update() para escrever na EEPROM apenas se os dados forem diferentes
   dos dados anteriores,
 economizando assim ciclos de escrita e aumentando a vida útil da EEPROM.



*/