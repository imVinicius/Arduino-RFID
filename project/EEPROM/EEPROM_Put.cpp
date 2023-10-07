/*
O propósito deste exemplo é mostrar o método EEPROM.put()
 que escreve dados na EEPROM, usando também o EEPROM.update()
  que escreve dados apenas se forem diferentes do conteúdo anterior das localizações
   a serem escritas. O número de bytes escritos está relacionado
    ao tipo de dado ou à estrutura personalizada da variável a ser escrita.
*/

/***

    Exemplo de eeprom_put.

    Isso mostra como usar o método EEPROM.put().

    Além disso, este esboço predefinirá os dados da EEPROM para o

    esboço de exemplo eeprom_get.

    Observe que, ao contrário da versão de um único byte EEPROM.write(),

    o método put usará a semântica de atualização. Ou seja, um byte

    só será gravado na EEPROM se os dados forem realmente diferentes.

    Escrito por Christopher Andrews 2015

    Liberado sob a licença MIT.

***/

#include <EEPROM.h>

struct MeuObjeto
{

    float campo1;

    byte campo2;

    char nome[10];
};

void setup()
{

    Serial.begin(9600);

    while (!Serial)
    {

        ; // aguardar a conexão da porta serial. Necessário apenas para portas USB nativas
    }

    float f = 123.456f; // Variável para armazenar na EEPROM.
    int eeAddress = 0; // Local onde queremos colocar os dados.
    // Uma chamada simples, com o endereço primeiro e o objeto segundo.

    EEPROM.put(eeAddress, f);
    Serial.println("Tipo de dado float gravado!");

    /** O put é projetado para uso com estruturas personalizadas também. **/

    // Dados a serem armazenados.

    MeuObjeto varPersonalizada = {
        3.14f,
        65,
        "Funcionando!"

    };

    eeAddress += sizeof(float); // Move o endereço para o próximo byte após o float 'f'.
    EEPROM.put(eeAddress, varPersonalizada);
    Serial.print("Tipo de dado personalizado gravado! \n\nConsulte o exemplo de código eeprom_get para ver como você pode recuperar os valores!");
}

void loop()
{

    /* Loop vazio */
}

/*
Este código demonstra o uso do método EEPROM.put()
 para escrever dados na EEPROM do Arduino, 
 incluindo o armazenamento de um objeto personalizado.
  O código também explica que o método put utiliza a semântica de atualização,
   garantindo que os dados só serão gravados na EEPROM
    se forem diferentes dos dados anteriores.
*/