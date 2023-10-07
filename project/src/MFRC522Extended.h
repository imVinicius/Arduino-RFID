/**
 * Biblioteca estende MFRC522.h para suportar RATS para PICC ISO-14443-4.
 * RATS - Request for Answer To Select.
 * @author JPG-Consulting
 */
#ifndef MFRC522Extended_h
#define MFRC522Extended_h

#include <Arduino.h>
#include "MFRC522.h"

class MFRC522Extended : public MFRC522
{

public:
	// Taxas de bits ISO/IEC 14443-4
	enum TaxasBitTag : byte
	{
		BITRATE_106KBITS = 0x00,
		BITRATE_212KBITS = 0x01,
		BITRATE_424KBITS = 0x02,
		BITRATE_848KBITS = 0x03
	};

	// Estrutura para armazenar ATS ISO/IEC 14443-4
	typedef struct
	{
		byte tamanho;
		byte fsc; // Tamanho do quadro para cartão de proximidade

		struct
		{
			bool transmitido;
			bool mesmoD;	// Apenas o mesmo D para ambas as direções suportadas
			TaxasBitTag ds; // Enviar D
			TaxasBitTag dr; // Receber D
		} ta1;

		struct
		{
			bool transmitido;
			byte fwi;  // Tempo de espera do quadro inteiro
			byte sfgi; // Tempo de guarda do quadro de inicialização inteiro
		} tb1;

		struct
		{
			bool transmitido;
			bool suportaCID;
			bool suportaNAD;
		} tc1;

		// Dados brutos de ATS
		byte dados[FIFO_SIZE - 2]; // ATS não pode ser maior que FSD - 2 bytes (CRC), de acordo com ISO 14443-4 5.2.2
	} Ats;

	// Uma estrutura usada para passar informações PICC
	typedef struct
	{
		uint16_t atqa;
		Uid uid;
		Ats ats;

		// Para o bloco PCB
		bool numeroBloco;
	} InformacoesTag;

	// Uma estrutura usada para passar o bloco PCB
	typedef struct
	{
		struct
		{
			byte pcb;
			byte cid;
			byte nad;
		} prologo;
		struct
		{
			byte tamanho;
			byte *dados;
		} inf;
	} BlocoPcb;

	// Variáveis de membro
	InformacoesTag tag;

	/////////////////////////////////////////////////////////////////////////////////////
	// Contrutores
	/////////////////////////////////////////////////////////////////////////////////////
	MFRC522Extended() : MFRC522(){};
	MFRC522Extended(uint8_t rst) : MFRC522(rst){};
	MFRC522Extended(uint8_t ss, uint8_t rst) : MFRC522(ss, rst){};

	/////////////////////////////////////////////////////////////////////////////////////
	// Funções para comunicar com PICCs
	/////////////////////////////////////////////////////////////////////////////////////
	StatusCode PICC_Select(Uid *uid, byte validBits = 0) override; // sobrescrever
	StatusCode PICC_RequestATS(Ats *ats);
	StatusCode PICC_PPS();													 // Comando PPS sem parâmetro de taxa de bits
	StatusCode PICC_PPS(TaxasBitTag taxaEnvio, TaxasBitTag taxaRecebimento); // Diferentes valores D

	/////////////////////////////////////////////////////////////////////////////////////
	// Funções para comunicar com cartões ISO/IEC 14433-4
	/////////////////////////////////////////////////////////////////////////////////////
	StatusCode TCL_Transceive(BlocoPcb *enviar, BlocoPcb *resposta);
	StatusCode TCL_Transceive(InformacoesTag *tag, byte *dadosEnvio, byte tamanhoEnvio, byte *dadosResposta = NULL, byte *tamanhoResposta = NULL);
	StatusCode TCL_TransceiveRBlock(InformacoesTag *tag, bool ack, byte *dadosResposta = NULL, byte *tamanhoResposta = NULL);
	StatusCode TCL_Deselect(InformacoesTag *tag);

	/////////////////////////////////////////////////////////////////////////////////////
	// Funções de suporte
	/////////////////////////////////////////////////////////////////////////////////////
	static PICC_Type PICC_GetType(InformacoesTag *tag);
	using MFRC522::PICC_GetType; // // disponibiliza a antiga função PICC_GetType(byte sak), caso contrário seria ocultada por PICC_GetType(InformacoesTag *tag)

	// Funções de suporte para depuração
	void PICC_DumpToSerial(InformacoesTag *tag);
	using MFRC522::PICC_DumpToSerial; // disponibiliza a antiga função PICC_DumpToSerial(Uid *uid), caso contrário seria ocultada por PICC_DumpToSerial(InformacoesTag *tag)
	void PICC_DumpDetailsToSerial(InformacoesTag *tag);
	using MFRC522::PICC_DumpDetailsToSerial; // disponibiliza a antiga função PICC_DumpDetailsToSerial(Uid *uid), caso contrário seria ocultada por PICC_DumpDetailsToSerial(InformacoesTag *tag)
	void PICC_DumpISO14443_4(InformacoesTag *tag);

	/////////////////////////////////////////////////////////////////////////////////////
	// Funções de conveniência - não adicionam funcionalidade extra
	/////////////////////////////////////////////////////////////////////////////////////
	bool PICC_IsNewCardPresent() override; // sobrescrever
	bool PICC_ReadCardSerial() override;   // sobrescrever
};

#endif
