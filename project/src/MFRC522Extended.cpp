/*
 * Biblioteca estende o MFRC522.h para suportar RATS para o PICC ISO-14443-4.
 * RATS - Request for Answer To Select.
 * NOTA: Por favor, verifique também os comentários em MFRC522Extended.h
 * @autor JPG-Consulting
 */

#include "MFRC522Extended.h"

/////////////////////////////////////////////////////////////////////////////////////
// Funções para comunicação com PICCs
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Transmite comandos SELECT/ANTICOLLISION para selecionar um único PICC.
 * Antes de chamar esta função, os PICCs devem ser colocados no estado READY(*) chamando PICC_RequestA() ou PICC_WakeupA().
 * Em caso de sucesso:
 * - O PICC escolhido está no estado ATIVO(*) e todos os outros PICCs retornaram ao estado IDLE/HALT. (Figura 7 do rascunho ISO/IEC 14443-3.)
 * - O tamanho e o valor do UID do PICC escolhido são retornados em *uid, juntamente com o SAK.
 *
 * Um UID do PICC consiste em 4, 7 ou 10 bytes.
 * Apenas 4 bytes podem ser especificados em um comando SELECT, portanto, para UID mais longos, são usadas duas ou três iterações:
 * Tamanho do UID	Número de bytes do UID	Níveis de Cascata	Exemplo de PICC
 * =============	======================	================	==============================
 * Único			4						1				MIFARE Classic
 * Duplo			7						2				MIFARE Ultralight
 * Triplo			10						3				Não atualmente em uso?
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */

MFRC522::StatusCode MFRC522Extended::PICC_Select(Uid *uid, byte validBits)
{
	bool uidCompleto;
	bool selecaoConcluida;
	bool usarEtiquetaCascata;
	byte nivelCascata = 1;
	MFRC522::StatusCode resultado;
	byte contador;
	byte indice;
	byte indiceUID;
	int8_t bitsConhecidosNivelAtual;
	byte buffer[9];
	byte bufferUsado;
	byte alinhamentoRx;
	byte ultimosBitsTx;
	byte *bufferResposta;
	byte comprimentoResposta;

	// Verificações de validade
	if (validBits > 80)
	{
		return STATUS_INVALID;
	}

	// Prepara o MFRC522
	PCD_ClearRegisterBitMask(CollReg, 0x80);

	// Repetir o loop do nível de cascata até termos um UID completo.
	uidCompleto = false;
	while (!uidCompleto)
	{
		// Define o Nível de Cascata no byte SEL, descobre se precisamos usar a Etiqueta de Cascata no byte 2.
		switch (nivelCascata)
		{
		case 1:
			buffer[0] = PICC_CMD_SEL_CL1;
			indiceUID = 0;
			usarEtiquetaCascata = validBits && uid->size > 4;
			break;

		case 2:
			buffer[0] = PICC_CMD_SEL_CL2;
			indiceUID = 3;
			usarEtiquetaCascata = validBits && uid->size > 7;
			break;

		case 3:
			buffer[0] = PICC_CMD_SEL_CL3;
			indiceUID = 6;
			usarEtiquetaCascata = false;
			break;

		default:
			return STATUS_INTERNAL_ERROR;
			break;
		}

		// Quantos bits de UID são conhecidos neste Nível de Cascata?
		bitsConhecidosNivelAtual = validBits - (8 * indiceUID);
		if (bitsConhecidosNivelAtual < 0)
		{
			bitsConhecidosNivelAtual = 0;
		}
		// Copie os bits conhecidos de uid->uidByte[] para buffer[]
		indice = 2;
		if (usarEtiquetaCascata)
		{
			buffer[indice++] = PICC_CMD_CT;
		}
		byte bytesParaCopiar = bitsConhecidosNivelAtual / 8 + (bitsConhecidosNivelAtual % 8 ? 1 : 0);
		if (bytesParaCopiar)
		{
			byte maxBytes = usarEtiquetaCascata ? 3 : 4;
			if (bytesParaCopiar > maxBytes)
			{
				bytesParaCopiar = maxBytes;
			}
			for (contador = 0; contador < bytesParaCopiar; contador++)
			{
				buffer[indice++] = uid->uidByte[indiceUID + contador];
			}
		}
		// Agora que os dados foram copiados, precisamos incluir os 8 bits em CT em bitsConhecidosNivelAtual
		if (usarEtiquetaCascata)
		{
			bitsConhecidosNivelAtual += 8;
		}

		// Repetir o loop de anticolisão até que possamos transmitir todos os bits do UID + BCC e receber um SAK - no máximo 32 iterações.
		selecaoConcluida = false;
		while (!selecaoConcluida)
		{
			// Descubra quantos bits e bytes enviar e receber.
			if (bitsConhecidosNivelAtual >= 32)
			{
				buffer[1] = 0x70;
				buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
				resultado = PCD_CalculateCRC(buffer, 7, &buffer[7]);
				if (resultado != STATUS_OK)
				{
					return resultado;
				}
				ultimosBitsTx = 0;
				bufferUsado = 9;
				bufferResposta = &buffer[6];
				comprimentoResposta = 3;
			}
			else
			{
				ultimosBitsTx = bitsConhecidosNivelAtual % 8;
				contador = bitsConhecidosNivelAtual / 8;
				indice = 2 + contador;
				buffer[1] = (indice << 4) + ultimosBitsTx;
				bufferUsado = indice + (ultimosBitsTx ? 1 : 0);
				bufferResposta = &buffer[indice];
				comprimentoResposta = sizeof(buffer) - indice;
			}

			alinhamentoRx = ultimosBitsTx;
			PCD_WriteRegister(BitFramingReg, (alinhamentoRx << 4) + ultimosBitsTx);

			resultado = PCD_TransceiveData(buffer, bufferUsado, bufferResposta, &comprimentoResposta, &ultimosBitsTx, alinhamentoRx);
			if (resultado == STATUS_COLLISION)
			{
				byte valorCollReg = PCD_ReadRegister(CollReg);
				if (valorCollReg & 0x20)
				{
					return STATUS_COLLISION;
				}
				byte posicaoColisao = valorCollReg & 0x1F;
				if (posicaoColisao == 0)
				{
					posicaoColisao = 32;
				}
				if (posicaoColisao <= bitsConhecidosNivelAtual)
				{
					return STATUS_INTERNAL_ERROR;
				}
				bitsConhecidosNivelAtual = posicaoColisao;
				contador = (bitsConhecidosNivelAtual - 1) % 8;
				indice = 1 + (bitsConhecidosNivelAtual / 8) + (contador ? 1 : 0);
				buffer[indice] |= (1 << contador);
			}
			else if (resultado != STATUS_OK)
			{
				return resultado;
			}
			else
			{
				if (bitsConhecidosNivelAtual >= 32)
				{
					selecaoConcluida = true;
				}
				else
				{
					bitsConhecidosNivelAtual = 32;
				}
			}
		}

		if (comprimentoResposta != 3 || ultimosBitsTx != 0)
		{
			return STATUS_ERROR;
		}
		resultado = PCD_CalculateCRC(bufferResposta, 1, &buffer[2]);
		if (resultado != STATUS_OK)
		{
			return resultado;
		}
		if ((buffer[2] != bufferResposta[1]) || (buffer[3] != bufferResposta[2]))
		{
			return STATUS_CRC_WRONG;
		}
		if (bufferResposta[0] & 0x04)
		{
			nivelCascata++;
		}
		else
		{
			uidCompleto = true;
			uid->sak = bufferResposta[0];
		}
	}
}

// Define o tamanho correto de uid->size
uid->size = 3 * nivelCascata + 1;

// SE o bit 6 do SAK for 1, então é ISO/IEC 14443-4 (T=CL)
// Um comando Request ATS deve ser enviado
// Também verificamos se o bit 3 do SAK é zero, pois isso indica um UID completo (1 indicaria que está incompleto)
if ((uid->sak & 0x24) == 0x20)
{
	Ats ats;
	resultado = PICC_RequestATS(&ats);
	if (resultado == STATUS_OK)
	{
		// Verifica o ATS
		if (ats.size > 0)
		{
			// TA1 foi transmitido?
			// PPS deve ser suportado...
			if (ats.ta1.transmitted)
			{
				// TA1
				//  8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | Descrição
				// ---+---+---+---+---+---+---+---+------------------------------------------
				//  0 | - | - | - | 0 | - | - | - | Diferente D para cada direção suportada
				//  1 | - | - | - | 0 | - | - | - | Somente o mesmo D para ambas as direções suportadas
				//  - | x | x | x | 0 | - | - | - | DS (Enviar D)
				//  - | - | - | - | 0 | x | x | x | DR (Receber D)
				//
				// Tabela de D para taxa de bits
				//  3 | 2 | 1 | Valor
				// ---+---+---+-----------------------------
				//  1 | - | - | 848 kBaud é suportado
				//  - | 1 | - | 424 kBaud é suportado
				//  - | - | 1 | 212 kBaud é suportado
				//  0 | 0 | 0 | Apenas 106 kBaud é suportado
				//
				// Nota: 106 kBaud é sempre suportado
				//
				// Eu tenho quase constantes tempos limite ao alterar as velocidades :(
				// padrão nunca usado, apenas declarado
				// TagBitRates ds = BITRATE_106KBITS;
				// TagBitRates dr = BITRATE_106KBITS;
				TagBitRates ds;
				TagBitRates dr;

				//// TODO Não está funcionando em 848 ou 424
				// if (ats.ta1.ds & 0x04)
				//{
				//   ds = BITRATE_848KBITS;
				// }
				// else if (ats.ta1.ds & 0x02)
				//{
				//   ds = BITRATE_424KBITS;
				// }
				// else if (ats.ta1.ds & 0x01)
				//{
				//   ds = BITRATE_212KBITS;
				// }
				// else
				//{
				//   ds = BITRATE_106KBITS;
				// }

				if (ats.ta1.ds & 0x01)
				{
					ds = BITRATE_212KBITS;
				}
				else
				{
					ds = BITRATE_106KBITS;
				}

				//// Não está funcionando em 848 ou 424
				// if (ats.ta1.dr & 0x04)
				//{
				//   dr = BITRATE_848KBITS;
				// }
				// else if (ats.ta1.dr & 0x02)
				//{
				//   dr = BITRATE_424KBITS;
				// }
				// else if (ats.ta1.dr & 0x01)
				//{
				//   dr = BITRATE_212KBITS;
				// }
				// else
				//{
				//   dr = BITRATE_106KBITS;
				// }

				if (ats.ta1.dr & 0x01)
				{
					dr = BITRATE_212KBITS;
				}
				else
				{
					dr = BITRATE_106KBITS;
				}

				PICC_PPS(ds, dr);
			}
		}
	}
}

return STATUS_OK;
} // Fim de PICC_Select()

/**
 * Transmite um comando Request para Answer To Select (ATS).
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */

MFRC522::StatusCode MFRC522Extended::PICC_RequestATS(Ats *ats)
{
	// TODO variável não utilizada
	// byte count;
	MFRC522::StatusCode resultado;

	byte bufferATS[FIFO_SIZE];
	byte bufferSize = FIFO_SIZE;

	memset(bufferATS, 0, FIFO_SIZE);

	// Construir o buffer de comando
	bufferATS[0] = PICC_CMD_RATS;

	// O CID define o número lógico do cartão endereçado e tem um intervalo de 0
	// a 14; 15 é reservado para uso futuro (RFU).
	//
	// FSDI codifica o tamanho máximo do quadro (FSD) que o terminal pode receber.
	//
	// FSDI        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9-F
	// ------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----------
	// FSD (bytes) |  16 |  24 |  32 |  40 |  48 |  64 |  96 | 128 | 256 | RFU > 256
	//
	bufferATS[1] = 0x50; // FSD=64, CID=0

	// Calcular o CRC_A
	resultado = PCD_CalculateCRC(bufferATS, 2, &bufferATS[2]);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Transmitir o buffer e receber a resposta, validar o CRC_A.
	resultado = PCD_TransceiveData(bufferATS, 4, bufferATS, &bufferSize, NULL, 0, true);
	if (resultado != STATUS_OK)
	{
		PICC_HaltA();
	}

	// Definir os dados da estrutura ats
	ats->size = bufferATS[0];

	// Byte T0:
	//
	// b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 | Significado
	//----+----+----+----+----+----+----+----+---------------------------
	//  0 | ...| ...| ...| ...|... | ...| ...| Definido como 0 (RFU)
	//  0 |  1 | x  |  x | ...|... | ...| ...| TC1 transmitido
	//  0 |  x | 1  |  x | ...|... | ...| ...| TB1 transmitido
	//  0 |  x | x  |  1 | ...|... | ...| ...| TA1 transmitido
	//  0 | ...| ...| ...|  x |  x |  x | x  | Tamanho máximo do quadro (FSCI)
	//
	// FSCI        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9-F
	// ------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----------
	// FSC (bytes) |  16 |  24 |  32 |  40 |  48 |  64 |  96 | 128 | 256 | RFU > 256
	//
	// O valor padrão de FSCI é 2 (32 bytes)
	if (ats->size > 0x01)
	{
		// TC1, TB1 e TA1 NÃO foram transmitidos
		ats->ta1.transmitido = (bool)(bufferATS[1] & 0x40);
		ats->tb1.transmitido = (bool)(bufferATS[1] & 0x20);
		ats->tc1.transmitido = (bool)(bufferATS[1] & 0x10);

		// Decodificar FSCI
		switch (bufferATS[1] & 0x0F)
		{
		case 0x00:
			ats->fsc = 16;
			break;
		case 0x01:
			ats->fsc = 24;
			break;
		case 0x02:
			ats->fsc = 32;
			break;
		case 0x03:
			ats->fsc = 40;
			break;
		case 0x04:
			ats->fsc = 48;
			break;
		case 0x05:
			ats->fsc = 64;
			break;
		case 0x06:
			ats->fsc = 96;
			break;
		case 0x07:
			ats->fsc = 128;
			break;
		case 0x08:
			// Esse valor não pode ser mantido em um byte
			// A razão pela qual eu o ignoro é que o FIFO MFRC255 é de 64 bytes, então esse não é um valor possível (ou pelo menos não deveria ser)
			// ats->fsc = 256;
			break;
			// TODO: O que fazer com RFU (Reservado para uso futuro)?
		default:
			break;
		}

		// TA1
		if (ats->ta1.transmitido)
		{
			ats->ta1.mesmoD = (bool)(bufferATS[2] & 0x80);
			ats->ta1.ds = (TagBitRates)((bufferATS[2] & 0x70) >> 4);
			ats->ta1.dr = (TagBitRates)(bufferATS[2] & 0x07);
		}
		else
		{
			// TA1 padrão
			ats->ta1.ds = BITRATE_106KBITS;
			ats->ta1.dr = BITRATE_106KBITS;
		}

		// TB1
		if (ats->tb1.transmitido)
		{
			uint8_t tb1Index = 2;

			if (ats->ta1.transmitido)
				tb1Index++;

			ats->tb1.fwi = (bufferATS[tb1Index] & 0xF0) >> 4;
			ats->tb1.sfgi = bufferATS[tb1Index] & 0x0F;
		}
		else
		{
			// Padrões para TB1
			ats->tb1.fwi = 0;  // TODO: Não sei o padrão para isso!
			ats->tb1.sfgi = 0; // O valor padrão de SFGI é 0 (o que significa que o cartão não precisa de nenhum SFGT específico)
		}

		// TC1
		if (ats->tc1.transmitido)
		{
			uint8_t tc1Index = 2;

			if (ats->ta1.transmitido)
				tc1Index++;
			if (ats->tb1.transmitido)
				tc1Index++;

			ats->tc1.supportsCID = (bool)(bufferATS[tc1Index] & 0x02);
			ats->tc1.supportsNAD = (bool)(bufferATS[tc1Index] & 0x01);
		}
		else
		{
			// Padrões para TC1
			ats->tc1.supportsCID = true;
			ats->tc1.supportsNAD = false;
		}
	}
	else
	{
		// TC1, TB1 e TA1 NÃO foram transmitidos
		ats->ta1.transmitido = false;
		ats->tb1.transmitido = false;
		ats->tc1.transmitido = false;

		// FSCI padrão
		ats->fsc = 32; // Padrão para FSCI 2 (32 bytes)

		// TA1 padrão
		ats->ta1.mesmoD = false;
		ats->ta1.ds = BITRATE_106KBITS;
		ats->ta1.dr = BITRATE_106KBITS;

		// Padrões para TB1
		ats->tb1.transmitido = false;
		ats->tb1.fwi = 0;  // TODO: Não sei o padrão para isso!
		ats->tb1.sfgi = 0; // O valor padrão de SFGI é 0 (o que significa que o cartão não precisa de nenhum SFGT específico)

		// Padrões para TC1
		ats->tc1.transmitido = false;
		ats->tc1.supportsCID = true;
		ats->tc1.supportsNAD = false;
	}

	memcpy(ats->data, bufferATS, bufferSize - 2);

	return resultado;
} // Fim de PICC_RequestATS()

/**
 * Transmite uma Solicitação de Seleção de Protocolo e Parâmetros (PPS) sem o parâmetro 1.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522Extended::PICC_PPS()
{
	StatusCode resultado;

	byte bufferPPS[4];
	byte tamanhoBufferPPS = 4;
	// Byte inicial: O byte inicial (PPS) consiste em duas partes:
	//  – O nibble superior (b8–b5) é definido como 'D' para identificar o PPS. Todos os outros valores são RFU.
	//  - O nibble inferior (b4–b1), chamado de 'identificador do cartão' (CID), define o número lógico do cartão endereçado.
	bufferPPS[0] = 0xD0; // O CID é fixo como 0 em RATS
	bufferPPS[1] = 0x00; // PPS0 indica se o PPS1 está presente

	// Calcular o CRC_A
	resultado = PCD_CalculateCRC(bufferPPS, 2, &bufferPPS[2]);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Transmitir o buffer e receber a resposta, validar o CRC_A.
	resultado = PCD_TransceiveData(bufferPPS, 4, bufferPPS, &tamanhoBufferPPS, NULL, 0, true);
	if (resultado == STATUS_OK)
	{
		// Habilitar CRC para T=CL
		byte registroTx = PCD_ReadRegister(TxModeReg) | 0x80;
		byte registroRx = PCD_ReadRegister(RxModeReg) | 0x80;

		PCD_WriteRegister(TxModeReg, registroTx);
		PCD_WriteRegister(RxModeReg, registroRx);
	}

	return resultado;
} // Fim de PICC_PPS()

/**
 * Transmite uma Solicitação de Seleção de Protocolo e Parâmetros (PPS).
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522Extended::PICC_PPS(TagBitRates taxaEnvio,   ///< DS
											  TagBitRates taxaRecepcao ///< DR
)
{
	StatusCode resultado;

	byte bufferPPS[5];
	byte tamanhoBufferPPS = 5;
	// Byte inicial: O byte inicial (PPS) consiste em duas partes:
	//  – O nibble superior (b8–b5) é definido como 'D' para identificar o PPS. Todos os outros valores são RFU.
	//  - O nibble inferior (b4–b1), chamado de 'identificador do cartão' (CID), define o número lógico do cartão endereçado.
	bufferPPS[0] = 0xD0; // O CID é fixo como 0 em RATS
	bufferPPS[1] = 0x11; // PPS0 indica se o PPS1 está presente

	// Bit 8 - Definido como '0', já que o MFRC522 permite diferentes taxas de bits para envio e recebimento
	// Bit 4 - Definido como '0', pois é Reservado para uso futuro.
	// bufferPPS[2] = (((taxaEnvio & 0x03) << 4) | (taxaRecepcao & 0x03)) & 0xE7;
	bufferPPS[2] = (((taxaEnvio & 0x03) << 2) | (taxaRecepcao & 0x03)) & 0xE7;

	// Calcular o CRC_A
	resultado = PCD_CalculateCRC(bufferPPS, 3, &bufferPPS[3]);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Transmitir o buffer e receber a resposta, validar o CRC_A.
	resultado = PCD_TransceiveData(bufferPPS, 5, bufferPPS, &tamanhoBufferPPS, NULL, 0, true);
	if (resultado == STATUS_OK)
	{
		// Certifique-se de que é uma resposta ao nosso PPS
		// Deveríamos receber nosso byte PPS e 2 bytes de CRC
		if ((tamanhoBufferPPS == 3) && (bufferPPS[0] == 0xD0))
		{
			byte registroTx = PCD_ReadRegister(TxModeReg) & 0x8F;
			byte registroRx = PCD_ReadRegister(RxModeReg) & 0x8F;

			// Configurar taxa de bits e habilitar CRC para T=CL
			registroTx = (registroTx & 0x8F) | ((taxaRecepcao & 0x03) << 4) | 0x80;
			registroRx = (registroRx & 0x8F) | ((taxaEnvio & 0x03) << 4) | 0x80;
			registroRx &= 0xF0; // Garantir que isso já esteja configurado

			// De ConfigIsoType
			// registroRx |= 0x06;

			PCD_WriteRegister(TxModeReg, registroTx);
			PCD_WriteRegister(RxModeReg, registroRx);

			// A 212 kBps
			switch (taxaEnvio)
			{
			case BITRATE_212KBITS:
			{
				// PCD_WriteRegister(ModWidthReg, 0x13);
				PCD_WriteRegister(ModWidthReg, 0x15);
			}
			break;
			case BITRATE_424KBITS:
			{
				PCD_WriteRegister(ModWidthReg, 0x0A);
			}
			break;
			case BITRATE_848KBITS:
			{
				PCD_WriteRegister(ModWidthReg, 0x05);
			}
			break;
			default:
			{
				PCD_WriteRegister(ModWidthReg, 0x26); // Valor padrão
			}
			break;
			}

			// PCD_WriteRegister(RxThresholdReg, 0x84); // ISO-14443.4 Tipo A (padrão)
			// PCD_WriteRegister(ControlReg, 0x10);

			delayMicroseconds(10);
		}
		else
		{
			return STATUS_ERROR;
		}
	}

	return resultado;
} // Fim de PICC_PPS()

/////////////////////////////////////////////////////////////////////////////////////
// Funções para comunicação com cartões ISO/IEC 14433-4
/////////////////////////////////////////////////////////////////////////////////////

MFRC522::StatusCode MFRC522Extended::TCL_Transceive(PcbBlock *enviar, PcbBlock *retorno)
{
	MFRC522::StatusCode resultado;
	byte bufferEntrada[FIFO_SIZE];
	byte tamanhoBufferEntrada = FIFO_SIZE;
	byte bufferSaida[enviar->inf.size + 5]; // PCB + CID + NAD + INF + EPILOGUE (CRC)
	byte offsetBufferSaida = 1;
	byte offsetBufferEntrada = 1;

	// Definir o byte PCB
	bufferSaida[0] = enviar->prologue.pcb;

	// Definir o byte CID, se disponível
	if (enviar->prologue.pcb & 0x08)
	{
		bufferSaida[offsetBufferSaida] = enviar->prologue.cid;
		offsetBufferSaida++;
	}

	// Definir o byte NAD, se disponível
	if (enviar->prologue.pcb & 0x04)
	{
		bufferSaida[offsetBufferSaida] = enviar->prologue.nad;
		offsetBufferSaida++;
	}

	// Copiar o campo INF, se disponível
	if (enviar->inf.size > 0)
	{
		memcpy(&bufferSaida[offsetBufferSaida], enviar->inf.data, enviar->inf.size);
		offsetBufferSaida += enviar->inf.size;
	}

	// O CRC está habilitado para transmissão?
	byte registroTxMode = PCD_ReadRegister(TxModeReg);
	if ((registroTxMode & 0x80) != 0x80)
	{
		// Calcular o CRC_A
		resultado = PCD_CalculateCRC(bufferSaida, offsetBufferSaida, &bufferSaida[offsetBufferSaida]);
		if (resultado != STATUS_OK)
		{
			return resultado;
		}

		offsetBufferSaida += 2;
	}

	// Transmitir o bloco
	resultado = PCD_TransceiveData(bufferSaida, offsetBufferSaida, bufferEntrada, &tamanhoBufferEntrada);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Queremos transformar o array recebido de volta em um PcbBlock
	retorno->prologue.pcb = bufferEntrada[0];

	// O byte CID está presente?
	if (enviar->prologue.pcb & 0x08)
	{
		retorno->prologue.cid = bufferEntrada[offsetBufferEntrada];
		offsetBufferEntrada++;
	}

	// O byte NAD está presente?
	if (enviar->prologue.pcb & 0x04)
	{
		retorno->prologue.nad = bufferEntrada[offsetBufferEntrada];
		offsetBufferEntrada++;
	}

	// Verificar se o CRC é tratado pelo MFRC522
	byte registroRxMode = PCD_ReadRegister(TxModeReg);
	if ((registroRxMode & 0x80) != 0x80)
	{
		Serial.print("CRC não é tratado pelo MFRC522: ");
		Serial.println(registroRxMode, HEX);

		// Verificar o CRC
		// Precisamos pelo menos do valor CRC_A.
		if ((int)(tamanhoBufferEntrada - offsetBufferEntrada) < 2)
		{
			return STATUS_CRC_WRONG;
		}

		// Verificar o CRC_A - fazer nosso próprio cálculo e armazenar o controle em controlBuffer.
		byte controlBuffer[2];
		MFRC522::StatusCode status = PCD_CalculateCRC(bufferEntrada, tamanhoBufferEntrada - 2, controlBuffer);
		if (status != STATUS_OK)
		{
			return status;
		}

		if ((bufferEntrada[tamanhoBufferEntrada - 2] != controlBuffer[0]) || (bufferEntrada[tamanhoBufferEntrada - 1] != controlBuffer[1]))
		{
			return STATUS_CRC_WRONG;
		}

		// Remover os bytes CRC
		tamanhoBufferEntrada -= 2;
	}

	// Recebeu mais dados?
	if (tamanhoBufferEntrada > offsetBufferEntrada)
	{
		if ((tamanhoBufferEntrada - offsetBufferEntrada) > retorno->inf.size)
		{
			return STATUS_NO_ROOM;
		}

		memcpy(retorno->inf.data, &bufferEntrada[offsetBufferEntrada], tamanhoBufferEntrada - offsetBufferEntrada);
		retorno->inf.size = tamanhoBufferEntrada - offsetBufferEntrada;
	}
	else
	{
		retorno->inf.size = 0;
	}

	// Se a resposta for um bloco R, verificar o NACK
	if (((bufferEntrada[0] & 0xC0) == 0x80) && (bufferEntrada[0] & 0x20))
	{
		return STATUS_MIFARE_NACK;
	}

	return resultado;
}
/**
 * Enviar um I-Block (Aplicação)
 */

MFRC522::StatusCode MFRC522Extended::TCL_Transceive(TagInfo *tag, byte *sendData, byte sendLen, byte *backData, byte *backLen)
{
	MFRC522::StatusCode resultado;

	PcbBlock out;
	PcbBlock in;
	byte outBuffer[FIFO_SIZE];
	byte outBufferSize = FIFO_SIZE;
	byte totalBackLen = *backLen;

	// Este comando envia um bloco I
	out.prologue.pcb = 0x02;

	if (tag->ats.tc1.supportsCID)
	{
		out.prologue.pcb |= 0x08;
		out.prologue.cid = 0x00; // O CID está atualmente codificado como 0x00
	}

	// Este comando não suporta NAD
	out.prologue.pcb &= 0xFB;
	out.prologue.nad = 0x00;

	// Define o número do bloco
	if (tag->blockNumber)
	{
		out.prologue.pcb |= 0x01;
	}

	// Temos dados para enviar?
	if (sendData && (sendLen > 0))
	{
		out.inf.size = sendLen;
		out.inf.data = sendData;
	}
	else
	{
		out.inf.size = 0;
		out.inf.data = NULL;
	}

	// Inicializa os dados de recepção
	// Atenção: O valor escapa do escopo local
	in.inf.data = outBuffer;
	in.inf.size = outBufferSize;

	resultado = TCL_Transceive(&out, &in);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Troca o número do bloco em caso de sucesso
	tag->blockNumber = !tag->blockNumber;

	if (backData && (backLen > 0))
	{
		if (*backLen < in.inf.size)
			return STATUS_NO_ROOM;

		*backLen = in.inf.size;
		memcpy(backData, in.inf.data, in.inf.size);
	}

	// Verifica se há encadeamento
	if ((in.prologue.pcb & 0x10) == 0x00)
		return resultado;

	// O resultado está encadeado
	// Envia um ACK para receber mais dados
	// Atenção: Deve ser verificado, nunca precisei enviar um ACK
	while (in.prologue.pcb & 0x10)
	{
		byte ackData[FIFO_SIZE];
		byte ackDataSize = FIFO_SIZE;

		resultado = TCL_TransceiveRBlock(tag, true, ackData, &ackDataSize);
		if (resultado != STATUS_OK)
			return resultado;

		if (backData && (backLen > 0))
		{
			if ((*backLen + ackDataSize) > totalBackLen)
				return STATUS_NO_ROOM;

			memcpy(&(backData[*backLen]), ackData, ackDataSize);
			*backLen += ackDataSize;
		}
	}

	return resultado;
} // Fim de TCL_Transceive()

/**
 * Envia um bloco R para o PICC.
 */
MFRC522::StatusCode MFRC522Extended::TCL_TransceiveRBlock(TagInfo *tag, bool ack, byte *backData, byte *backLen)
{
	MFRC522::StatusCode resultado;

	PcbBlock out;
	PcbBlock in;
	byte outBuffer[FIFO_SIZE];
	byte outBufferSize = FIFO_SIZE;

	// Este comando envia um bloco R
	if (ack)
		out.prologue.pcb = 0xA2; // ACK
	else
		out.prologue.pcb = 0xB2; // NAK

	if (tag->ats.tc1.supportsCID)
	{
		out.prologue.pcb |= 0x08;
		out.prologue.cid = 0x00; // O CID está atualmente codificado como 0x00
	}

	// Este comando não suporta NAD
	out.prologue.pcb &= 0xFB;
	out.prologue.nad = 0x00;

	// Define o número do bloco
	if (tag->blockNumber)
	{
		out.prologue.pcb |= 0x01;
	}

	// Sem dados INF para o bloco R
	out.inf.size = 0;
	out.inf.data = NULL;

	// Inicializa os dados de recepção
	// Atenção: O valor escapa do escopo local
	in.inf.data = outBuffer;
	in.inf.size = outBufferSize;

	resultado = TCL_Transceive(&out, &in);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Troca o número do bloco em caso de sucesso
	tag->blockNumber = !tag->blockNumber;

	if (backData && backLen)
	{
		if (*backLen < in.inf.size)
			return STATUS_NO_ROOM;

		*backLen = in.inf.size;
		memcpy(backData, in.inf.data, in.inf.size);
	}

	return resultado;
} // Fim de TCL_TransceiveRBlock()

/**
 * Envia um bloco S para desselecionar o cartão.
 */

MFRC522::StatusCode MFRC522Extended::TCL_Deselect(TagInfo *tag)
{
	MFRC522::StatusCode resultado;
	byte outBuffer[4];
	byte outBufferSize = 1;
	byte inBuffer[FIFO_SIZE];
	byte inBufferSize = FIFO_SIZE;

	outBuffer[0] = 0xC2;
	if (tag->ats.tc1.supportsCID)
	{
		outBuffer[0] |= 0x08;
		outBuffer[1] = 0x00; // O CID está codificado como fixo
		outBufferSize = 2;
	}

	resultado = PCD_TransceiveData(outBuffer, outBufferSize, inBuffer, &inBufferSize);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// TODO: Talvez fazer algumas verificações? Nos meus testes, retorna: CA 00 (Mesmos dados que enviei para o meu cartão)

	return resultado;
} // Fim de TCL_Deselect()

/////////////////////////////////////////////////////////////////////////////////////
// Funções de suporte
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Obtém o tipo de PICC.
 *
 * @return PICC_Type
 */
MFRC522::PICC_Type MFRC522Extended::PICC_GetType(TagInfo *tag ///< A TagInfo retornada pelo PICC_Select().
)
{
	// http://www.nxp.com/documents/application_note/AN10833.pdf
	// 3.2 Codificação do Select Acknowledge (SAK)
	// Ignora 8 bits (iso14443 começa com LSBit = bit 1)
	// Corrige o tipo errado para o fabricante Infineon (http://nfc-tools.org/index.php?title=ISO14443A)
	byte sak = tag->uid.sak & 0x7F;
	switch (sak)
	{
	case 0x04:
		return PICC_TYPE_NOT_COMPLETE; // UID não completo
	case 0x09:
		return PICC_TYPE_MIFARE_MINI;
	case 0x08:
		return PICC_TYPE_MIFARE_1K;
	case 0x18:
		return PICC_TYPE_MIFARE_4K;
	case 0x00:
		return PICC_TYPE_MIFARE_UL;
	case 0x10:
	case 0x11:
		return PICC_TYPE_MIFARE_PLUS;
	case 0x01:
		return PICC_TYPE_TNP3XXX;
	case 0x20:
		if (tag->atqa == 0x0344)
			return PICC_TYPE_MIFARE_DESFIRE;
		return PICC_TYPE_ISO_14443_4;
	case 0x40:
		return PICC_TYPE_ISO_18092;
	default:
		return PICC_TYPE_UNKNOWN;
	}
} // Fim de PICC_GetType()

/**
 * Exibe informações de depuração sobre o PICC selecionado no Serial.
 * Em caso de sucesso, o PICC é parado após a exibição dos dados.
 * Para MIFARE Classic, é tentada a chave padrão de fábrica 0xFFFFFFFFFFFF.
 */
void MFRC522Extended::PICC_DumpToSerial(TagInfo *tag)
{
	MIFARE_Key chave;

	// Exibe UID, SAK e Tipo
	PICC_DumpDetailsToSerial(tag);

	// Exibe o conteúdo
	PICC_Type piccType = MFRC522::PICC_GetType(tag->uid.sak);
	switch (piccType)
	{
	case PICC_TYPE_MIFARE_MINI:
	case PICC_TYPE_MIFARE_1K:
	case PICC_TYPE_MIFARE_4K:
		// Todas as chaves são definidas como FFFFFFFFFFFFh na entrega da fábrica.
		for (byte i = 0; i < 6; i++)
		{
			chave.keyByte[i] = 0xFF;
		}
		PICC_DumpMifareClassicToSerial(&tag->uid, piccType, &chave);
		break;

	case PICC_TYPE_MIFARE_UL:
		PICC_DumpMifareUltralightToSerial();
		break;

	case PICC_TYPE_ISO_14443_4:
	case PICC_TYPE_MIFARE_DESFIRE:
		PICC_DumpISO14443_4(tag);
		Serial.println(F("Despejo de conteúdo de memória não implementado para esse tipo de PICC."));
		break;
	case PICC_TYPE_ISO_18092:
	case PICC_TYPE_MIFARE_PLUS:
	case PICC_TYPE_TNP3XXX:
		Serial.println(F("Despejo de conteúdo de memória não implementado para esse tipo de PICC."));
		break;

	case PICC_TYPE_UNKNOWN:
	case PICC_TYPE_NOT_COMPLETE:
	default:
		break; // Nenhum despejo de memória aqui
	}

	Serial.println();
	PICC_HaltA(); // Já foi feito se for um PICC MIFARE Classic.
}

/**
 * Exibe informações do cartão (UID, SAK, Tipo) sobre o PICC selecionado no Serial.
 */
void MFRC522Extended::PICC_DumpDetailsToSerial(TagInfo *tag ///< Ponteiro para a estrutura TagInfo retornada de um PICC_Select() bem-sucedido.
)
{
	// ATQA
	Serial.print(F("Cartão ATQA:"));
	if (((tag->atqa & 0xFF00u) >> 8) < 0x10)
		Serial.print(F(" 0"));
	Serial.print((tag->atqa & 0xFF00u) >> 8, HEX);
	if ((tag->atqa & 0x00FFu) < 0x10)
		Serial.print(F("0"));
	else
		Serial.print(F(" "));
	Serial.println(tag->atqa & 0x00FFu, HEX);

	// UID
	Serial.print(F("Cartão UID:"));
	for (byte i = 0; i < tag->uid.size; i++)
	{
		if (tag->uid.uidByte[i] < 0x10)
			Serial.print(F(" 0"));
		else
			Serial.print(F(" "));
		Serial.print(tag->uid.uidByte[i], HEX);
	}
	Serial.println();

	// SAK
	Serial.print(F("Cartão SAK: "));
	if (tag->uid.sak < 0x10)
		Serial.print(F("0"));
	Serial.println(tag->uid.sak, HEX);

	// Tipo PICC (sugerido)
	PICC_Type piccType = PICC_GetType(tag);
	Serial.print(F("Tipo PICC: "));
	Serial.println(PICC_GetTypeName(piccType));
} // Fim de PICC_DumpDetailsToSerial()

/**
 * Exibe o conteúdo da memória de um PICC ISO-14443-4.
 */
void MFRC522Extended::PICC_DumpISO14443_4(TagInfo *tag)
{
	// ATS
	if (tag->ats.size > 0x00)
	{ // O primeiro byte é o comprimento do ATS, incluindo o byte de comprimento
		Serial.print(F("Cartão ATS:"));
		for (byte offset = 0; offset < tag->ats.size; offset++)
		{
			if (tag->ats.data[offset] < 0x10)
				Serial.print(F(" 0"));
			else
				Serial.print(F(" "));
			Serial.print(tag->ats.data[offset], HEX);
		}
		Serial.println();
	}

} // Fim de PICC_DumpISO14443_4

/////////////////////////////////////////////////////////////////////////////////////
// Funções de conveniência - não adicionam funcionalidades adicionais
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Retorna verdadeiro se um PICC responde a PICC_CMD_REQA.
 * Somente "novos" cartões no estado IDLE são convidados. Cartões em estado HALT são ignorados.
 *
 * @return bool
 */

/**
 * Verifica se um novo cartão está presente.
 *
 * @return bool
 */
bool MFRC522Extended::PICC_IsNewCardPresent()
{
	byte bufferATQA[2];
	byte bufferSize = sizeof(bufferATQA);

	// Redefine as taxas de baud
	PCD_WriteRegister(TxModeReg, 0x00);
	PCD_WriteRegister(RxModeReg, 0x00);
	// Redefine ModWidthReg
	PCD_WriteRegister(ModWidthReg, 0x26);

	MFRC522::StatusCode result = PICC_RequestA(bufferATQA, &bufferSize);

	if (result == STATUS_OK || result == STATUS_COLLISION)
	{
		tag.atqa = ((uint16_t)bufferATQA[1] << 8) | bufferATQA[0];
		tag.ats.size = 0;
		tag.ats.fsc = 32; // valor FSC padrão

		// Padrões para TA1
		tag.ats.ta1.transmitted = false;
		tag.ats.ta1.sameD = false;
		tag.ats.ta1.ds = MFRC522Extended::BITRATE_106KBITS;
		tag.ats.ta1.dr = MFRC522Extended::BITRATE_106KBITS;

		// Padrões para TB1
		tag.ats.tb1.transmitted = false;
		tag.ats.tb1.fwi = 0;  // TODO: Não conheço o valor padrão para isso!
		tag.ats.tb1.sfgi = 0; // O valor padrão de SFGI é 0 (o que significa que o cartão não precisa de nenhum SFGT específico)

		// Padrões para TC1
		tag.ats.tc1.transmitted = false;
		tag.ats.tc1.supportsCID = true;
		tag.ats.tc1.supportsNAD = false;

		memset(tag.ats.data, 0, FIFO_SIZE - 2);

		tag.blockNumber = false;
		return true;
	}
	return false;
} // Fim de PICC_IsNewCardPresent()

/**
 * Função simples que chama PICC_Select.
 * Retorna verdadeiro se um UID puder ser lido.
 * Lembre-se de chamar PICC_IsNewCardPresent(), PICC_RequestA() ou PICC_WakeupA() primeiro.
 * O UID lido está disponível na variável de classe uid.
 *
 * @return bool
 */
bool MFRC522Extended::PICC_ReadCardSerial()
{
	MFRC522::StatusCode result = PICC_Select(&tag.uid);

	// Compatibilidade com versões anteriores
	uid.size = tag.uid.size;
	uid.sak = tag.uid.sak;
	memcpy(uid.uidByte, tag.uid.uidByte, sizeof(tag.uid.uidByte));

	return (result == STATUS_OK);
} // Fim
