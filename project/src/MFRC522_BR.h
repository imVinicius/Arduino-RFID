/**
 * Biblioteca para usar o módulo MFRC522 no Arduino.
 *
 * @autores Dr.Leong, Miguel Balboa, Søren Thing Andersen, Tom Clement, e muitos outros! Veja o GitLog.
 *
 * Para mais informações, leia o README.
 *
 * Por favor, leia este arquivo para uma visão geral e depois o MFRC522.cpp para comentários sobre funções específicas.
 */
#ifndef MFRC522_h
#define MFRC522_h

#include "require_cpp11.h"
#include "deprecated.h"
// Habilitar limites de inteiros
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

#ifndef MFRC522_SPICLOCK
#define MFRC522_SPICLOCK (4000000u) // O MFRC522 aceita até 10 MHz, configurado para 4 MHz.
#endif

// Dados de firmware para autoteste
// Valores de referência com base na versão do firmware
// Dica: se necessário, você pode remover dados de autoteste não utilizados para economizar memória flash
//
// Versão 0.0 (0x90)
// Philips Semiconductors; Especificação Preliminar Revisão 2.0 - 01 de agosto de 2005; 16.1 autoteste
const byte MFRC522_firmware_referenceV0_0[] PROGMEM = {
	0x00, 0x87, 0x98, 0x0f, 0x49, 0xFF, 0x07, 0x19,
	0xBF, 0x22, 0x30, 0x49, 0x59, 0x63, 0xAD, 0xCA,
	0x7F, 0xE3, 0x4E, 0x03, 0x5C, 0x4E, 0x49, 0x50,
	0x47, 0x9A, 0x37, 0x61, 0xE7, 0xE2, 0xC6, 0x2E,
	0x75, 0x5A, 0xED, 0x04, 0x3D, 0x02, 0x4B, 0x78,
	0x32, 0xFF, 0x58, 0x3B, 0x7C, 0xE9, 0x00, 0x94,
	0xB4, 0x4A, 0x59, 0x5B, 0xFD, 0xC9, 0x29, 0xDF,
	0x35, 0x96, 0x98, 0x9E, 0x4F, 0x30, 0x32, 0x8D};
// Versão 1.0 (0x91)
// NXP Semiconductors; Rev. 3.8 - 17 de setembro de 2014; 16.1.1 autoteste
const byte MFRC522_firmware_referenceV1_0[] PROGMEM = {
	0x00, 0xC6, 0x37, 0xD5, 0x32, 0xB7, 0x57, 0x5C,
	0xC2, 0xD8, 0x7C, 0x4D, 0xD9, 0x70, 0xC7, 0x73,
	0x10, 0xE6, 0xD2, 0xAA, 0x5E, 0xA1, 0x3E, 0x5A,
	0x14, 0xAF, 0x30, 0x61, 0xC9, 0x70, 0xDB, 0x2E,
	0x64, 0x22, 0x72, 0xB5, 0xBD, 0x65, 0xF4, 0xEC,
	0x22, 0xBC, 0xD3, 0x72, 0x35, 0xCD, 0xAA, 0x41,
	0x1F, 0xA7, 0xF3, 0x53, 0x14, 0xDE, 0x7E, 0x02,
	0xD9, 0x0F, 0xB5, 0x5E, 0x25, 0x1D, 0x29, 0x79};
// Versão 2.0 (0x92)
// NXP Semiconductors; Rev. 3.8 - 17 de setembro de 2014; 16.1.1 autoteste
const byte MFRC522_firmware_referenceV2_0[] PROGMEM = {
	0x00, 0xEB, 0x66, 0xBA, 0x57, 0xBF, 0x23, 0x95,
	0xD0, 0xE3, 0x0D, 0x3D, 0x27, 0x89, 0x5C, 0xDE,
	0x9D, 0x3B, 0xA7, 0x00, 0x21, 0x5B, 0x89, 0x82,
	0x51, 0x3A, 0xEB, 0x02, 0x0C, 0xA5, 0x00, 0x49,
	0x7C, 0x84, 0x4D, 0xB3, 0xCC, 0xD2, 0x1B, 0x81,
	0x5D, 0x48, 0x76, 0xD5, 0x71, 0x61, 0x21, 0xA9,
	0x86, 0x96, 0x83, 0x38, 0xCF, 0x9D, 0x5B, 0x6D,
	0xDC, 0x15, 0xBA, 0x3E, 0x7D, 0x95, 0x3B, 0x2F};
// Clone
// Fudan Semiconductor FM17522 (0x88)
const byte FM17522_firmware_reference[] PROGMEM = {
	0x00, 0xD6, 0x78, 0x8C, 0xE2, 0xAA, 0x0C, 0x18,
	0x2A, 0xB8, 0x7A, 0x7F, 0xD3, 0x6A, 0xCF, 0x0B,
	0xB1, 0x37, 0x63, 0x4B, 0x69, 0xAE, 0x91, 0xC7,
	0xC3, 0x97, 0xAE, 0x77, 0xF4, 0x37, 0xD7, 0x9B,
	0x7C, 0xF5, 0x3C, 0x11, 0x8F, 0x15, 0xC3, 0xD7,
	0xC1, 0x5B, 0x00, 0x2A, 0xD0, 0x75, 0xDE, 0x9E,
	0x51, 0x64, 0xAB, 0x3E, 0xE9, 0x15, 0xB5, 0xAB,
	0x56, 0x9A, 0x98, 0x82, 0x26, 0xEA, 0x2A, 0x62};

class MFRC522
{
public:
	// Tamanho do FIFO do MFRC522
	static constexpr byte TAMANHO_FIFO = 64; // O FIFO tem 64 bytes.
	// Valor padrão para pino não utilizado
	static constexpr uint8_t PINO_NAO_UTILIZADO = UINT8_MAX;

	// Registradores do MFRC522. Descritos no capítulo 9 da folha de dados.
	// Ao usar SPI, todos os endereços são deslocados um bit para a esquerda no "byte de endereço SPI" (seção 8.1.2.3)
	enum PCD_Register : byte
	{
		// Página 0: Comando e status
		//						  0x00			// reservado para uso futuro
		RegistroComando = 0x01 << 1,	// inicia e interrompe a execução de comandos
		RegistroComIEn = 0x02 << 1,		// habilita e desabilita bits de controle de solicitação de interrupção
		RegistroDivIEn = 0x03 << 1,		// habilita e desabilita bits de controle de solicitação de interrupção
		RegistroComIrq = 0x04 << 1,		// bits de solicitação de interrupção
		RegistroDivIrq = 0x05 << 1,		// bits de solicitação de interrupção
		RegistroErro = 0x06 << 1,		// bits de erro mostrando o status de erro do último comando executado
		RegistroStatus1 = 0x07 << 1,	// bits de status de comunicação
		RegistroStatus2 = 0x08 << 1,	// bits de status do receptor e transmissor
		RegistroDadosFIFO = 0x09 << 1,	// entrada e saída do buffer FIFO de 64 bytes
		RegistroNivelFIFO = 0x0A << 1,	// número de bytes armazenados no buffer FIFO
		RegistroNivelAgua = 0x0B << 1,	// nível para aviso de subfluxo e sobrefluxo do FIFO
		RegistroControle = 0x0C << 1,	// registros de controle diversos
		RegistroBitFraming = 0x0D << 1, // ajustes para quadros orientados por bits
		RegistroColisao = 0x0E << 1,	// posição do bit da primeira colisão detectada na interface RF
		//						  0x0F			// reservado para uso futuro

		// Página 1: Comando
		// 						  0x10			// reservado para uso futuro
		RegistroModo = 0x11 << 1,		 // define modos gerais para transmissão e recepção
		RegistroModoTx = 0x12 << 1,		 // define taxa de dados de transmissão e formatação
		RegistroModoRx = 0x13 << 1,		 // define taxa de dados de recepção e formatação
		RegistroControleTx = 0x14 << 1,	 // controla o comportamento lógico dos pinos do driver da antena TX1 e TX2
		RegistroTxASK = 0x15 << 1,		 // controla a configuração da modulação de transmissão
		RegistroTxSelecao = 0x16 << 1,	 // seleciona as fontes internas para o driver da antena
		RegistroRxSelecao = 0x17 << 1,	 // seleciona configurações internas do receptor
		RegistroLimiarRx = 0x18 << 1,	 // seleciona limiares para o decodificador de bits
		RegistroDemodulador = 0x19 << 1, // define configurações do demodulador
		// 						  0x1A			// reservado para uso futuro
		// 						  0x1B			// reservado para uso futuro
		RegistroMfTx = 0x1C << 1, // controla alguns parâmetros de transmissão de comunicação MIFARE
		RegistroMfRx = 0x1D << 1, // controla alguns parâmetros de recepção de comunicação MIFARE
		// 						  0x1E			// reservado para uso futuro
		RegistroVelocidadeSerial = 0x1F << 1, // seleciona a velocidade da interface serial UART

		// Página 2: Configuração
		// 						  0x20			// reservado para uso futuro
		RegistroCRCResultadoH = 0x21 << 1, // mostra os valores MSB e LSB do cálculo CRC
		RegistroCRCResultadoL = 0x22 << 1,
		// 						  0x23			// reservado para uso futuro
		RegistroLarguraMod = 0x24 << 1, // controla a configuração da LarguraMod?
		// 						  0x25			// reservado para uso futuro
		RegistroConfiguracaoRF = 0x26 << 1, // configura o ganho do receptor
		RegistroGsN = 0x27 << 1,			// seleciona a condutância dos pinos do driver da antena TX1 e TX2 para modulação
		RegistroCWGsP = 0x28 << 1,			// define a condutância da saída do driver p durante períodos sem modulação
		RegistroModGsP = 0x29 << 1,			// define a condutância da saída do driver p durante períodos de modulação
		RegistroModoTimer = 0x2A << 1,		// define configurações para o temporizador interno
		RegistroPrescalerTimer = 0x2B << 1, // os 8 bits inferiores do valor do Prescaler do temporizador. Os 4 bits superiores estão em RegistroModoTimer.
		RegistroRecargaH = 0x2C << 1,		// define o valor de recarga do temporizador de 16 bits
		RegistroRecargaL = 0x2D << 1,
		RegistroValorContadorH = 0x2E << 1, // mostra o valor de 16 bits do temporizador
		RegistroValorContadorL = 0x2F << 1,

		// Página 3: Registros de Teste
		// 						  0x30			// reservado para uso futuro
		RegistroSelecaoTeste1 = 0x31 << 1,	   // configuração de sinal de teste geral
		RegistroSelecaoTeste2 = 0x32 << 1,	   // configuração de sinal de teste geral
		RegistroHabilitaPinoTeste = 0x33 << 1, // habilita o driver de saída do pino nos pinos D1 a D7
		RegistroValorPinoTeste = 0x34 << 1,	   // define os valores para D1 a D7 quando é usado como barramento de E/S
		RegistroBusTeste = 0x35 << 1,		   // mostra o status do barramento de teste interno
		RegistroAutoTeste = 0x36 << 1,		   // controla o autoteste digital
		RegistroVersao = 0x37 << 1,			   // mostra a versão do software
		RegistroTesteAnalogico = 0x38 << 1,	   // controla os pinos AUX1 e AUX2
		RegistroTesteDAC1 = 0x39 << 1,		   // define o valor de teste para TestDAC1
		RegistroTesteDAC2 = 0x3A << 1,		   // define o valor de teste para TestDAC2
		RegistroTesteADC = 0x3B << 1		   // mostra o valor dos canais ADC I e Q
											   // 						  0x3C			// reservado para testes de produção
											   // 						  0x3D			// reservado para testes de produção
											   // 						  0x3E			// reservado para testes de produção
											   // 						  0x3F			// reservado para testes de produção
	};
};
// Comandos MFRC522. Descritos no capítulo 10 da folha de dados.
enum ComandoMFRC522 : byte
{
	MFRC522_Inativo = 0x00,			   // nenhuma ação, cancela a execução do comando atual
	MFRC522_Memoria = 0x01,			   // armazena 25 bytes no buffer interno
	MFRC522_GerarIDAleatorio = 0x02,   // gera um número de ID aleatório de 10 bytes
	MFRC522_CalcularCRC = 0x03,		   // ativa o coprocessador CRC ou realiza um autoteste
	MFRC522_Transmitir = 0x04,		   // transmite dados do buffer FIFO
	MFRC522_SemAlteracaoCmd = 0x07,	   // nenhuma alteração de comando, pode ser usado para modificar os bits do registro CommandReg sem afetar o comando atual, por exemplo, o bit PowerDown
	MFRC522_Receber = 0x08,			   // ativa os circuitos do receptor
	MFRC522_Transciever = 0x0C,		   // transmite dados do buffer FIFO para a antena e ativa automaticamente o receptor após a transmissão
	MFRC522_AutenticacaoMIFARE = 0x0E, // realiza a autenticação padrão MIFARE como leitor
	MFRC522_ResetSuave = 0x0F		   // reinicia o MFRC522
};

// Máscaras MFRC522 RxGain[2:0], define o fator de ganho de tensão do receptor (no PCD).
// Descrito em 9.3.3.6 / tabela 98 da folha de dados em http://www.nxp.com/documents/data_sheet/MFRC522.pdf
enum GanhoRxMFRC522 : byte
{
	GanhoRx_18dB = 0x00 << 4,	// 000b - 18 dB, mínimo
	GanhoRx_23dB = 0x01 << 4,	// 001b - 23 dB
	GanhoRx_18dB_2 = 0x02 << 4, // 010b - 18 dB, parece que 010b é um duplicado de 000b
	GanhoRx_23dB_2 = 0x03 << 4, // 011b - 23 dB, parece que 011b é um duplicado de 001b
	GanhoRx_33dB = 0x04 << 4,	// 100b - 33 dB, médio e padrão típico
	GanhoRx_38dB = 0x05 << 4,	// 101b - 38 dB
	GanhoRx_43dB = 0x06 << 4,	// 110b - 43 dB
	GanhoRx_48dB = 0x07 << 4,	// 111b - 48 dB, máximo
	GanhoRx_Min = 0x00 << 4,	// 000b - 18 dB, mínimo, conveniência para GanhoRx_18dB
	GanhoRx_Medio = 0x04 << 4,	// 100b - 33 dB, médio, conveniência para GanhoRx_33dB
	GanhoRx_Max = 0x07 << 4		// 111b - 48 dB, máximo, conveniência para GanhoRx_48dB
};

// Comandos enviados para o PICC.
enum ComandoPICC : byte
{
	// Os comandos usados pelo PCD para gerenciar a comunicação com vários PICCs (ISO 14443-3, Tipo A, seção 6.4)
	PICC_CMD_REQA = 0x26,	 // comando REQuest, Tipo A. Convida os PICCs no estado IDLE a irem para READY e se prepararem para anticolisão ou seleção. Quadro de 7 bits.
	PICC_CMD_WUPA = 0x52,	 // comando Wake-UP, Tipo A. Convida PICCs no estado IDLE e HALT a irem para READY(*) e se prepararem para anticolisão ou seleção. Quadro de 7 bits.
	PICC_CMD_CT = 0x88,		 // Tag de Cascata. Não é realmente um comando, mas usado durante anticolisão.
	PICC_CMD_SEL_CL1 = 0x93, // Anticolisão/Seleção, Nível de Cascata 1
	PICC_CMD_SEL_CL2 = 0x95, // Anticolisão/Seleção, Nível de Cascata 2
	PICC_CMD_SEL_CL3 = 0x97, // Anticolisão/Seleção, Nível de Cascata 3
	PICC_CMD_HLTA = 0x50,	 // Comando HaLT, Tipo A. Instrui um PICC ATIVO a ir para o estado HALT.
	PICC_CMD_RATS = 0xE0,	 // Comando de solicitação para Resposta à Redefinição.
	// Os comandos usados para MIFARE Classic (de http://www.mouser.com/ds/2/302/MF1S503x-89574.pdf, Seção 9)
	// Use PCD_MFAuthent para autenticar o acesso a um setor, depois use esses comandos para ler/escrever/modificar os blocos no setor.
	// Os comandos de leitura/escrita também podem ser usados para MIFARE Ultralight.
	PICC_CMD_MF_AUTH_KEY_A = 0x60, // Realiza autenticação com a Chave A
	PICC_CMD_MF_AUTH_KEY_B = 0x61, // Realiza autenticação com a Chave B
	PICC_CMD_MF_READ = 0x30,	   // Lê um bloco de 16 bytes do setor autenticado do PICC. Também usado para MIFARE Ultralight.
	PICC_CMD_MF_WRITE = 0xA0,	   // Escreve um bloco de 16 bytes no setor autenticado do PICC. Chamado de "ESCRITA DE COMPATIBILIDADE" para MIFARE Ultralight.
	PICC_CMD_MF_DECREMENT = 0xC0,  // Decrementa o conteúdo de um bloco e armazena o resultado no registro de dados interno.
	PICC_CMD_MF_INCREMENT = 0xC1,  // Incrementa o conteúdo de um bloco e armazena o resultado no registro de dados interno.
	PICC_CMD_MF_RESTORE = 0xC2,	   // Lê o conteúdo de um bloco no registro de dados interno.
	PICC_CMD_MF_TRANSFER = 0xB0,   // Escreve o conteúdo do registro de dados interno em um bloco.
	// Os comandos usados para MIFARE Ultralight (de http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Seção 8.6)
	// Os comandos PICC_CMD_MF_READ e PICC_CMD_MF_WRITE também podem ser usados para MIFARE Ultralight.
	PICC_CMD_UL_WRITE = 0xA2 // Escreve uma página de 4 bytes no PICC.
};

// Constantes MIFARE que não se encaixam em nenhum outro lugar
enum MiscMIFARE
{
	MIFARE_ACK = 0xA,		 // O MIFARE Classic usa um ACK/NAK de 4 bits. Qualquer valor diferente de 0xA é um NAK.
	MIFARE_TAMANHO_CHAVE = 6 // Uma chave Mifare Crypto1 tem 6 bytes.
};

// Tipos PICC que podemos detectar. Lembre-se de atualizar PICC_GetTypeName() se adicionar mais.
// último valor definido como 0xff, então o compilador usa menos RAM, parece que algumas otimizações são acionadas
enum TipoPICC : byte
{
	PICC_TIPO_DESCONHECIDO,
	PICC_TIPO_ISO_14443_4,		  // PICC compatível com ISO/IEC 14443-4
	PICC_TIPO_ISO_18092,		  // PICC compatível com ISO/IEC 18092 (NFC)
	PICC_TIPO_MIFARE_MINI,		  // Protocolo MIFARE Classic, 320 bytes
	PICC_TIPO_MIFARE_1K,		  // Protocolo MIFARE Classic, 1KB
	PICC_TIPO_MIFARE_4K,		  // Protocolo MIFARE Classic, 4KB
	PICC_TIPO_MIFARE_UL,		  // MIFARE Ultralight ou Ultralight C
	PICC_TIPO_MIFARE_PLUS,		  // MIFARE Plus
	PICC_TIPO_MIFARE_DESFIRE,	  // MIFARE DESFire
	PICC_TIPO_TNP3XXX,			  // Mencionado apenas no NXP AN 10833 Procedimento de Identificação do Tipo MIFARE
	PICC_TIPO_NAO_COMPLETO = 0xff // SAK indica que o UID não está completo.
};

// Códigos de retorno das funções desta classe. Lembre-se de atualizar GetNomeCodigoStatus() se adicionar mais.
// último valor definido como 0xff, então o compilador usa menos RAM, parece que algumas otimizações são acionadas
enum CodigoStatus : byte
{
	STATUS_SUCESSO,			  // Sucesso
	STATUS_ERRO,			  // Erro na comunicação
	STATUS_COLISAO,			  // Colisão detectada
	STATUS_TIMEOUT,			  // Timeout na comunicação.
	STATUS_SEM_ESPACO,		  // Um buffer não tem tamanho suficiente.
	STATUS_ERRO_INTERNO,	  // Erro interno no código. Não deve acontecer ;-)
	STATUS_INVALIDO,		  // Argumento inválido.
	STATUS_CRC_INCORRETO,	  // O CRC_A não corresponde
	STATUS_MIFARE_NACK = 0xff // Um PICC MIFARE respondeu com NAK.
};
// A struct usada para passar o UID de um PICC.
typedef struct
{
	byte tamanho; // Número de bytes no UID. 4, 7 ou 10.
	byte uidByte[10];
	byte sak; // O byte SAK (Select Acknowledge) retornado pelo PICC após a seleção bem-sucedida.
} Uid;

// Uma struct usada para passar uma chave MIFARE Crypto1
typedef struct
{
	byte keyByte[MF_KEY_SIZE];
} ChaveMIFARE;

// Variáveis de membro
Uid uid; // Usado por PICC_ReadCardSerial().

/////////////////////////////////////////////////////////////////////////////////////
// Funções para configurar o Arduino
/////////////////////////////////////////////////////////////////////////////////////
MFRC522();
MFRC522(byte resetPowerDownPin);
MFRC522(byte chipSelectPin, byte resetPowerDownPin);

/////////////////////////////////////////////////////////////////////////////////////
// Funções de interface básica para comunicar com o MFRC522
/////////////////////////////////////////////////////////////////////////////////////
void PCD_EscreverRegistro(PCD_Register reg, byte valor);
void PCD_EscreverRegistro(PCD_Register reg, byte contador, byte *valores);
byte PCD_LerRegistro(PCD_Register reg);
void PCD_LerRegistro(PCD_Register reg, byte contador, byte *valores, byte rxAlinhamento = 0);
void PCD_DefinirMascaraBitsRegistro(PCD_Register reg, byte mascara);
void PCD_LimparMascaraBitsRegistro(PCD_Register reg, byte mascara);
StatusCode PCD_CalcularCRC(byte *dados, byte comprimento, byte *resultado);

/////////////////////////////////////////////////////////////////////////////////////
// Funções para manipular o MFRC522
/////////////////////////////////////////////////////////////////////////////////////
void PCD_Inicializar();
void PCD_Inicializar(byte resetPowerDownPin);
void PCD_Inicializar(byte chipSelectPin, byte resetPowerDownPin);
void PCD_Resetar();
void PCD_AntenaAtivar();
void PCD_AntenaDesativar();
byte PCD_ObterGanhoAntena();
void PCD_DefinirGanhoAntena(byte mascara);
bool PCD_RealizarAutoTeste();

/////////////////////////////////////////////////////////////////////////////////////
// Funções de controle de energia
/////////////////////////////////////////////////////////////////////////////////////
void PCD_DesligamentoSuave();
void PCD_LigamentoSuave();

/////////////////////////////////////////////////////////////////////////////////////
// Funções para comunicar com PICCs
/////////////////////////////////////////////////////////////////////////////////////
StatusCode PCD_TransmitirDados(byte *enviarDados, byte enviarLen, byte *receberDados, byte *receberLen, byte *validBits = nullptr, byte rxAlinhamento = 0, bool verificarCRC = false);
StatusCode PCD_ComunicarComPICC(byte comando, byte esperarIRq, byte *enviarDados, byte enviarLen, byte *receberDados = nullptr, byte *receberLen = nullptr, byte *validBits = nullptr, byte rxAlinhamento = 0, bool verificarCRC = false);
StatusCode PICC_SolicitarA(byte *bufferATQA, byte *tamanhoBuffer);
StatusCode PICC_DespertarA(byte *bufferATQA, byte *tamanhoBuffer);
StatusCode PICC_REQA_ou_WUPA(byte comando, byte *bufferATQA, byte *tamanhoBuffer);
virtual StatusCode PICC_Selecionar(Uid *uid, byte validBits = 0);
StatusCode PICC_PararA();
/////////////////////////////////////////////////////////////////////////////////////
// Funções para comunicação com PICCs MIFARE
/////////////////////////////////////////////////////////////////////////////////////
StatusCode PCD_Autenticar(byte comando, byte enderecoBloco, ChaveMIFARE *chave, Uid *uid);
void PCD_PararCrypto1();
StatusCode MIFARE_Ler(byte enderecoBloco, byte *buffer, byte *tamanhoBuffer);
StatusCode MIFARE_Escrever(byte enderecoBloco, byte *buffer, byte tamanhoBuffer);
StatusCode MIFARE_Ultralight_Escrever(byte pagina, byte *buffer, byte tamanhoBuffer);
StatusCode MIFARE_Decrementar(byte enderecoBloco, int32_t delta);
StatusCode MIFARE_Incrementar(byte enderecoBloco, int32_t delta);
StatusCode MIFARE_Restaurar(byte enderecoBloco);
StatusCode MIFARE_Transferir(byte enderecoBloco);
StatusCode MIFARE_ObterValor(byte enderecoBloco, int32_t *valor);
StatusCode MIFARE_DefinirValor(byte enderecoBloco, int32_t valor);
StatusCode PCD_NTAG216_AUTENTICA(byte *senha, byte pACK[]);

/////////////////////////////////////////////////////////////////////////////////////
// Funções de suporte
/////////////////////////////////////////////////////////////////////////////////////
StatusCode PCD_MIFARE_Transmitir(byte *enviarDados, byte enviarTamanho, bool aceitarTimeout = false);
// A função antiga usava muita memória, agora o nome foi movido para a flash; se você precisar de um char, copie da flash para a memória
// const char *ObterNomeStatusCode(byte codigo);
static const __FlashStringHelper *ObterNomeStatusCode(StatusCode codigo);
static PICC_Type PICC_ObterTipo(byte sak);
// A função antiga usava muita memória, agora o nome foi movido para a flash; se você precisar de um char, copie da flash para a memória
// const char *PICC_ObterNomeTipo(byte tipo);
static const __FlashStringHelper *PICC_ObterNomeTipo(PICC_Type tipo);

// Funções de suporte para depuração
void PCD_DespejarVersaoNoSerial();
void PICC_DespejarNoSerial(Uid *uid);
void PICC_DespejarDetalhesNoSerial(Uid *uid);
void PICC_DespejarMifareClassicNoSerial(Uid *uid, PICC_Type tipoPICC, ChaveMIFARE *chave);
void PICC_DespejarSetorMifareClassicNoSerial(Uid *uid, ChaveMIFARE *chave, byte setor);
void PICC_DespejarMifareUltralightNoSerial();

// Funções avançadas para MIFARE
void MIFARE_DefinirBitsAcesso(byte *bufferBitsAcesso, byte g0, byte g1, byte g2, byte g3);
bool MIFARE_AbrirPortaTraseiraUid(bool registrarErros);
bool MIFARE_DefinirUid(byte *novoUid, byte tamanhoUid, bool registrarErros);
bool MIFARE_DesbricarSetorUid(bool registrarErros);

/////////////////////////////////////////////////////////////////////////////////////
// Funções de conveniência - não adicionam funcionalidade extra
/////////////////////////////////////////////////////////////////////////////////////
virtual bool PICC_NovoCartaoPresente();
virtual bool PICC_LerSerialDoCartao();

protected:
byte _chipSelectPin;	 // Pino Arduino conectado à entrada de seleção de escravo SPI do MFRC522 (Pino 24, NSS, ativo baixo)
byte _resetPowerDownPin; // Pino Arduino conectado à entrada de reset e desligamento do MFRC522 (Pino 6, NRSTPD, ativo baixo)
StatusCode MIFARE_AssistenteDoisPassos(byte comando, byte enderecoBloco, int32_t dados);
}
;

#endif
