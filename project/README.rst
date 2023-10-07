MFRC522
=======

.. image:: https://img.shields.io/maintenance/no/2019.svg
    :target: `desenvolvimento`_
.. image:: https://github.com/miguelbalboa/rfid/workflows/PlatformIO%20CI/badge.svg
   :target: https://github.com/miguelbalboa/rfid/actions
   :alt: GitHub Actions
.. image:: https://img.shields.io/badge/C%2B%2B-11-green.svg
    :target: `IDE compatível`_
.. image:: https://img.shields.io/github/release/miguelbalboa/rfid.svg?colorB=green
    :target: https://github.com/miguelbalboa/rfid/releases
    :alt: Versões
.. image:: https://img.shields.io/badge/ArduinoIDE-%3E%3D1.6.10-lightgrey.svg
    :target: `IDE compatível`_

Biblioteca Arduino para módulos MFRC522 e outros módulos baseados em RFID RC522.

Leia e escreva diferentes tipos de cartões de Identificação por Radiofrequência (RFID)
no seu Arduino usando um leitor baseado em RC522 conectado através da Interface Periférica Serial (SPI).

Para desenvolvimento avançado e adicional, por favor, use a biblioteca `RFID_MFRC522v2 <https://github.com/OSSLibraries/Arduino_MFRC522v2>`_.

.. _desenvolvimento:
Desenvolvimento
---------------

**O desenvolvimento pelo proprietário miguelbalboa foi encerrado**.

**Status de recursos: congelamento completo**; nenhuma função ou mudança de API.

**Status de código: congelamento parcial**; apenas correções/erros ou atualizações de documentação; *sem* extensões para outras placas; *sem* novos exemplos.

**Status de manutenção: esporadicamente**.

**Por que não há mais desenvolvimento?**
Esta biblioteca tem uma longa história e é usada em muitos projetos. Muitas vezes, esses projetos não documentam qual versão estão usando. Fazer alterações pode quebrar esses projetos antigos e levar a experiências ruins (para iniciantes) e solicitações de suporte. Por essas razões, a biblioteca está em modo de congelamento. Ainda é possível corrigir erros de digitação, documentação ou bugs.

**Código-fonte refatorado por Vinicius do Nascimento**.
;O andamento do projeto tem como propósito o uso liver do código para fins não lucrativos.

.. _antes de comprar:
Antes de comprar
----------------

Observe que existem muitos vendedores (eBay, AliExpress, etc.) que vendem placas MFRC522. **A qualidade dessas placas é extremamente diferente.** Algumas têm capacitores errados/de baixa qualidade ou placas MFRC522 falsas/defeituosas.

**Considere comprar vários dispositivos de diferentes fornecedores.** Assim, a chance de obter um dispositivo funcional é maior.

Se você recebeu uma placa ruim e pode nos dizer como detectar essas placas (seda, descrição do chip, etc.), por favor, compartilhe seu conhecimento.


.. _o que funciona e o que não funciona:
O que funciona e o que não funciona?
--------------------------------------

* **Funciona**
  
  #. Comunicação (Crypto1) com MIFARE Classic (1k, 4k, Mini).
  #. Comunicação (Crypto1) com PICCs compatíveis com MIFARE Classic.
  #. Verificação automática do firmware do MFRC522.
  #. Definir o UID, escrever no setor 0 e desfazer blocos UID chineses modificáveis MIFARE.
  #. Gerenciar o pino de seleção de chip SPI (também conhecido como SS, SDA).

* **Funciona parcialmente**

  #. Comunicação com MIFARE Ultralight.
  #. Outros PICCs (Ntag216).
  #. Mais de 2 módulos, requer um multiplexador `#191 <https://github.com/miguelbalboa/rfid/issues/191#issuecomment-242631153>`_.

* **Não funciona**
  
  #. MIFARE DESFire, MIFARE DESFire EV1/EV2, não suportado por software.
  #. Comunicação com 3DES ou AES, não suportado por software.
  #. Peer-to-peer (ISO/IEC 18092), não `suportado por hardware`_.
  #. Comunicação com smartphone, não `suportado por hardware`_.
  #. Emulação de cartão, não `suportado por hardware`_.
  #. Uso do pino IRQ. Mas há um exemplo de prova de conceito.
  #. Com Intel Galileo (Gen2), veja `#310 <https://github.com/miguelbalboa/rfid/issues/310>`__, não suportado por software.
  #. Modos de redução de energia `#269 <https://github.com/miguelbalboa/rfid/issues/269>`_, não suportado por software.
  #. I2C em vez de SPI `#240 <https://github.com/miguelbalboa/rfid/issues/240>`_, não suportado por software.
  #. UART em vez de SPI `#281 <https://github.com/miguelbalboa/rfid/issues/281>`_, não suportado por software.
  
* **Precisa de mais?**

  #. Se for software: escreva o código e faça uma solicitação de pull.
  #. Se for hardware: compre um chip mais caro como o PN532 (suporta NFC e muito mais, mas custa cerca de $15 e não é compatível com esta biblioteca).


.. _IDE compatível:
IDE Compatível
--------------

Esta biblioteca funciona com o Arduino IDE 1.6, versões mais antigas **não são suportadas** e causarão erros de compilação. O gerenciador de bibliotecas embutido é suportado.

Se você usar seu próprio compilador, precisará habilitar o suporte a ``c++11``.


.. _placas compatíveis:
Placas Compatíveis
------------------

**!!!Apenas para usuários avançados!!!**

Esta biblioteca é compatível com o Teensy e o ESP8266 se você usar o plugin da placa no Arduino IDE. Nem todos os exemplos estão disponíveis para todas as placas. Você também precisará alterar os pinos. Consulte `layout de pinos`_.

Alguns usuários fizeram alguns patches/sugestões/portes para outras placas:

* Linux: https://github.com/miguelbalboa/rfid/pull/216
* chipKIT: https://github.com/miguelbalboa/rfid/pull/230
* ESP8266 (nativo): https://github.com/miguelbalboa/rfid/pull/235
* LPCOPen (em C): https://github.com/miguelbalboa/rfid/pull/258

Observe que o principal alvo/suporte da biblioteca ainda é o Arduino.

.. _suporte e problemas:
Suporte/Problemas
-----------------
1. Primeiro, verifique o que funciona e o que não funciona e a seção de solução de problemas.

2. Parece ser um problema de hardware ou você precisa de suporte para programar seu projeto?
    Por favor, pergunte no `fórum oficial do Arduino <https://forum.arduino.cc/>`, onde você obterá uma resposta muito mais rápida do que no Github.

3. Parece ser um problema de software?
    Abra um problema no Github.


.. _estilo de código:
Estilo de Código
----------------

Por favor, use ``inteiros fixos``, consulte `stdint.h`_. Por quê? Esta biblioteca é compatível com diferentes placas que usam arquiteturas diferentes (16 bits e 32 bits). Variáveis ``int`` não fixadas têm tamanhos diferentes em ambientes diferentes e podem causar comportamento imprevisível.


.. _layout de pinos:
Layout de Pinos
----------------

A tabela a seguir mostra o layout típico de pinos usado:

+-----------+----------+-----------------------------------------------------------------------------------+
|           | PCD      |                                      Arduino                                      |
|           +----------+-------------+---------+---------+-----------------+-----------+---------+---------+
|           | MFRC522  | Uno / 101   | Mega    | Nano v3 |Leonardo / Micro | Pro Micro | Yun [4]_| Due     |
+-----------+----------+-------------+---------+---------+-----------------+-----------+---------+---------+
| Sinal     | Pino     | Pino        | Pino    | Pino    | Pino            | Pino      | Pino    | Pino    |
+===========+==========+=============+=========+=========+=================+===========+=========+=========+
| RST/Reset | RST      | 9 [1]_      | 5 [1]_  | D9      | RESET / ICSP-5  | RST       | Pin9    | 22 [1]_ |
+-----------+----------+-------------+---------+---------+-----------------+-----------+---------+---------+
| SPI SS    | SDA [3]_ | 10 [2]_     | 53 [2]_ | D10     | 10              | 10        | Pin10   | 23 [2]_ |
+-----------+----------+-------------+---------+---------+-----------------+-----------+---------+---------+
| SPI MOSI  | MOSI     | 11 / ICSP-4 | 51      | D11     | ICSP-4          | 16        | ICSP4   | SPI-4   |
+-----------+----------+-------------+---------+---------+-----------------+-----------+---------+---------+
| SPI MISO  | MISO     | 12 / ICSP-1 | 50      | D12     | ICSP-1          | 14        | ICSP1   | SPI-1   |
+-----------+----------+-------------+---------+---------+-----------------+-----------+---------+---------+
| SPI SCK   | SCK      | 13 / ICSP-3 | 52      | D13     | ICSP-3          | 15        | ICSP3   | SPI-3   |
+-----------+----------+-------------+---------+---------+-----------------+-----------+---------+---------+

+-----------+---------------+--------------------------+-------------+
|           | ESP8266       | Teensy                   | 8F328P-U    |
|           +---------------+--------+--------+--------+-------------+
|           | Wemos D1 mini | 2.0    | ++ 2.0 | 3.1    | ALPHA       |
+-----------+---------------+--------+--------+--------+-------------+
| Sinal     | Pino          | Pino   | Pino   | Pino   | Pino [5]_    |
+===========+===============+========+========+========+=============+
| RST/Reset | D3            | 7      | 4      | 9      | D9# [1]_    |
+-----------+---------------+--------+--------+--------+-------------+
| SPI SS    | D8            | 0      | 20     | 10     | D10# [2]_   |
+-----------+---------------+--------+--------+--------+-------------+
| SPI MOSI  | D7            | 2      | 22     | 11     | MOSI / D11# |
+-----------+---------------+--------+--------+--------+-------------+
| SPI MISO  | D6            | 3      | 23     | 12     | MISO / D12# |
+-----------+---------------+--------+--------+--------+-------------+
| SPI SCK   | D5            | 1      | 21     | 13     | SCK         |
+-----------+---------------+--------+--------+--------+-------------+

.. [1] Configurável, normalmente definido como RST_PIN no esboço/programa.
.. [2] Configurável, normalmente definido como SS_PIN no esboço/programa.
.. [3] O pino SDA pode ser rotulado como SS em algumas/placas MFRC522 mais antigas. 
.. [4] Fonte: `#111 <https://github.com/miguelbalboa/rfid/issues/111#issuecomment-420433658>`_ .
.. [5] Os nomes dos pinos do lado de trás (vazio) da placa foram usados como mais definitivos.

Importante: Se seu microcontrolador suportar múltiplas interfaces SPI, a biblioteca usará apenas a **SPI padrão (primeira)** do framework Arduino.

.. _hardware:
Hardware

Existem três componentes de hardware envolvidos:

    Microcontrolador:

    Um Arduino_ ou compatível que execute o Sketch usando esta biblioteca.

    Os preços variam de USD 7 para clones a USD 75 para "kits iniciantes" (o que
    pode ser uma boa escolha se esta for sua primeira exposição ao Arduino;
    verifique se tal kit já inclui o Arduino, o leitor e algumas tags).

    Dispositivo de Acoplamento de Proximidade (PCD):

    O PCD é o Leitor RFID real baseado no Circuito Integrado de Leitura Sem Contato NXP MFRC522_.

    Leitores podem ser encontrados no eBay_ por cerca de USD 5: pesquise por "rc522".

    Você também pode encontrá-los em várias lojas online. Eles costumam ser incluídos em
    "kits iniciantes", então verifique seu fornecedor de eletrônicos favorito também.

    Circuito Integrado de Cartão de Proximidade (PICC):

    O PICC é o Cartão ou Etiqueta RFID usando a interface ISO/IEC 14443A_,
    como Mifare ou NTAG203.

    Um ou dois podem ser incluídos com o Leitor ou em um "kit iniciante".

.. _protocol:
Protocolos

    O microcontrolador e o leitor usam SPI para comunicação.

    O protocolo é descrito no datasheet NXP MFRC522_.

    Consulte a seção Layout de Pinos_ para detalhes sobre como conectar os pinos.

    O leitor e as etiquetas se comunicam usando um campo eletromagnético de 13,56 MHz.

    O protocolo é definido na ISO/IEC 14443-3:2011 Parte 3 Tipo A.

        Os detalhes são encontrados no capítulo 6 "Tipo A - Inicialização e anti-colisão".

        Veja http://wg8.de/wg8n1496_17n3613_Ballot_FCD14443-3.pdf para uma versão gratuita
        do rascunho final (que pode estar desatualizado em algumas áreas).

        O leitor não suporta o Tipo B ISO/IEC 14443-3.

.. _security:
Segurança

    O UID de um cartão não pode ser usado como uma identificação única para projetos relacionados à segurança. Alguns cartões chineses permitem alterar o UID, o que significa que você pode clonar facilmente um cartão. Para projetos como controle de acesso, abridor de portas ou sistemas de pagamento, você deve implementar um mecanismo de segurança adicional, como uma senha ou chave normal.

    Esta biblioteca suporta apenas comunicação criptografada com Crypto1. O Crypto1 é conhecido como quebrado_ há alguns anos, portanto, NÃO oferece NENHUMA segurança, é uma comunicação virtualmente não criptografada. Não o utilize para aplicativos relacionados à segurança!

    Esta biblioteca não oferece autenticação 3DES ou AES usada por cartões como o Mifare DESFire. Pode ser possível implementá-lo, pois o datasheet diz que há suporte. Esperamos por solicitações de pull :).

.. _troubleshooting:
Solução de Problemas

    Não estou recebendo entrada do leitor ou AVISO: Falha na comunicação, o MFRC522 está corretamente conectado?

    #. Verifique sua conexão física, veja o Layout de Pinos_ .
    #. Verifique as configurações/variáveis de pinos no código, veja o Layout de Pinos_ .
    #. Verifique a soldagem do cabeçalho dos pinos. Talvez você tenha soldas frias.
    #. Verifique sua fonte de alimentação. Talvez adicione um capacitor entre 3,3V e GND para estabilizar a energia #560, às vezes, um atraso adicional após PCD_Init() pode ajudar.
    #. Verifique a voltagem. A maioria dos módulos funciona com 3,3V.
    #. SPI funciona apenas com 3,3V, a maioria dos módulos parece ser tolerante a 5V, mas tente um level shifter (conversor de nível).
    #. SPI não gosta de conexões longas. Tente conexões mais curtas.
    #. SPI não gosta de placas de prototipagem. Tente conexões soldadas.
    #. De acordo com os relatórios #101, #126 e #131, pode haver um problema com a soldagem no módulo MFRC522. Você pode consertar isso por conta própria.

    Versão do Firmware: 0x12 = (desconhecida) ou outros valores aleatórios

    #. A razão exata desse comportamento é desconhecida.
    #. Alguns módulos precisam de mais tempo após PCD_Init() para ficarem prontos. Como solução alternativa, adicione um delay(4) diretamente após PCD_Init() para dar mais tempo ao PCD.
    #. Se isso aparecer às vezes, uma conexão ruim ou fonte de alimentação é a razão.
    #. Se a versão do firmware for relatada permanentemente, é muito provável que o hardware seja falso ou tenha um defeito. Entre em contato com seu fornecedor.

    Às vezes ocorrem timeouts ou às vezes a etiqueta/cartão não funciona.

    #. Tente o outro lado da antena.
    #. Tente diminuir a distância entre o MFRC522 e sua etiqueta.
    #. Aumente o ganho da antena por meio do firmware: mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    #. Use uma fonte de alimentação melhor.
    #. O hardware pode estar corrompido, a maioria dos produtos vem da China e, às vezes, a qualidade é realmente ruim. Entre em contato com seu vendedor.

    Minha etiqueta/cartão não funciona.

    #. Distância entre a antena e o token muito grande (>1cm).
    #. Você pode ter o tipo de PICC errado. É realmente de 13,56 MHz? É realmente um Tipo A Mifare?
    #. Etiquetas NFC não são suportadas. Algumas podem funcionar.
    #. Etiquetas de identificação de animais RFID não são suportadas. Elas usam uma frequência diferente (125 kHz).
    #. O hardware pode estar corrompido, a maioria dos produtos vem da China e, às vezes, a qualidade é realmente ruim. Entre em contato com seu vendedor.
    #. Versões mais recentes de cartões Mifare, como DESFire/Ultralight, podem não funcionar devido à autenticação ausente, consulte segurança_ ou protocolo_ diferente.
    #. Alguns módulos comprados de fabricantes chineses não usam os melhores componentes e isso pode afetar a detecção de diferentes tipos de etiquetas/cartões. Em alguns desses módulos, os indutores L1 e L2 não têm corrente alta o suficiente, então o sinal gerado não é suficiente para fazer com que as etiquetas Ultralight C e NTAG203 funcionem. Substituir esses indutores por indutores com a mesma indutância (2,2uH), mas corrente operacional mais alta, deve fazer as coisas funcionarem sem problemas. Além disso, em alguns desses módulos, o circuito harmônico e de casamento precisa ser ajustado, para isso substitua C4 e C5 por capacitores de 33pF e você estará pronto. (Fonte: Mikro Elektronika_)

    Meu celular não reconhece o MFRC522 ou meu MFRC522 não consegue ler dados de outro MFRC522

    #. A simulação de cartões não é suportada.
    #. A comunicação com telefones celulares não é suportada.
    #. A comunicação ponto a ponto não é suportada.

    Só consigo ler o UID do cartão.

    #. Talvez os AccessBits tenham sido acidentalmente configurados e agora uma senha desconhecida esteja definida. Isso não pode ser revertido.
    #. Provavelmente o cartão está criptografado. Especialmente cartões oficiais como os de transporte público, universidade ou biblioteca. Não há nenhuma maneira de acessar com esta biblioteca.

    Onde posso obter mais informações?

    #. Para suporte geral da comunidade, consulte Fórum do Arduino <https://forum.arduino.cc/>_ ou StackOverflow <https://stackoverflow.com/questions/tagged/mifare>_ .
    #. Visite a wiki da comunidade mfrc522 <https://github.com/miguelbalboa/rfid/wiki>_ .
    #. Leia os datasheets!
    #. Seu mecanismo de busca preferido.

    Preciso de mais recursos.

    #. Se for software: programe e faça um pull request.
    #. Se for hardware: compre um chip mais caro como o PN532 (suporta NFC e muito mais, mas custa cerca de $15)

.. _license:
Licença

Este é um software gratuito e desimpedido lançado no domínio público.

Qualquer pessoa está livre para copiar, modificar, publicar, usar, compilar, vender ou
distribuir este software, seja na forma de código-fonte ou como um binário compilado,
para qualquer finalidade, comercial ou não comercial, e por qualquer
meio.

Em jurisdições que reconhecem leis de direitos autorais, o autor ou autores
deste software dedicam todo e qualquer interesse de direitos autorais no
software ao domínio público. Fazemos esta dedicação em benefício
do público em geral e em detrimento de nossos herdeiros e
sucessores. Pretendemos que esta dedicação seja um ato evidente de
renúncia perpétua de todos os direitos presentes e futuros a este
software sob a lei de direitos autorais.

O SOFTWARE É FORNECIDO "COMO ESTÁ", SEM GARANTIA DE QUALQUER TIPO,
EXPRESSA OU IMPLÍCITA, INCLUINDO, MAS NÃO SE LIMITANDO A GARANTIAS DE
COMERCIALIZAÇÃO, ADEQUAÇÃO A UM DETERMINADO FIM E NÃO VIOLAÇÃO.
EM NENHUM CASO OS AUTORES SERÃO RESPONSÁVEIS POR QUALQUER RECLAMAÇÃO, DANOS OU
OUTRA RESPONSABILIDADE, SEJA EM UMA AÇÃO DE CONTRATO, DELITO OU OUTRO MOTIVO,
DECORRENTE DE, OU EM CONEXÃO COM O SOFTWARE OU O USO OU
OUTRAS NEGOCIAÇÕES NO SOFTWARE.

Para mais informações, consulte https://unlicense.org/

.. _dependency:
Dependências

    Arduino.h
        Origem: Arduino IDE / Específico para o alvo
        Licença: (alvo: Arduino) GNU Lesser General Public License 2.1

    SPI.h
        Origem: Arduino IDE / Específico para o alvo
        Licença: (alvo: Arduino) GNU Lesser General Public License 2.1

    stdint.h
        Origem: Arduino IDE / Compilador e alvo específico
        Licença: Diferente

Histórico

A biblioteca MFRC522 foi criada pela primeira vez em janeiro de 2012 por Miguel Balboa (de
http://circuitito.com) com base no código de Dr. Leong (de http://B2CQSHOP.com)
para o "Kit de módulo RFID Arduino 13,56 Mhz com Tags SPI W e R por COOQRobot".

Foi traduzida para o inglês e reescrita/refatorada no outono de 2013
por Søren Thing Andersen (de http://access.thing.dk).

Em outubro de 2014, foi estendida com funcionalidades para alterar o setor 0 em um cartão MIFARE chinês com UID alterável por Tom Clement (de http://tomclement.nl).

Mantida por miguelbalboa até 2016.
Mantida por Rotzbua de 2016 até 2022.

.. _arduino: https://arduino.cc/
.. _ebay: https://www.ebay.com/
.. _iso/iec 14443a: https://en.wikipedia.org/wiki/ISO/IEC_14443
.. _iso/iec 14443-3:2011 parte 3:
.. _nxp mfrc522: https://www.nxp.com/documents/data_sheet/MFRC522.pdf
.. _broken: https://eprint.iacr.org/2008/166
.. _suportado pelo hardware: https://web.archive.org/web/20151210045625/http://www.nxp.com/documents/leaflet/939775017564.pdf
.. _Fórum Arduino: https://forum.arduino.cc
.. _stdint.h: https://en.wikibooks.org/wiki/C_Programming/C_Reference/stdint.h
.. _Mikro Elektronika: https://forum.mikroe.com/viewtopic.php?f=147&t=64203
