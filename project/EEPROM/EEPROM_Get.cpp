/*
O propósito deste exemplo é mostrar como os métodos put() e get()
    -> fornecem um comportamento diferente do que os métodos write() e read()
que operam em bytes individuais. Ao obter diferentes variáveis da EEPROM,
    -> são recuperados um número de bytes relacionados ao tipo de dado da variável.
*/

/***

    Exemplo de uso do eeprom_get.

    Isso mostra como usar o método EEPROM.get().

    Para predefinir os dados da EEPROM, execute o esboço de exemplo eeprom_put.

    Este esboço funcionará sem ele, no entanto, os valores exibidos

    serão exibidos a partir do que já estiver na EEPROM.

    Isso pode fazer com que o objeto serial imprima uma grande sequência

    de lixo se não houver um caractere nulo dentro de uma das strings

    carregadas.

    Escrito por Christopher Andrews 2015

    Liberado sob a licença MIT.

***/

#include <EEPROM.h>

void setup()
{

    float f = 0.00f; // Variável para armazenar os dados lidos da EEPROM.

    int eeAddress = 0; // Endereço da EEPROM para começar a leitura

    Serial.begin(9600);

    while (!Serial)
    {

        ; // aguardar a conexão da porta serial. Necessário apenas para portas USB nativas
    }

    Serial.print("Ler float da EEPROM: ");

    // Obtenha os dados de ponto flutuante da EEPROM na posição 'eeAddress'

    EEPROM.get(eeAddress, f);

    Serial.println(f, 3); // Isso pode imprimir 'ovf, nan' se os dados dentro da EEPROM não forem um float válido.

    /***

      Como o get também retorna uma referência para 'f', você pode usá-lo inline.

      Exemplo: Serial.print( EEPROM.get( eeAddress, f ) );

    ***/

    /***

      O get pode ser usado com estruturas personalizadas também.

      Eu separei isso em uma função extra.

    ***/

    segundoTeste(); // Execute o próximo teste.
}

struct MeuObjeto
{

    float campo1;

    byte campo2;

    char nome[10];
};

void segundoTeste()
{

    int eeAddress = sizeof(float); // Mova o endereço para o próximo byte após o float 'f'.

    MeuObjeto varPersonalizada; // Variável para armazenar objeto personalizado lido da EEPROM.

    EEPROM.get(eeAddress, varPersonalizada);

    Serial.println("Ler objeto personalizado da EEPROM: ");

    Serial.println(varPersonalizada.campo1);

    Serial.println(varPersonalizada.campo2);

    Serial.println(varPersonalizada.nome);
}

void loop()
{

    /* Loop vazio */
}

/*
Este código demonstra o uso do método EEPROM.get() para ler dados da EEPROM do Arduino
    -> incluindo o armazenamento e leitura de um objeto personalizado
O código também explica que, se a EEPROM não estiver inicializada com dados válidos
    -> pode ocorrer a impressão de caracteres inválidos na porta serial.
*/