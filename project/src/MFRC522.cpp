/*
 * MFRC522.cpp - Biblioteca para usar o MÓDULO ARDUINO RFID KIT 13,56 MHZ COM TAGS SPI W E R DA COOQROBOT.
 * OBSERVAÇÃO: Por favor, verifique também os comentários em MFRC522.h - eles fornecem dicas úteis e informações de fundo.
 * Liberado para o domínio público.
 */

#include <Arduino.h>
#include "MFRC522.h"

/////////////////////////////////////////////////////////////////////////////////////
// Funções para configurar o Arduino
/////////////////////////////////////////////////////////////////////////////////////
/**
 * Construtor.
 */
MFRC522::MFRC522() : MFRC522(SS, UINT8_MAX)
{ // SS é definido em pins_arduino.h, UINT8_MAX significa que não há conexão do Arduino para a entrada de reset e desligamento do MFRC522
} // Fim do construtor

/**
 * Construtor.
 * Prepara as saídas.
 */
MFRC522::MFRC522(byte resetPowerDownPin) : MFRC522(SS, resetPowerDownPin)
{ // SS é definido em pins_arduino.h
} // Fim do construtor

/**
 * Construtor.
 * Prepara as saídas.
 */
MFRC522::MFRC522(byte chipSelectPin, byte resetPowerDownPin)
{
	_chipSelectPin = chipSelectPin;
	_resetPowerDownPin = resetPowerDownPin;
} // Fim do construtor

/////////////////////////////////////////////////////////////////////////////////////
// Funções de interface básica para comunicação com o MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Escreve um byte no registro especificado no chip MFRC522.
 * A interface é descrita na seção 8.1.2 do datasheet.
 */
void MFRC522::PCD_WriteRegister(PCD_Register reg, byte value)
{
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0)); // Configurações para trabalhar com o barramento SPI
	digitalWrite(_chipSelectPin, LOW);										  // Seleciona o escravo
	SPI.transfer(reg);														  // MSB == 0 é para escrita. LSB não é usado no endereço. Seção 8.1.2.3 do datasheet.
	SPI.transfer(value);
	digitalWrite(_chipSelectPin, HIGH); // Libera o escravo
	SPI.endTransaction();				// Para de usar o barramento SPI
} // Fim de PCD_WriteRegister()

/**
 * Escreve um número de bytes no registro especificado no chip MFRC522.
 * A interface é descrita na seção 8.1.2 do datasheet.
 */
void MFRC522::PCD_WriteRegister(PCD_Register reg, byte count, byte *values)
{
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0)); // Configurações para trabalhar com o barramento SPI
	digitalWrite(_chipSelectPin, LOW);										  // Seleciona o escravo
	SPI.transfer(reg);														  // MSB == 0 é para escrita. LSB não é usado no endereço. Seção 8.1.2.3 do datasheet.
	for (byte index = 0; index < count; index++)
	{
		SPI.transfer(values[index]);
	}
	digitalWrite(_chipSelectPin, HIGH); // Libera o escravo
	SPI.endTransaction();				// Para de usar o barramento SPI
} // Fim de PCD_WriteRegister()

/**
 * Lê um byte do registro especificado no chip MFRC522.
 * A interface é descrita na seção 8.1.2 do datasheet.
 */
byte MFRC522::PCD_ReadRegister(PCD_Register reg)
{
	byte value;
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0)); // Configurações para trabalhar com o barramento SPI
	digitalWrite(_chipSelectPin, LOW);										  // Seleciona o escravo
	SPI.transfer(0x80 | reg);												  // MSB == 1 é para leitura. LSB não é usado no endereço. Seção 8.1.2.3 do datasheet.
	value = SPI.transfer(0);												  // Lê o valor de volta. Envia 0 para parar a leitura.
	digitalWrite(_chipSelectPin, HIGH);										  // Libera o escravo
	SPI.endTransaction();													  // Para de usar o barramento SPI
	return value;
} // Fim de PCD_ReadRegister()

/**
 * Lê um número de bytes do registro especificado no chip MFRC522.
 * A interface é descrita na seção 8.1.2 do datasheet.
 */
void MFRC522::PCD_ReadRegister(PCD_Register reg, byte count, byte *values, byte rxAlign)
{
	if (count == 0)
	{
		return;
	}
	// Serial.print(F("Lendo ")); 	Serial.print(count); Serial.println(F(" bytes do registro."));
	byte address = 0x80 | reg;												  // MSB == 1 é para leitura. LSB não é usado no endereço. Seção 8.1.2.3 do datasheet.
	byte index = 0;															  // Índice no array de valores.
	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0)); // Configurações para trabalhar com o barramento SPI
	digitalWrite(_chipSelectPin, LOW);										  // Seleciona o escravo
	count--;																  // Uma leitura é realizada fora do loop
	SPI.transfer(address);													  // Diz ao MFRC522 qual endereço queremos ler
	if (rxAlign)
	{ // Atualiza apenas as posições de bit rxAlign..7 em values[0]
		// Crie uma máscara de bits para as posições de bit rxAlign..7
		byte mask = (0xFF << rxAlign) & 0xFF;
		// Leia o valor e diga que queremos ler o mesmo endereço novamente.
		byte value = SPI.transfer(address);
		// Aplique a máscara ao valor atual de values[0] e aos novos dados em value.
		values[0] = (values[0] & ~mask) | (value & mask);
		index++;
	}
	while (index < count)
	{
		values[index] = SPI.transfer(address); // Leia o valor e diga que queremos ler o mesmo endereço novamente.
		index++;
	}
	values[index] = SPI.transfer(0);	// Leia o último byte. Envie 0 para parar a leitura.
	digitalWrite(_chipSelectPin, HIGH); // Libera o escravo
	SPI.endTransaction();				// Para de usar o barramento SPI
} // Fim de PCD_ReadRegister()

/**
 * Define os bits dados em mask no registro reg.
 */
void MFRC522::PCD_SetRegisterBitMask(PCD_Register reg, byte mask)
{
	byte tmp;
	tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp | mask); // Define a máscara de bits
} // Fim de PCD_SetRegisterBitMask()

/**
 * Limpa os bits dados em mask do registro reg.
 */
void MFRC522::PCD_ClearRegisterBitMask(PCD_Register reg, byte mask)
{
	byte tmp;
	tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp & (~mask)); // Limpa a máscara de bits
} // Fim de PCD_ClearRegisterBitMask()

/**
 * Usa o coprocessador CRC no MFRC522 para calcular um CRC_A.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PCD_CalculateCRC(byte *data, byte length, byte *result)
{
	PCD_WriteRegister(CommandReg, PCD_Idle);	  // Pare qualquer comando ativo.
	PCD_WriteRegister(DivIrqReg, 0x04);			  // Limpe o bit de solicitação de interrupção CRCIRq
	PCD_WriteRegister(FIFOLevelReg, 0x80);		  // FlushBuffer = 1, inicialização FIFO
	PCD_WriteRegister(FIFODataReg, length, data); // Escreva dados no FIFO
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);	  // Inicie o cálculo

	// Aguarde o cálculo do CRC ser concluído. Verifique o registro para
	// indicar que o cálculo do CRC foi concluído em um loop. Se o
	// cálculo não for indicado como concluído em ~90ms, então ocorre
	// o timeout da operação.
	const uint32_t deadline = millis() + 89;

	do
	{
		// Os bits DivIrqReg[7..0] são: Set2 reservado reservado MfinActIRq reservado CRCIRq reservado reservado
		byte n = PCD_ReadRegister(DivIrqReg);
		if (n & 0x04)
		{											 // Bit CRCIRq definido - cálculo concluído
			PCD_WriteRegister(CommandReg, PCD_Idle); // Pare o cálculo CRC para um novo conteúdo no FIFO.
			// Transfira o resultado dos registradores para o buffer de resultados
			result[0] = PCD_ReadRegister(CRCResultRegL);
			result[1] = PCD_ReadRegister(CRCResultRegH);
			return STATUS_OK;
		}
		yield();
	} while (static_cast<uint32_t>(millis()) < deadline);

	// Passaram-se 89ms e nada aconteceu. A comunicação com o MFRC522 pode estar desativada.
	return STATUS_TIMEOUT;
} // Fim de PCD_CalculateCRC()

/////////////////////////////////////////////////////////////////////////////////////
// Funções para manipular o MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Inicializa o chip MFRC522.
 */
void MFRC522::PCD_Init()
{
	bool hardReset = false;

	// Defina o chipSelectPin como saída digital, mas não selecione o escravo ainda
	pinMode(_chipSelectPin, OUTPUT);
	digitalWrite(_chipSelectPin, HIGH);

	// Se um número de pino válido foi definido, retire o dispositivo do estado de desligamento / reset do MFRC522.
	if (_resetPowerDownPin != UNUSED_PIN)
	{
		// Primeiro, defina o resetPowerDownPin como entrada digital para verificar o modo de desligamento do MFRC522.
		pinMode(_resetPowerDownPin, INPUT);

		if (digitalRead(_resetPowerDownPin) == LOW)
		{											// O chip MFRC522 está no modo de desligamento.
			pinMode(_resetPowerDownPin, OUTPUT);	// Agora defina o resetPowerDownPin como saída digital.
			digitalWrite(_resetPowerDownPin, LOW);	// Certifique-se de que temos um estado LOW limpo.
			delayMicroseconds(2);					// A seção 8.8.1 dos requisitos de tempo de reset diz cerca de 100ns. Vamos ser generosos: 2μs.
			digitalWrite(_resetPowerDownPin, HIGH); // Saia do modo de desligamento. Isso desencadeia um reset duro.
			// A seção 8.8.2 do datasheet diz que o tempo de inicialização do oscilador é o tempo de inicialização do cristal + 37,74μs. Vamos ser generosos: 50ms.
			delay(50);
			hardReset = true;
		}
	}

	if (!hardReset)
	{ // Realize um reset suave se não tivermos acionado um reset duro acima.
		PCD_Reset();
	}

	// Redefina as taxas de baud
	PCD_WriteRegister(TxModeReg, 0x00);
	PCD_WriteRegister(RxModeReg, 0x00);
	// Redefina ModWidthReg
	PCD_WriteRegister(ModWidthReg, 0x26);

	// Ao comunicar com um PICC, precisamos de um timeout se algo der errado.
	// f_timer = 13.56 MHz / (2*TPreScaler+1) onde TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
	// TPrescaler_Hi são os quatro bits baixos em TModeReg. TPrescaler_Lo é TPrescalerReg.
	PCD_WriteRegister(TModeReg, 0x80);		// TAuto=1; o temporizador começa automaticamente no final da transmissão em todos os modos de comunicação em todas as velocidades
	PCD_WriteRegister(TPrescalerReg, 0xA9); // TPreScaler = TModeReg[3..0]:TPrescalerReg, ou seja, 0x0A9 = 169 => f_timer=40kHz, ou seja, um período de temporização de 25μs.
	PCD_WriteRegister(TReloadRegH, 0x03);	// Recarregar temporizador com 0x3E8 = 1000, ou seja, 25ms antes do timeout.
	PCD_WriteRegister(TReloadRegL, 0xE8);

	PCD_WriteRegister(TxASKReg, 0x40); // Padrão 0x00. Força uma modulação ASK de 100 % independente da configuração do registro ModGsPReg
	PCD_WriteRegister(ModeReg, 0x3D);  // Padrão 0x3F. Defina o valor predefinido para o coprocessador CRC para o comando CalcCRC como 0x6363 (ISO 14443-3 parte 6.2.4)
	PCD_AntennaOn();				   // Ative os pinos do driver da antena TX1 e TX2 (eles foram desativados pelo reset)
} // Fim de PCD_Init()

/**
 * Inicializa o chip MFRC522.
 */
void MFRC522::PCD_Init(byte resetPowerDownPin)
{
	PCD_Init(SS, resetPowerDownPin); // SS é definido em pins_arduino.h
} // Fim de PCD_Init()

/**
 * Inicializa o chip MFRC522.
 */
void MFRC522::PCD_Init(byte chipSelectPin, byte resetPowerDownPin)
{
	_chipSelectPin = chipSelectPin;
	_resetPowerDownPin = resetPowerDownPin;
	// Defina o chipSelectPin como saída digital, mas não selecione o escravo ainda
	PCD_Init();
} // Fim de PCD_Init()

/**
 * Executa um reset suave no chip MFRC522 e aguarda que ele esteja pronto novamente.
 */
void MFRC522::PCD_Reset()
{
	PCD_WriteRegister(CommandReg, PCD_SoftReset); // Emita o comando SoftReset.
	// O datasheet não menciona quanto tempo leva para o comando SoftReset ser concluído.
	// Mas o MFRC522 pode ter estado em modo de desligamento suave (acionado pelo bit 4 do CommandReg)
	// A seção 8.8.2 do datasheet diz que o tempo de inicialização do oscilador é o tempo de inicialização do cristal + 37,74μs. Vamos ser generosos: 50ms.
	uint8_t count = 0;
	do
	{
		// Aguarde o bit PowerDown em CommandReg ser limpo (máximo 3x50ms)
		delay(50);
	} while ((PCD_ReadRegister(CommandReg) & (1 << 4)) && (++count) < 3);
} // Fim de PCD_Reset()

/**
 * Liga a antena ativando os pinos TX1 e TX2.
 * Após um reset, esses pinos são desativados.
 */
void MFRC522::PCD_AntennaOn()
{
	byte value = PCD_ReadRegister(TxControlReg);
	if ((value & 0x03) != 0x03)
	{
		PCD_WriteRegister(TxControlReg, value | 0x03);
	}
} // Fim de PCD_AntennaOn()

/**
 * Desliga a antena desativando os pinos TX1 e TX2.
 */
void MFRC522::PCD_AntennaOff()
{
	PCD_ClearRegisterBitMask(TxControlReg, 0x03);
} // Fim de PCD_AntennaOff()

// !!!!!!!!!! COMENTADOOOOOOOOOOOOOOOO
/**
 * Obtém o ganho da antena atual do MFRC522 (RxGain[2:0]).
 * Consulte 9.3.3.6 / Tabela 98 em http://www.nxp.com/documents/data_sheet/MFRC522.pdf
 * OBSERVAÇÃO: O valor de retorno é ajustado com (0x07 << 4) = 01110000b, pois o RCFfgReg pode usar bits reservados.
 *
 * @return Valor do ganho da antena Rx, ajustado para os 3 bits usados.
 */
byte MFRC522::PCD_GetAntennaGain()
{
	return PCD_ReadRegister(RFCfgReg) & (0x07 << 4);
} // Fim de PCD_GetAntennaGain()

/**
 * Define o Ganho do Receptor MFRC522 (RxGain) para o valor especificado pela máscara fornecida.
 * Consulte 9.3.3.6 / tabela 98 em http://www.nxp.com/documents/data_sheet/MFRC522.pdf
 * OBSERVAÇÃO: A máscara fornecida é limpa com (0x07<<4)=01110000b, pois RCFfgReg pode usar bits reservados.
 */

void MFRC522::PCD_SetAntennaGain(byte mascara)
{
	if (PCD_GetAntennaGain() != mascara)
	{															 // só nos preocupamos se houver uma alteração
		PCD_ClearRegisterBitMask(RFCfgReg, (0x07 << 4));		 // limpa para permitir o padrão 000
		PCD_SetRegisterBitMask(RFCfgReg, mascara & (0x07 << 4)); // define apenas os bits RxGain[2:0]
	}
} // Fim de PCD_SetAntennaGain()

/**
 * Realiza um autoteste do MFRC522
 * Consulte a seção 16.1.1 em http://www.nxp.com/documents/data_sheet/MFRC522.pdf
 *
 * @return Se o teste passou ou não. Ou falso se nenhuma referência de firmware estiver disponível.
 */

bool MFRC522::PCD_PerformSelfTest()
{
	// Isso segue diretamente os passos descritos na seção 16.1.1
	// 1. Realize uma reinicialização suave.
	PCD_Reset();

	// 2. Limpe o buffer interno escrevendo 25 bytes de 00h
	byte ZEROES[25] = {0x00};
	PCD_WriteRegister(FIFOLevelReg, 0x80);		// limpe o buffer FIFO
	PCD_WriteRegister(FIFODataReg, 25, ZEROES); // escreva 25 bytes de 00h no FIFO
	PCD_WriteRegister(CommandReg, PCD_Mem);		// transfira para o buffer interno

	// 3. Ative o autoteste
	PCD_WriteRegister(AutoTestReg, 0x09);

	// 4. Escreva 00h no buffer FIFO
	PCD_WriteRegister(FIFODataReg, 0x00);

	// 5. Inicie o autoteste emitindo o comando CalcCRC
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);

	// 6. Aguarde o autoteste ser concluído
	byte n;
	for (uint8_t i = 0; i < 0xFF; i++)
	{
		// O datasheet não especifica uma condição exata de conclusão, exceto
		// que o buffer FIFO deve conter 64 bytes.
		// Embora o autoteste seja iniciado pelo comando CalcCRC
		// ele se comporta de forma diferente da computação de CRC normal,
		// então não é possível usar com confiabilidade o DivIrqReg para verificar a conclusão.
		// Relata-se que alguns dispositivos não acionam a bandeira CRCIRq
		// durante o autoteste.
		n = PCD_ReadRegister(FIFOLevelReg);
		if (n >= 64)
		{
			break;
		}
	}
	PCD_WriteRegister(CommandReg, PCD_Idle); // Pare de calcular o CRC para novo conteúdo no FIFO.

	// 7. Leia os 64 bytes resultantes do buffer FIFO.
	byte result[64];
	PCD_ReadRegister(FIFODataReg, 64, result, 0);

	// Autoteste automático concluído
	// Redefina o registro AutoTestReg para 0 novamente. Necessário para operação normal.
	PCD_WriteRegister(AutoTestReg, 0x00);

	// Determine a versão do firmware (consulte a seção 9.3.4.8 no manual)
	byte version = PCD_ReadRegister(VersionReg);

	// Escolha os valores de referência apropriados
	const byte *reference;
	switch (version)
	{
	case 0x88: // Clone Fudan Semiconductor FM17522
		reference = FM17522_firmware_reference;
		break;
	case 0x90: // Versão 0.0
		reference = MFRC522_firmware_referenceV0_0;
		break;
	case 0x91: // Versão 1.0
		reference = MFRC522_firmware_referenceV1_0;
		break;
	case 0x92: // Versão 2.0
		reference = MFRC522_firmware_referenceV2_0;
		break;
	default:		  // Versão desconhecida
		return false; // abortar teste
	}

	// Verifique se os resultados correspondem às nossas expectativas
	for (uint8_t i = 0; i < 64; i++)
	{
		if (result[i] != pgm_read_byte(&(reference[i])))
		{
			return false;
		}
	}

	// 8. Realize uma reinicialização, pois o PCD não funciona após o teste.
	// A redefinição não funciona como o esperado.
	// "Autoteste automático concluído" não funciona como o esperado.
	PCD_Init();

	// Teste passou; tudo está bom.
	return true;
} // Fim PCD_PerformSelfTest()

/////////////////////////////////////////////////////////////////////////////////////
// Controle de Energia
/////////////////////////////////////////////////////////////////////////////////////

// NOTA IMPORTANTE!!!!
// Chamar qualquer outra função que usa CommandReg desabilitará o modo de desligamento suave de energia !!!
// Para mais detalhes sobre o controle de energia, consulte o datasheet - página 33 (8.6)

void MFRC522::PCD_SoftPowerDown()
{
	// Nota: Somente o modo de desligamento suave de energia está disponível via software
	byte val = PCD_ReadRegister(CommandReg); // Lê o estado do registro de comando
	val |= (1 << 4);						 // Define o bit PowerDown (bit 4) como 1
	PCD_WriteRegister(CommandReg, val);		 // Escreve o novo valor no registro de comando
}

void MFRC522::PCD_SoftPowerUp()
{
	byte val = PCD_ReadRegister(CommandReg); // Lê o estado do registro de comando
	val &= ~(1 << 4);						 // Define o bit PowerDown (bit 4) como 0
	PCD_WriteRegister(CommandReg, val);		 // Escreve o novo valor no registro de comando

	// Aguarde até que o bit PowerDown seja apagado (isso indica o fim do procedimento de despertar)
	const uint32_t timeout = (uint32_t)millis() + 500; // Cria um temporizador para tempo limite (apenas por precaução)

	while (millis() <= timeout)
	{										// Defina o tempo limite para 500 ms
		val = PCD_ReadRegister(CommandReg); // Lê o estado do registro de comando
		if (!(val & (1 << 4)))
		{		   // Se o bit de desligamento de energia for 0
			break; // O procedimento de despertar está concluído
		}
		yield();
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// Funções para comunicação com PICCs
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Executa o comando Transceive.
 * A validação do CRC só pode ser feita se backData e backLen forem especificados.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PCD_TransceiveData(byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC)
{
	byte waitIRq = 0x30; // RxIRq e IdleIRq
	return PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
} // Fim de PCD_TransceiveData()

/**
 * Transfere dados para o FIFO do MFRC522, executa um comando, aguarda a conclusão e transfere dados de volta do FIFO.
 * A validação do CRC só pode ser feita se backData e backLen forem especificados.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PCD_CommunicateWithPICC(byte command, byte waitIRq, byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC)
{
	// Prepare os valores para BitFramingReg
	byte txLastBits = validBits ? *validBits : 0;
	byte bitFraming = (rxAlign << 4) + txLastBits; // RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]

	PCD_WriteRegister(CommandReg, PCD_Idle);		   // Pare qualquer comando ativo.
	PCD_WriteRegister(ComIrqReg, 0x7F);				   // Limpe todos os sete bits de solicitação de interrupção
	PCD_WriteRegister(FIFOLevelReg, 0x80);			   // FlushBuffer = 1, inicialização do FIFO
	PCD_WriteRegister(FIFODataReg, sendLen, sendData); // Escreva sendData no FIFO
	PCD_WriteRegister(BitFramingReg, bitFraming);	   // Ajustes de bits
	PCD_WriteRegister(CommandReg, command);			   // Execute o comando
	if (command == PCD_Transceive)
	{
		PCD_SetRegisterBitMask(BitFramingReg, 0x80); // StartSend=1, início da transmissão de dados
	}

	// Em PCD_Init(), definimos a bandeira TAuto em TModeReg. Isso significa que o temporizador
	// inicia automaticamente quando o PCD para de transmitir.
	//
	// Aguarde aqui até que o comando seja concluído. Os bits especificados no
	// parâmetro 'waitIRq' definem quais bits constituem um comando concluído.
	// Quando eles estão definidos no registro ComIrqReg, então o comando é
	// considerado completo. Se o comando não for indicado como completo em
	// ~36ms, considere o comando como expirado.
	const uint32_t deadline = millis() + 36;
	bool completed = false;

	do
	{
		byte n = PCD_ReadRegister(ComIrqReg); // Os bits ComIrqReg[7..0] são: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (n & waitIRq)
		{ // Um dos bits de interrupção que sinaliza o sucesso foi definido.
			completed = true;
			break;
		}
		if (n & 0x01)
		{ // Interrupção do temporizador - nada recebido em 25ms
			return STATUS_TIMEOUT;
		}
		yield();
	} while (static_cast<uint32_t>(millis()) < deadline);

	// 36ms e nada aconteceu. A comunicação com o MFRC522 pode estar inativa.
	if (!completed)
	{
		return STATUS_TIMEOUT;
	}

	// Pare agora se algum erro, exceto colisões, foi detectado.
	byte errorRegValue = PCD_ReadRegister(ErrorReg); // Os bits ErrorReg[7..0] são: WrErr TempErr reservado BufferOvfl CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13)
	{ // BufferOvfl ParityErr ProtocolErr
		return STATUS_ERROR;
	}

	byte _validBits = 0;

	// Se o chamador desejar os dados de volta, obtenha-os do MFRC522.
	if (backData && backLen)
	{
		byte n = PCD_ReadRegister(FIFOLevelReg); // Número de bytes no FIFO
		if (n > *backLen)
		{
			return STATUS_NO_ROOM;
		}
		*backLen = n;										 // Número de bytes retornados
		PCD_ReadRegister(FIFODataReg, n, backData, rxAlign); // Obtenha os dados recebidos do FIFO
		_validBits = PCD_ReadRegister(ControlReg) & 0x07;	 // RxLastBits[2:0] indica o número de bits válidos no último byte recebido. Se este valor for 000b, o byte inteiro é válido.
		if (validBits)
		{
			*validBits = _validBits;
		}
	}

	// Informe sobre colisões
	if (errorRegValue & 0x08)
	{ // CollErr
		return STATUS_COLLISION;
	}

	// Realize a validação CRC_A, se solicitado.
	if (backData && backLen && checkCRC)
	{
		// Neste caso, um NAK MIFARE Classic não é OK.
		if (*backLen == 1 && _validBits == 4)
		{
			return STATUS_MIFARE_NACK;
		}
		// Precisamos de pelo menos o valor CRC_A e todos os 8 bits do último byte devem ser recebidos.
		if (*backLen < 2 || _validBits != 0)
		{
			return STATUS_CRC_WRONG;
		}
		// Verifique o CRC_A - faça nosso próprio cálculo e armazene o controle em controlBuffer.
		byte controlBuffer[2];
		MFRC522::StatusCode status = PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
		if (status != STATUS_OK)
		{
			return status;
		}
		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1]))
		{
			return STATUS_CRC_WRONG;
		}
	}

	return STATUS_OK;
} // Fim de PCD_CommunicateWithPICC()

/**
 * Transmite um comando REQuest, Tipo A. Convida os PICCs no estado IDLE a irem para o estado READY e se prepararem para anticollision ou seleção. Quadro de 7 bits.
 * Atenção: Quando dois PICCs estão no campo ao mesmo tempo, muitas vezes obtenho STATUS_TIMEOUT - provavelmente devido a um projeto de antena ruim.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PICC_RequestA(byte *bufferATQA, byte *bufferSize)
{
	return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
} // Fim de PICC_RequestA()

/**
 * Transmite um comando Wake-UP, Tipo A. Convida os PICCs nos estados IDLE e HALT a irem para o estado READY(*) e se prepararem para anticollision ou seleção. Quadro de 7 bits.
 * Atenção: Quando dois PICCs estão no campo ao mesmo tempo, muitas vezes obtenho STATUS_TIMEOUT - provavelmente devido a um projeto de antena ruim.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PICC_WakeupA(byte *bufferATQA, byte *bufferSize)
{
	return PICC_REQA_or_WUPA(PICC_CMD_WUPA, bufferATQA, bufferSize);
} // Fim de PICC_WakeupA()

/**
 * Transmite os comandos REQA ou WUPA.
 * Atenção: Quando dois PICCs estão no campo ao mesmo tempo, muitas vezes obtenho STATUS_TIMEOUT - provavelmente devido a um projeto de antena ruim.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
/**
 * Transmite comandos SELECT/ANTICOLLISION para selecionar um único PICC.
 * Antes de chamar esta função, os PICCs devem estar no estado READY(*) chamando PICC_RequestA() ou PICC_WakeupA().
 * Em caso de sucesso:
 *      - O PICC escolhido está no estado ACTIVE(*) e todos os outros PICCs retornaram ao estado IDLE/HALT. (Figura 7 do rascunho ISO/IEC 14443-3.)
 *      - O tamanho e o valor do UID do PICC escolhido são retornados em *uid, juntamente com o SAK.
 *
 * Um UID do PICC consiste em 4, 7 ou 10 bytes.
 * Apenas 4 bytes podem ser especificados em um comando SELECT, portanto, para os UID mais longos, são usadas duas ou três iterações:
 *      Tamanho do UID  Número de bytes do UID  Níveis de cascata  Exemplo de PICC
 *      ==========      ===================      ==============   =================
 *      único                   4                       1             MIFARE Classic
 *      duplo                   7                       2             MIFARE Ultralight
 *      triplo                 10                       3             Não atualmente em uso?
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
/**
 * Seleciona um cartão PICC transmitindo comandos SELECT/ANTICOLLISION.
 * Antes de chamar esta função, os PICCs devem estar no estado READY(*) chamando PICC_RequestA() ou PICC_WakeupA().
 * Em caso de sucesso:
 *      - O PICC escolhido estará no estado ACTIVE(*) e todos os outros PICCs retornarão ao estado IDLE/HALT. (Figura 7 do rascunho ISO/IEC 14443-3.)
 *      - O tamanho e o valor do UID do PICC escolhido são retornados em *uid, juntamente com o SAK.
 *
 * Um UID do PICC consiste em 4, 7 ou 10 bytes.
 * Apenas 4 bytes podem ser especificados em um comando SELECT, portanto, para os UID mais longos, são usadas duas ou três iterações:
 *      Tamanho do UID  Número de bytes do UID  Níveis de cascata  Exemplo de PICC
 *      ==========      ===================      ==============   =================
 *      único                   4                       1             MIFARE Classic
 *      duplo                   7                       2             MIFARE Ultralight
 *      triplo                 10                       3             Não atualmente em uso?
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PICC_Select(Uid *uid, byte validBits)
{
	bool uidCompleto;
	bool selecaoConcluida;
	bool usarEtiquetaDeCascata;
	byte nivelDeCascata = 1;
	MFRC522::StatusCode resultado;
	byte contagem;
	byte bitDeVerificacao;
	byte indice;
	byte indiceUID;
	int8_t bitsConhecidosNivelAtual;
	byte buffer[9];
	byte tamanhoBufferUsado;
	byte alinhamentoRX;
	byte ultimosBitsTX;
	byte *bufferResposta;
	byte comprimentoResposta;

	// Verificações de integridade
	if (validBits > 80)
	{
		return STATUS_INVALID;
	}

	// Prepara o MFRC522
	PCD_ClearRegisterBitMask(CollReg, 0x80);

	// Repete o loop do Nível de Cascata até que tenhamos um UID completo.
	uidCompleto = false;
	while (!uidCompleto)
	{
		switch (nivelDeCascata)
		{
		case 1:
			buffer[0] = PICC_CMD_SEL_CL1;
			indiceUID = 0;
			usarEtiquetaDeCascata = validBits && uid->size > 4;
			break;

		case 2:
			buffer[0] = PICC_CMD_SEL_CL2;
			indiceUID = 3;
			usarEtiquetaDeCascata = validBits && uid->size > 7;
			break;

		case 3:
			buffer[0] = PICC_CMD_SEL_CL3;
			indiceUID = 6;
			usarEtiquetaDeCascata = false;
			break;

		default:
			return STATUS_INTERNAL_ERROR;
			break;
		}

		bitsConhecidosNivelAtual = validBits - (8 * indiceUID);
		if (bitsConhecidosNivelAtual < 0)
		{
			bitsConhecidosNivelAtual = 0;
		}

		indice = 2;
		if (usarEtiquetaDeCascata)
		{
			buffer[indice++] = PICC_CMD_CT;
		}
		byte bytesParaCopiar = bitsConhecidosNivelAtual / 8 + (bitsConhecidosNivelAtual % 8 ? 1 : 0);
		if (bytesParaCopiar)
		{
			byte maximoBytes = usarEtiquetaDeCascata ? 3 : 4;
			if (bytesParaCopiar > maximoBytes)
			{
				bytesParaCopiar = maximoBytes;
			}
			for (contagem = 0; contagem < bytesParaCopiar; contagem++)
			{
				buffer[indice++] = uid->uidByte[indiceUID + contagem];
			}
		}
		if (usarEtiquetaDeCascata)
		{
			bitsConhecidosNivelAtual += 8;
		}

		selecaoConcluida = false;
		while (!selecaoConcluida)
		{
			if (bitsConhecidosNivelAtual >= 32)
			{
				buffer[1] = 0x70;
				buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
				resultado = PCD_CalculateCRC(buffer, 7, &buffer[7]);
				if (resultado != STATUS_OK)
				{
					return resultado;
				}
				ultimosBitsTX = 0;
				tamanhoBufferUsado = 9;
				bufferResposta = &buffer[6];
				comprimentoResposta = 3;
			}
			else
			{
				ultimosBitsTX = bitsConhecidosNivelAtual % 8;
				contagem = bitsConhecidosNivelAtual / 8;
				indice = 2 + contagem;
				buffer[1] = (indice << 4) + ultimosBitsTX;
				tamanhoBufferUsado = indice + (ultimosBitsTX ? 1 : 0);
				bufferResposta = &buffer[indice];
				comprimentoResposta = sizeof(buffer) - indice;
			}

			alinhamentoRX = ultimosBitsTX;
			PCD_WriteRegister(BitFramingReg, (alinhamentoRX << 4) + ultimosBitsTX);

			resultado = PCD_TransceiveData(buffer, tamanhoBufferUsado, bufferResposta, &comprimentoResposta, &ultimosBitsTX, alinhamentoRX);
			if (resultado == STATUS_COLLISION)
			{
				byte valorDeCollReg = PCD_ReadRegister(CollReg);
				if (valorDeCollReg & 0x20)
				{
					return STATUS_COLLISION;
				}
				byte posicaoColisao = valorDeCollReg & 0x1F;
				if (posicaoColisao == 0)
				{
					posicaoColisao = 32;
				}
				if (posicaoColisao <= bitsConhecidosNivelAtual)
				{
					return STATUS_INTERNAL_ERROR;
				}
				bitsConhecidosNivelAtual = posicaoColisao;
				contagem = bitsConhecidosNivelAtual % 8;
				bitDeVerificacao = (bitsConhecidosNivelAtual - 1) % 8;
				indice = 1 + (bitsConhecidosNivelAtual / 8) + (contagem ? 1 : 0);
				buffer[indice] |= (1 << bitDeVerificacao);
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

		if (comprimentoResposta != 3 || ultimosBitsTX != 0)
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
			nivelDeCascata++;
		}
		else
		{
			uidCompleto = true;
			uid->sak = bufferResposta[0];
		}
	}

	uid->size = 3 * nivelDeCascata + 1;

	return STATUS_OK;
} // Fim de PICC_Select()

/**
 * Instrui um PICC no estado ACTIVE(*) a entrar no estado HALT.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PICC_HaltA()
{
	MFRC522::StatusCode resultado;
	byte buffer[4];

	// Monta o buffer de comando
	buffer[0] = PICC_CMD_HLTA;
	buffer[1] = 0;
	// Calcula o CRC_A
	resultado = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Envia o comando.
	// O padrão diz:
	//		Se o PICC responder com qualquer modulação durante um período de 1 ms após o final do quadro contendo o
	//		comando HLTA, essa resposta será interpretada como 'não reconhecida'.
	// Interpretamos da seguinte forma: Apenas STATUS_TIMEOUT é um sucesso.
	resultado = PCD_TransceiveData(buffer, sizeof(buffer), nullptr, 0);
	if (resultado == STATUS_TIMEOUT)
	{
		return STATUS_OK;
	}
	if (resultado == STATUS_OK)
	{ // Ironicamente, isso NÃO está ok neste caso ;-)
		return STATUS_ERROR;
	}
	return resultado;
} // Fim de PICC_HaltA()

/////////////////////////////////////////////////////////////////////////////////////
// Funções para comunicar com PICCs MIFARE
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Executa o comando MFAuthent do MFRC522.
 * Este comando gerencia a autenticação MIFARE para permitir uma comunicação segura com qualquer cartão MIFARE Mini, MIFARE 1K e MIFARE 4K.
 * A autenticação é descrita na seção 10.3.1.9 do datasheet do MFRC522 e na seção 10.1 do documento http://www.nxp.com/documents/data_sheet/MF1S503x.pdf.
 * Para uso com PICCs MIFARE Classic.
 * O PICC deve estar selecionado - ou seja, no estado ACTIVE(*) - antes de chamar esta função.
 * Lembre-se de chamar PCD_StopCrypto1() após se comunicar com o PICC autenticado - caso contrário, nenhuma nova comunicação pode ser iniciada.
 *
 * Todas as chaves são definidas como FFFFFFFFFFFFh na entrega do chip.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário. Provavelmente STATUS_TIMEOUT se você fornecer a chave errada.
 */
MFRC522::StatusCode MFRC522::PCD_Authenticate(byte comando,		 ///< PICC_CMD_MF_AUTH_KEY_A ou PICC_CMD_MF_AUTH_KEY_B
											  byte blocoAddr,	 ///< O número do bloco. Veja a numeração nos comentários no arquivo .h.
											  MIFARE_Key *chave, ///< Ponteiro para a chave Crypteo1 a ser usada (6 bytes)
											  Uid *uid			 ///< Ponteiro para a estrutura Uid. Os primeiros 4 bytes do UID são usados.
)
{
	byte waitIRq = 0x10; // IdleIRq

	// Constrói o buffer de comando
	byte sendData[12];
	sendData[0] = comando;
	sendData[1] = blocoAddr;
	for (byte i = 0; i < MF_KEY_SIZE; i++)
	{ // 6 bytes de chave
		sendData[2 + i] = chave->keyByte[i];
	}
	// Use os últimos bytes do UID conforme especificado em http://cache.nxp.com/documents/application_note/AN10927.pdf
	// Seção 3.2.5 "Autenticação MIFARE Classic".
	// O único caso não abordado é a ativação do atalho MF1Sxxxx,
	// mas isso requer um byte de tag de cascata (CT), que não faz parte do UID.
	for (byte i = 0; i < 4; i++)
	{ // Os últimos 4 bytes do UID
		sendData[8 + i] = uid->uidByte[i + uid->size - 4];
	}

	// Inicia a autenticação.
	return PCD_CommunicateWithPICC(PCD_MFAuthent, waitIRq, &sendData[0], sizeof(sendData));
} // Fim PCD_Authenticate()

/**
 * Usado para sair do estado autenticado do PCD.
 * Lembre-se de chamar esta função após se comunicar com um PICC autenticado - caso contrário, nenhuma nova comunicação pode ser iniciada.
 */
void MFRC522::PCD_StopCrypto1()
{
	// Limpa o bit MFCrypto1On
	PCD_ClearRegisterBitMask(Status2Reg, 0x08); // Os bits de Status2Reg[7..0] são: TempSensClear I2CForceHS reservado reservado MFCrypto1On ModemState[2:0]
} // Fim PCD_StopCrypto1()

/**
 * Lê 16 bytes (+ 2 bytes CRC_A) do PICC ativo.
 *
 * Para MIFARE Classic, o setor que contém o bloco deve estar autenticado antes de chamar esta função.
 *
 * Para MIFARE Ultralight, apenas os endereços de 00h a 0Fh são decodificados.
 * O MF0ICU1 retorna um NAK para endereços mais altos.
 * O MF0ICU1 responde ao comando READ enviando 16 bytes a partir do endereço da página definido pelo argumento do comando.
 * Por exemplo, se blocoAddr for 03h, então as páginas 03h, 04h, 05h, 06h são retornadas.
 * Um rollback é implementado: se blocoAddr for 0Eh, então o conteúdo das páginas 0Eh, 0Fh, 00h e 01h é retornado.
 *
 * O buffer deve ter pelo menos 18 bytes, pois um CRC_A também é retornado.
 * Verifica o CRC_A antes de retornar STATUS_OK.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */

/**
 * Lê 16 bytes do PICC ativo.
 *
 * Para MIFARE Classic, o setor que contém o bloco deve estar autenticado antes de chamar esta função.
 *
 * Para MIFARE Ultralight, a operação é chamada de "LEITURA DE COMPATIBILIDADE".
 * Embora 16 bytes sejam transferidos para o PICC Ultralight, apenas os 4 bytes menos significativos (bytes 0 a 3)
 * são lidos do endereço especificado. É recomendável definir os bytes restantes de 04h a 0Fh como lógica 0.
 * *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_Read(byte blocoAddr,  ///< MIFARE Classic: O número do bloco (0-0xff). MIFARE Ultralight: A primeira página para retornar dados.
										 byte *buffer,	  ///< O buffer para armazenar os dados
										 byte *bufferSize ///< Tamanho do buffer, pelo menos 18 bytes. Também é o número de bytes retornados se STATUS_OK.
)
{
	MFRC522::StatusCode resultado;

	// Verificação de sanidade
	if (buffer == nullptr || *bufferSize < 18)
	{
		return STATUS_NO_ROOM;
	}

	// Constrói o buffer de comando
	buffer[0] = PICC_CMD_MF_READ;
	buffer[1] = blocoAddr;
	// Calcula CRC_A
	resultado = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Transmite o buffer e recebe a resposta, validando o CRC_A.
	return PCD_TransceiveData(buffer, 4, buffer, bufferSize, nullptr, 0, true);
} // Fim MIFARE_Read()

/**
 * Escreve 16 bytes no PICC ativo.
 *
 * Para MIFARE Classic, o setor que contém o bloco deve estar autenticado antes de chamar esta função.
 *
 * Para MIFARE Ultralight, a operação é chamada de "ESCRITA DE COMPATIBILIDADE".
 * Embora 16 bytes sejam transferidos para o PICC Ultralight, apenas os 4 bytes menos significativos (bytes 0 a 3)
 * são escritos no endereço especificado. É recomendável definir os bytes restantes de 04h a 0Fh como lógica 0.
 * *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_Write(byte blocoAddr, ///< MIFARE Classic: O número do bloco (0-0xff). MIFARE Ultralight: A página (2-15) para escrever.
										  byte *buffer,	  ///< Os 16 bytes a serem escritos no PICC
										  byte bufferSize ///< Tamanho do buffer, deve ser pelo menos 16 bytes. Exatamente 16 bytes serão escritos.
)
{
	MFRC522::StatusCode resultado;

	// Verificação de sanidade
	if (buffer == nullptr || bufferSize < 16)
	{
		return STATUS_INVALID;
	}

	// O protocolo Mifare Classic requer duas comunicações para realizar uma escrita.
	// Passo 1: Diz ao PICC que queremos escrever no bloco blocoAddr.
	byte cmdBuffer[2];
	cmdBuffer[0] = PICC_CMD_MF_WRITE;
	cmdBuffer[1] = blocoAddr;
	resultado = PCD_MIFARE_Transceive(cmdBuffer, 2); // Adiciona CRC_A e verifica se a resposta é MF_ACK.
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Passo 2: Transfere os dados
	resultado = PCD_MIFARE_Transceive(buffer, bufferSize); // Adiciona CRC_A e verifica se a resposta é MF_ACK.
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	return STATUS_OK;
} // Fim MIFARE_Write()

/**
 * Escreve uma página de 4 bytes no PICC MIFARE Ultralight ativo.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */

MFRC522::StatusCode MFRC522::MIFARE_Ultralight_Write(	byte page, 		///< The page (2-15) to write to.
														byte *buffer,	///< The 4 bytes to write to the PICC
														byte bufferSize	///< Buffer size, must be at least 4 bytes. Exactly 4 bytes are written.
													) {
	MFRC522::StatusCode result;
	
	// Sanity check
	if (buffer == nullptr || bufferSize < 4) {
		return STATUS_INVALID;
	}
	
	// Build commmand buffer
	byte cmdBuffer[6];
	cmdBuffer[0] = PICC_CMD_UL_WRITE;
	cmdBuffer[1] = page;
	memcpy(&cmdBuffer[2], buffer, 4);
	
	// Perform the write
	result = PCD_MIFARE_Transceive(cmdBuffer, 6); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK) {
		return result;
	}
	return STATUS_OK;
} // End MIFARE_Ultralight_Write()

/**
 * MIFARE Decremento subtrai o delta do valor do bloco endereçado e armazena o resultado em uma memória volátil.
 * Somente para MIFARE Classic. O setor que contém o bloco deve estar autenticado antes de chamar esta função.
 * Apenas para blocos no modo "bloco de valor", ou seja, com bits de acesso [C1 C2 C3] = [110] ou [001].
 * Use MIFARE_Transfer() para armazenar o resultado em um bloco.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_Decrement(byte blocoAddr, ///< O número do bloco (0-0xff).
											  int32_t delta	  ///< Este número é subtraído do valor do bloco blocoAddr.
)
{
	return MIFARE_TwoStepHelper(PICC_CMD_MF_DECREMENT, blocoAddr, delta);
} // Fim MIFARE_Decrement()

/**
 * MIFARE Incremento adiciona o delta ao valor do bloco endereçado e armazena o resultado em uma memória volátil.
 * Somente para MIFARE Classic. O setor que contém o bloco deve estar autenticado antes de chamar esta função.
 * Apenas para blocos no modo "bloco de valor", ou seja, com bits de acesso [C1 C2 C3] = [110] ou [001].
 * Use MIFARE_Transfer() para armazenar o resultado em um bloco.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_Increment(byte blocoAddr, ///< O número do bloco (0-0xff).
											  int32_t delta	  ///< Este número é adicionado ao valor do bloco blocoAddr.
)
{
	return MIFARE_TwoStepHelper(PICC_CMD_MF_INCREMENT, blocoAddr, delta);
} // Fim MIFARE_Increment()

/**
 * MIFARE Restore copia o valor do bloco endereçado para uma memória volátil.
 * Somente para MIFARE Classic. O setor que contém o bloco deve estar autenticado antes de chamar esta função.
 * Apenas para blocos no modo "bloco de valor", ou seja, com bits de acesso [C1 C2 C3] = [110] ou [001].
 * Use MIFARE_Transfer() para armazenar o resultado em um bloco.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_Restore(byte blocoAddr ///< O número do bloco (0-0xff).
)
{
	// O datasheet descreve o Restore como uma operação de duas etapas, mas não explica que dados transferir na etapa 2.
	// Fazer apenas uma etapa única não funciona, então escolhi transferir 0L na etapa dois.
	return MIFARE_TwoStepHelper(PICC_CMD_MF_RESTORE, blocoAddr, 0L);
} // Fim MIFARE_Restore()

/**
 * Função auxiliar para as operações de protocolo MIFARE Classic de duas etapas: Decremento, Incremento e Restore.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_TwoStepHelper(byte comando,	  ///< O comando a ser usado
												  byte blocoAddr, ///< O número do bloco (0-0xff).
												  int32_t dados	  ///< Os dados a serem transferidos na etapa 2
)
{
	MFRC522::StatusCode resultado;
	byte bufferComando[2]; // Só precisamos de espaço para 2 bytes.

	// Etapa 1: Diga ao PICC o comando e o endereço do bloco
	bufferComando[0] = comando;
	bufferComando[1] = blocoAddr;
	resultado = PCD_MIFARE_Transceive(bufferComando, 2); // Adiciona CRC_A e verifica se a resposta é MF_ACK.
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Etapa 2: Transferir os dados
	resultado = PCD_MIFARE_Transceive((byte *)&dados, 4, true); // Adiciona CRC_A e aceita timeout como sucesso.
	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	return STATUS_OK;
} // Fim MIFARE_TwoStepHelper()

/**
 * MIFARE Transfer escreve o valor armazenado na memória volátil em um bloco MIFARE Classic.
 * Somente para MIFARE Classic. O setor que contém o bloco deve estar autenticado antes de chamar esta função.
 * Apenas para blocos no modo "bloco de valor", ou seja, com bits de acesso [C1 C2 C3] = [110] ou [001].
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_Transfer(byte blocoAddr ///< O número do bloco (0-0xff).
)
{
	MFRC522::StatusCode resultado;
	byte bufferComando[2]; // Só precisamos de espaço para 2 bytes.

	// Diga ao PICC que queremos transferir o resultado para o bloco blocoAddr.
	bufferComando[0] = PICC_CMD_MF_TRANSFER;
	bufferComando[1] = blocoAddr;
	resultado = PCD_MIFARE_Transceive(bufferComando, 2); // Adiciona CRC_A e verifica se a resposta é MF_ACK.
	if (resultado != STATUS_OK)
	{
		return resultado;
	}
	return STATUS_OK;
} // Fim MIFARE_Transfer()

/**
 * Rotina auxiliar para ler o valor atual de um Bloco de Valor.
 *
 * Somente para MIFARE Classic e apenas para blocos no modo "bloco de valor", ou seja,
 * com bits de acesso [C1 C2 C3] = [110] ou [001]. O setor que contém o bloco deve
 * estar autenticado antes de chamar esta função.
 *
 * @param[in]   blocoAddr   O número do bloco (0x00-0xff).
 * @param[out]  valor       Valor atual do Bloco de Valor.
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_GetValue(byte blocoAddr, int32_t *valor)
{
	MFRC522::StatusCode status;
	byte buffer[18];
	byte tamanho = sizeof(buffer);

	// Leia o bloco
	status = MIFARE_Read(blocoAddr, buffer, &tamanho);
	if (status == STATUS_OK)
	{
		// Extraia o valor
		*valor = (int32_t(buffer[3]) << 24) | (int32_t(buffer[2]) << 16) | (int32_t(buffer[1]) << 8) | int32_t(buffer[0]);
	}
	return status;
} // Fim MIFARE_GetValue()

/**
 * Rotina auxiliar para escrever um valor específico em um Bloco de Valor.
 *
 * Somente para MIFARE Classic e apenas para blocos no modo "bloco de valor", ou seja,
 * com bits de acesso [C1 C2 C3] = [110] ou [001]. O setor que contém o bloco deve
 * estar autenticado antes de chamar esta função.
 *
 * @param[in]   blocoAddr   O número do bloco (0x00-0xff).
 * @param[in]   valor       Novo valor do Bloco de Valor.
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */

/**
 * MIFARE_SetValue define o valor do Bloco de Valor.
 * Somente para MIFARE Classic. O setor que contém o bloco deve estar autenticado antes de chamar esta função.
 * Apenas para blocos no modo "bloco de valor", ou seja, com bits de acesso [C1 C2 C3] = [110] ou [001].
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::MIFARE_SetValue(byte blocoAddr, int32_t valor)
{
	byte buffer[18];

	// Traduz o int32_t em 4 bytes; repetidos 2x no bloco de valor
	buffer[0] = buffer[8] = (valor & 0xFF);
	buffer[1] = buffer[9] = (valor & 0xFF00) >> 8;
	buffer[2] = buffer[10] = (valor & 0xFF0000) >> 16;
	buffer[3] = buffer[11] = (valor & 0xFF000000) >> 24;
	// Inverte os 4 bytes também encontrados no bloco de valor
	buffer[4] = ~buffer[0];
	buffer[5] = ~buffer[1];
	buffer[6] = ~buffer[2];
	buffer[7] = ~buffer[3];
	// Endereço 2x com endereço inverso 2x
	buffer[12] = buffer[14] = blocoAddr;
	buffer[13] = buffer[15] = ~blocoAddr;

	// Escreve o bloco de dados inteiro
	return MIFARE_Write(blocoAddr, buffer, 16);
} // Fim MIFARE_SetValue()

/**
 * Autenticação com um NTAG216.
 *
 * Apenas para NTAG216. Primeiramente implementado por Gargantuanman.
 *
 * @param[in]   senha   senha.
 * @param[in]   pACK    resultado de sucesso???.
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PCD_NTAG216_AUTH(byte *senha, byte pACK[]) // Autenticação com senha de 32 bits
{
	// TODO: Corrigir o comprimento de cmdBuffer e rxlength. Eles realmente deveriam ser iguais.
	//       (Melhor ainda, rxlength nem deveria ser necessário.)

	MFRC522::StatusCode resultado;
	byte cmdBuffer[18]; // Precisamos de espaço para 16 bytes de dados e 2 bytes de CRC_A.

	cmdBuffer[0] = 0x1B; // Comando de autenticação

	for (byte i = 0; i < 4; i++)
		cmdBuffer[i + 1] = senha[i];

	resultado = PCD_CalculateCRC(cmdBuffer, 5, &cmdBuffer[5]);

	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	// Transfira os dados, armazene a resposta em cmdBuffer[]
	byte waitIRq = 0x30; // RxIRq e IdleIRq
	byte validBits = 0;
	byte rxlength = 5;
	resultado = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, cmdBuffer, 7, cmdBuffer, &rxlength, &validBits);

	pACK[0] = cmdBuffer[0];
	pACK[1] = cmdBuffer[1];

	if (resultado != STATUS_OK)
	{
		return resultado;
	}

	return STATUS_OK;
} // Fim PCD_NTAG216_AUTH()

/////////////////////////////////////////////////////////////////////////////////////
// Funções de suporte
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper para a comunicação do protocolo MIFARE.
 * Adiciona CRC_A, executa o comando Transceive e verifica se a resposta é MF_ACK ou um timeout.
 *
 * @return STATUS_OK em caso de sucesso, STATUS_??? caso contrário.
 */
MFRC522::StatusCode MFRC522::PCD_MIFARE_Transceive(byte *sendData,	  ///< Ponteiro para os dados a serem transferidos para o FIFO. NÃO incluir o CRC_A.
												   byte sendLen,	  ///< Número de bytes em sendData.
												   bool acceptTimeout ///< True => Um timeout também é considerado sucesso
)
{
	MFRC522::StatusCode resultado;
	byte cmdBuffer[18]; // Precisamos de espaço para 16 bytes de dados e 2 bytes de CRC_A.

	// Verificação de sanidade
	if (sendData == nullptr || sendLen > 16)
	{
		return STATUS_INVALID;
	}

	// Copie sendData[] para cmdBuffer[] e adicione o CRC_A
	memcpy(cmdBuffer, sendData, sendLen);
	resultado = PCD_CalculateCRC(cmdBuffer, sendLen, &cmdBuffer[sendLen]);
	if (resultado != STATUS_OK)
	{
		return resultado;
	}
	sendLen += 2;

	// Transfira os dados, armazene a resposta em cmdBuffer[]
	byte waitIRq = 0x30; // RxIRq e IdleIRq
	byte cmdBufferSize = sizeof(cmdBuffer);
	byte validBits = 0;
	resultado = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, cmdBuffer, sendLen, cmdBuffer, &cmdBufferSize, &validBits);
	if (acceptTimeout && resultado == STATUS_TIMEOUT)
	{
		return STATUS_OK;
	}
	if (resultado != STATUS_OK)
	{
		return resultado;
	}
	// O PICC deve responder com um ACK de 4 bits
	if (cmdBufferSize != 1 || validBits != 4)
	{
		return STATUS_ERROR;
	}
	if (cmdBuffer[0] != MF_ACK)
	{
		return STATUS_MIFARE_NACK;
	}
	return STATUS_OK;
} // Fim PCD_MIFARE_Transceive()

/**
 * Retorna um ponteiro __FlashStringHelper para o nome do código de status.
 *
 * @return const __FlashStringHelper *
 */
const __FlashStringHelper *MFRC522::GetStatusCodeName(MFRC522::StatusCode code ///< Um dos enums StatusCode.
)
{
	switch (code)
	{
	case STATUS_OK:
		return F("Sucesso.");
	case STATUS_ERROR:
		return F("Erro na comunicação.");
	case STATUS_COLLISION:
		return F("Colisão detectada.");
	case STATUS_TIMEOUT:
		return F("Timeout na comunicação.");
	case STATUS_NO_ROOM:
		return F("Um buffer não tem tamanho suficiente.");
	case STATUS_INTERNAL_ERROR:
		return F("Erro interno no código. Não deveria ocorrer.");
	case STATUS_INVALID:
		return F("Argumento inválido.");
	case STATUS_CRC_WRONG:
		return F("O CRC_A não corresponde.");
	case STATUS_MIFARE_NACK:
		return F("Um PICC MIFARE respondeu com NAK.");
	default:
		return F("Erro desconhecido");
	}
} // Fim GetStatusCodeName()

/**
 * Traduz o SAK (Select Acknowledge) para um tipo de PICC.
 *
 * @return PICC_Type
 */
MFRC522::PICC_Type MFRC522::PICC_GetType(byte sak ///< O byte SAK retornado de PICC_Select().
)
{
	// http://www.nxp.com/documents/application_note/AN10833.pdf
	// 3.2 Coding of Select Acknowledge (SAK)
	// ignora 8 bits (iso14443 começa com LSBit = bit 1)
	// corrige o tipo errado para o fabricante Infineon (http://nfc-tools.org/index.php?title=ISO14443A)
	sak &= 0x7F;
	switch (sak)
	{
	case 0x04:
		return PICC_TYPE_NOT_COMPLETE; // UID não está completo
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
		return PICC_TYPE_ISO_14443_4;
	case 0x40:
		return PICC_TYPE_ISO_18092;
	default:
		return PICC_TYPE_UNKNOWN;
	}
} // Fim PICC_GetType()

/**
 * Retorna um ponteiro __FlashStringHelper para o nome do tipo de PICC.
 *
 * @return const __FlashStringHelper *
 */
const __FlashStringHelper *MFRC522::PICC_GetTypeName(PICC_Type piccType ///< Um dos enums PICC_Type.
)
{
	switch (piccType)
	{
	case PICC_TYPE_ISO_14443_4:
		return F("PICC compatível com ISO/IEC 14443-4");
	case PICC_TYPE_ISO_18092:
		return F("PICC compatível com ISO/IEC 18092 (NFC)");
	case PICC_TYPE_MIFARE_MINI:
		return F("MIFARE Mini, 320 bytes");
	case PICC_TYPE_MIFARE_1K:
		return F("MIFARE 1KB");
	case PICC_TYPE_MIFARE_4K:
		return F("MIFARE 4KB");
	case PICC_TYPE_MIFARE_UL:
		return F("MIFARE Ultralight ou Ultralight C");
	case PICC_TYPE_MIFARE_PLUS:
		return F("MIFARE Plus");
	case PICC_TYPE_MIFARE_DESFIRE:
		return F("MIFARE DESFire");
	case PICC_TYPE_TNP3XXX:
		return F("MIFARE TNP3XXX");
	case PICC_TYPE_NOT_COMPLETE:
		return F("SAK indica que o UID não está completo.");
	case PICC_TYPE_UNKNOWN:
	default:
		return F("Tipo desconhecido");
	}
} // Fim PICC_GetTypeName()

/**
 * Exibe informações de depuração sobre o PCD conectado no Serial.
 * Mostra todas as versões de firmware conhecidas.
 */
void MFRC522::PCD_DumpVersionToSerial()
{
	// Obtenha a versão do firmware do MFRC522
	byte v = PCD_ReadRegister(VersionReg);
	Serial.print(F("Versão do Firmware: 0x"));
	Serial.print(v, HEX);
	// Verifique qual versão
	switch (v)
	{
	case 0x88:
		Serial.println(F(" = (clone)"));
		break;
	case 0x90:
		Serial.println(F(" = v0.0"));
		break;
	case 0x91:
		Serial.println(F(" = v1.0"));
		break;
	case 0x92:
		Serial.println(F(" = v2.0"));
		break;
	case 0x12:
		Serial.println(F(" = chip falsificado"));
		break;
	default:
		Serial.println(F(" = (desconhecido)"));
	}
	// Quando 0x00 ou 0xFF é retornado, a comunicação provavelmente falhou
	if ((v == 0x00) || (v == 0xFF))
		Serial.println(F("AVISO: Falha na comunicação, o MFRC522 está conectado corretamente?"));
} // Fim PCD_DumpVersionToSerial()

/**
 * Exibe informações de depuração sobre o PICC selecionado no Serial.
 * Em caso de sucesso, o PICC é interrompido após a exibição dos dados.
 * Para o MIFARE Classic, a chave padrão de fábrica 0xFFFFFFFFFFFF é tentada.
 */
void MFRC522::PICC_DumpToSerial(Uid *uid ///< Ponteiro para a estrutura Uid retornada de um PICC_Select() bem-sucedido.
)
{
	MIFARE_Key chave;

	// Exibe UID, SAK e Tipo
	PICC_DumpDetailsToSerial(uid);

	// Exibe conteúdo
	PICC_Type piccType = PICC_GetType(uid->sak);
	switch (piccType)
	{
	case PICC_TYPE_MIFARE_MINI:
	case PICC_TYPE_MIFARE_1K:
	case PICC_TYPE_MIFARE_4K:
		// Todas as chaves são definidas como FFFFFFFFFFFFh na entrega do chip de fábrica.
		for (byte i = 0; i < 6; i++)
		{
			chave.keyByte[i] = 0xFF;
		}
		PICC_DumpMifareClassicToSerial(uid, piccType, &chave);
		break;

	case PICC_TYPE_MIFARE_UL:
		PICC_DumpMifareUltralightToSerial();
		break;

	case PICC_TYPE_ISO_14443_4:
	case PICC_TYPE_MIFARE_DESFIRE:
	case PICC_TYPE_ISO_18092:
	case PICC_TYPE_MIFARE_PLUS:
	case PICC_TYPE_TNP3XXX:
		Serial.println(F("Exibição de conteúdo da memória não implementada para esse tipo de PICC."));
		break;

	case PICC_TYPE_UNKNOWN:
	case PICC_TYPE_NOT_COMPLETE:
	default:
		break; // Nenhum despejo de memória aqui
	}

	Serial.println();
	PICC_HaltA(); // Já foi feito se for um PICC MIFARE Classic.
} // Fim PICC_DumpToSerial()

/**
 * Exibe informações do cartão (UID, SAK, Tipo) sobre o PICC selecionado no Serial.
 */
void MFRC522::PICC_DumpDetailsToSerial(Uid *uid ///< Ponteiro para a estrutura Uid retornada de um PICC_Select() bem-sucedido.
)
{
	// UID
	Serial.print(F("UID do Cartão:"));
	for (byte i = 0; i < uid->size; i++)
	{
		if (uid->uidByte[i] < 0x10)
			Serial.print(F(" 0"));
		else
			Serial.print(F(" "));
		Serial.print(uid->uidByte[i], HEX);
	}
	Serial.println();

	// SAK
	Serial.print(F("SAK do Cartão: "));
	if (uid->sak < 0x10)
		Serial.print(F("0"));
	Serial.println(uid->sak, HEX);

	// Tipo PICC (sugerido)
	PICC_Type piccType = PICC_GetType(uid->sak);
	Serial.print(F("Tipo PICC: "));
	Serial.println(PICC_GetTypeName(piccType));
} // Fim PICC_DumpDetailsToSerial()

/**
 * Exibe o conteúdo da memória de um PICC MIFARE Classic.
 * Em caso de sucesso, o PICC é interrompido após a exibição dos dados.
 */
void MFRC522::PICC_DumpMifareClassicToSerial(Uid *uid,			 ///< Ponteiro para a estrutura Uid retornada de um PICC_Select() bem-sucedido.
											 PICC_Type piccType, ///< Um dos enums PICC_Type.
											 MIFARE_Key *key	 ///< Chave A usada para todos os setores.
)
{
	byte no_of_sectors = 0;
	switch (piccType)
	{
	case PICC_TYPE_MIFARE_MINI:
		// Possui 5 setores * 4 blocos/setor * 16 bytes/bloco = 320 bytes.
		no_of_sectors = 5;
		break;

	case PICC_TYPE_MIFARE_1K:
		// Possui 16 setores * 4 blocos/setor * 16 bytes/bloco = 1024 bytes.
		no_of_sectors = 16;
		break;

	case PICC_TYPE_MIFARE_4K:
		// Possui (32 setores * 4 blocos/setor + 8 setores * 16 blocos/setor) * 16 bytes/bloco = 4096 bytes.
		no_of_sectors = 40;
		break;

	default: // Não deveria acontecer. Ignorar.
		break;
	}

	// Exibe setores, começando pelo endereço mais alto.
	if (no_of_sectors)
	{
		Serial.println(F("Setor Bloco   0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15  Bits de Acesso"));
		for (int8_t i = no_of_sectors - 1; i >= 0; i--)
		{
			PICC_DumpMifareClassicSectorToSerial(uid, key, i);
		}
	}
	PICC_HaltA(); // Interrompe o PICC antes de encerrar a sessão criptografada.
	PCD_StopCrypto1();
} // Fim PICC_DumpMifareClassicToSerial()

/**
 * Exibe o conteúdo da memória de um setor de um PICC MIFARE Classic.
 * Usa PCD_Authenticate(), MIFARE_Read() e PCD_StopCrypto1.
 * Sempre usa PICC_CMD_MF_AUTH_KEY_A porque apenas a Chave A pode sempre ler os bits de acesso do setor.
 */

/**
 * Exibe o conteúdo da memória de um setor MIFARE Classic para o Serial.
 * Em caso de sucesso, o PICC é interrompido após a exibição dos dados.
 */
void MFRC522::PICC_DumpMifareClassicSectorToSerial(Uid *uid,		///< Ponteiro para a estrutura Uid retornada de um PICC_Select() bem-sucedido.
												   MIFARE_Key *key, ///< Chave A para o setor.
												   byte setor		///< O setor a ser exibido, 0..39.
)
{
	MFRC522::StatusCode status;
	byte primeiroBloco;	 // Endereço do bloco mais baixo a ser exibido (na verdade, o último bloco exibido).
	byte numeroDeBlocos; // Número de blocos no setor
	bool eSetorTrailer;	 // Define como true ao lidar com o "último" (ou seja, o endereço mais alto) no setor.

	// Os bits de acesso são armazenados de forma peculiar.
	// Existem quatro grupos:
	// g[3]	Bits de acesso para o bloco 3 do trailer do setor (para setores 0-31) ou bloco 15 (para setores 32-39)
	// g[2]	Bits de acesso para o bloco 2 (para setores 0-31) ou blocos 10-14 (para setores 32-39)
	// g[1]	Bits de acesso para o bloco 1 (para setores 0-31) ou blocos 5-9 (para setores 32-39)
	// g[0]	Bits de acesso para o bloco 0 (para setores 0-31) ou blocos 0-4 (para setores 32-39)
	// Cada grupo possui bits de acesso [C1 C2 C3]. Neste código, C1 é o bit mais significativo e C3 é o menos significativo.
	// Os quatro bits CX são armazenados juntos em um nibble cx e em um nibble cx_ invertido.
	byte c1, c2, c3;	  // Nibbles
	byte c1_, c2_, c3_;	  // Nibbles invertidos
	bool erroInvertido;	  // True se um dos nibbles invertidos não corresponder
	byte g[4];			  // Bits de acesso para cada um dos quatro grupos.
	byte grupo;			  // 0-3 - grupo ativo para bits de acesso
	bool primeiroNoGrupo; // True para o primeiro bloco exibido no grupo

	// Determine a posição e o tamanho do setor.
	if (setor < 32)
	{ // Setores 0..31 têm 4 blocos cada
		numeroDeBlocos = 4;
		primeiroBloco = setor * numeroDeBlocos;
	}
	else if (setor < 40)
	{ // Setores 32-39 têm 16 blocos cada
		numeroDeBlocos = 16;
		primeiroBloco = 128 + (setor - 32) * numeroDeBlocos;
	}
	else
	{ // Entrada ilegal, nenhum PICC MIFARE Classic tem mais de 40 setores.
		return;
	}

	// Exibe blocos, começando pelo endereço mais alto.
	byte contadorDeBytes;
	byte buffer[18];
	byte enderecoDoBloco;
	eSetorTrailer = true;
	erroInvertido = false; // Evita o aviso de "variável não usada".
	for (int8_t deslocamentoDoBloco = numeroDeBlocos - 1; deslocamentoDoBloco >= 0; deslocamentoDoBloco--)
	{
		enderecoDoBloco = primeiroBloco + deslocamentoDoBloco;
		// Número do setor - apenas na primeira linha
		if (eSetorTrailer)
		{
			if (setor < 10)
				Serial.print(F("   ")); // Preenche com espaços
			else
				Serial.print(F("  ")); // Preenche com espaços
			Serial.print(setor);
			Serial.print(F("   "));
		}
		else
		{
			Serial.print(F("       "));
		}
		// Número do bloco
		if (enderecoDoBloco < 10)
			Serial.print(F("   ")); // Preenche com espaços
		else
		{
			if (enderecoDoBloco < 100)
				Serial.print(F("  ")); // Preenche com espaços
			else
				Serial.print(F(" ")); // Preenche com espaços
		}
		Serial.print(enderecoDoBloco);
		Serial.print(F("  "));
		// Estabelece comunicação criptografada antes de ler o primeiro bloco
		if (eSetorTrailer)
		{
			status = PCD_Authenticate(PICC_CMD_MF_AUTH_KEY_A, primeiroBloco, key, uid);
			if (status != STATUS_OK)
			{
				Serial.print(F("PCD_Authenticate() falhou: "));
				Serial.println(GetStatusCodeName(status));
				return;
			}
		}
		// Lê o bloco
		contadorDeBytes = sizeof(buffer);
		status = MIFARE_Read(enderecoDoBloco, buffer, &contadorDeBytes);
		if (status != STATUS_OK)
		{
			Serial.print(F("MIFARE_Read() falhou: "));
			Serial.println(GetStatusCodeName(status));
			continue;
		}
		// Exibe os dados
		for (byte indice = 0; indice < 16; indice++)
		{
			if (buffer[indice] < 0x10)
				Serial.print(F(" 0"));
			else
				Serial.print(F(" "));
			Serial.print(buffer[indice], HEX);
			if ((indice % 4) == 3)
			{
				Serial.print(F(" "));
			}
		}
		// Analisa os dados do trailer do setor
		if (eSetorTrailer)
		{
			c1 = buffer[7] >> 4;
			c2 = buffer[8] & 0xF;
			c3 = buffer[8] >> 4;
			c1_ = buffer[6] & 0xF;
			c2_ = buffer[6] >> 4;
			c3_ = buffer[7] & 0xF;
			erroInvertido = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
			g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
			g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
			g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
			g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
			eSetorTrailer = false;
		}

		// Em qual grupo de acesso está este bloco?
		if (numeroDeBlocos == 4)
		{
			grupo = deslocamentoDoBloco;
			primeiroNoGrupo = true;
		}
		else
		{
			grupo = deslocamentoDoBloco / 5;
			primeiroNoGrupo = (grupo == 3) || (grupo != (deslocamentoDoBloco + 1) / 5);
		}

		if (primeiroNoGrupo)
		{
			// Exibe bits de acesso
			Serial.print(F(" [ "));
			Serial.print((g[grupo] >> 2) & 1, DEC);
			Serial.print(F(" "));
			Serial.print((g[grupo] >> 1) & 1, DEC);
			Serial.print(F(" "));
			Serial.print((g[grupo] >> 0) & 1, DEC);
			Serial.print(F(" ] "));
			if (erroInvertido)
			{
				Serial.print(F(" Bits de acesso invertidos não coincidem! "));
			}
		}

		if (grupo != 3 && (g[grupo] == 1 || g[grupo] == 6))
		{ // Não é um trailer de setor, um bloco de valor
			int32_t valor = (int32_t(buffer[3]) << 24) | (int32_t(buffer[2]) << 16) | (int32_t(buffer[1]) << 8) | int32_t(buffer[0]);
			Serial.print(F(" Valor=0x"));
			Serial.print(valor, HEX);
			Serial.print(F(" End=0x"));
			Serial.print(buffer[12], HEX);
		}
		Serial.println();
	}

	return;
} // Fim PICC_DumpMifareClassicSectorToSerial()

/**
 * Exibe o conteúdo da memória de um PICC MIFARE Ultralight.
 */
void MFRC522::PICC_DumpMifareUltralightToSerial()
{
	MFRC522::StatusCode status;
	byte contadorDeBytes;
	byte buffer[18];
	byte i;

	Serial.println(F("Página  0  1  2  3"));
	// Tenta as páginas do Ultralight original. O Ultralight C tem mais páginas.
	for (byte pagina = 0; pagina < 16; pagina += 4)
	{ // A leitura retorna dados para 4 páginas de cada vez.
		// Lê as páginas
		contadorDeBytes = sizeof(buffer);
		status = MIFARE_Read(pagina, buffer, &contadorDeBytes);
		if (status != STATUS_OK)
		{
			Serial.print(F("MIFARE_Read() falhou: "));
			Serial.println(GetStatusCodeName(status));
			break;
		}
		// Exibe os dados
		for (byte deslocamento = 0; deslocamento < 4; deslocamento++)
		{
			i = pagina + deslocamento;
			if (i < 10)
				Serial.print(F("  ")); // Preenche com espaços
			else
				Serial.print(F(" ")); // Preenche com espaços
			Serial.print(i);
			Serial.print(F("  "));
			for (byte indice = 0; indice < 4; indice++)
			{
				i = 4 * deslocamento + indice;
				if (buffer[i] < 0x10)
					Serial.print(F(" 0"));
				else
					Serial.print(F(" "));
				Serial.print(buffer[i], HEX);
			}
			Serial.println();
		}
	}
} // Fim PICC_DumpMifareUltralightToSerial()

/**
 * Calcula o padrão de bits necessário para os bits de acesso especificados. Nas tuplas [C1 C2 C3], C1 é o MSB (=4) e C3 é o LSB (=1).
 */
void MFRC522::MIFARE_SetAccessBits(byte *accessBitBuffer, ///< Ponteiro para o byte 6, 7 e 8 no trailer do setor. Bytes [0..2] serão definidos.
								   byte g0,				  ///< Bits de acesso [C1 C2 C3] para o bloco 0 (para setores 0-31) ou blocos 0-4 (para setores 32-39)
								   byte g1,				  ///< Bits de acesso [C1 C2 C3] para o bloco 1 (para setores 0-31) ou blocos 5-9 (para setores 32-39)
								   byte g2,				  ///< Bits de acesso [C1 C2 C3] para o bloco 2 (para setores 0-31) ou blocos 10-14 (para setores 32-39)
								   byte g3				  ///< Bits de acesso [C1 C2 C3] para o trailer do setor, bloco 3 (para setores 0-31) ou bloco 15 (para setores 32-39)
)
{
	byte c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
	byte c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
	byte c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);

	accessBitBuffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
	accessBitBuffer[1] = c1 << 4 | (~c3 & 0xF);
	accessBitBuffer[2] = c3 << 4 | c2;
} // Fim MIFARE_SetAccessBits()

/**
 * Executa a "sequência mágica" necessária para permitir a gravação no setor 0 de cartões Mifare com UID chinês modificável.
 *
 * Observe que você não precisa ter selecionado o cartão através de REQA ou WUPA,
 * essa sequência funciona imediatamente quando o cartão está na vizinhança do leitor.
 * Isso significa que você pode usar esse método mesmo em cartões "bricked" que o seu leitor não reconhece mais (consulte MFRC522::MIFARE_UnbrickUidSector).
 *
 * Claro, com dispositivos não "bricked", você pode selecioná-los antes de chamar essa função.
 */
bool MFRC522::MIFARE_OpenUidBackdoor(bool registrarErros)
{
	// Sequência mágica:
	// > 50 00 57 CD (HALT + CRC)
	// > 40 (apenas 7 bits)
	// < A (apenas 4 bits)
	// > 43
	// < A (apenas 4 bits)
	// Em seguida, você pode gravar no setor 0 sem autenticação

	PICC_HaltA(); // 50 00 57 CD

	byte cmd = 0x40;
	byte validBits = 7; /* Nosso comando tem apenas 7 bits. Após receber a resposta do cartão,
						  isso conterá a quantidade de bits de resposta válidos. */
	byte response[32];	// A resposta do cartão é escrita aqui
	byte received = sizeof(response);
	MFRC522::StatusCode status = PCD_TransceiveData(&cmd, (byte)1, response, &received, &validBits, (byte)0, false); // 40
	if (status != STATUS_OK)
	{
		if (registrarErros)
		{
			Serial.println(F("O cartão não respondeu ao comando 0x40 após o comando HALT. Você tem certeza de que é um cartão com UID modificável?"));
			Serial.print(F("Nome do erro: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}
	if (received != 1 || response[0] != 0x0A)
	{
		if (registrarErros)
		{
			Serial.print(F("Recebeu uma resposta ruim no comando 0x40 de backdoor: "));
			Serial.print(response[0], HEX);
			Serial.print(F(" ("));
			Serial.print(validBits);
			Serial.print(F(" bits válidos)\r\n"));
		}
		return false;
	}

	cmd = 0x43;
	validBits = 8;
	status = PCD_TransceiveData(&cmd, (byte)1, response, &received, &validBits, (byte)0, false); // 43
	if (status != STATUS_OK)
	{
		if (registrarErros)
		{
			Serial.println(F("Erro na comunicação no comando 0x43, após a execução bem-sucedida do comando 0x40"));
			Serial.print(F("Nome do erro: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}
	if (received != 1 || response[0] != 0x0A)
	{
		if (registrarErros)
		{
			Serial.print(F("Recebeu uma resposta ruim no comando 0x43 de backdoor: "));
			Serial.print(response[0], HEX);
			Serial.print(F(" ("));
			Serial.print(validBits);
			Serial.print(F(" bits válidos)\r\n"));
		}
		return false;
	}

	// Agora você pode escrever no setor 0 sem autenticação!
	return true;
} // Fim MIFARE_OpenUidBackdoor()

/**
 * Lê todo o bloco 0, incluindo todos os dados do fabricante, e sobrescreve
 * esse bloco com o novo UID, um BCC recém-calculado e os dados originais do fabricante.
 *
 * Assume um KEY A padrão de 0xFFFFFFFFFFFF.
 * Certifique-se de ter selecionado o cartão antes que esta função seja chamada.
 */
bool MFRC522::MIFARE_SetUid(byte *novoUid, byte tamanhoUid, bool registrarErros)
{

	// UID + byte BCC não pode ser maior que 16 juntos
	if (!novoUid || !tamanhoUid || tamanhoUid > 15)
	{
		if (registrarErros)
		{
			Serial.println(F("Buffer de novo UID vazio, tamanho 0 ou tamanho > 15 fornecido"));
		}
		return false;
	}

	// Autenticar para leitura
	MIFARE_Key chave = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	MFRC522::StatusCode status = PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (byte)1, &chave, &uid);
	if (status != STATUS_OK)
	{

		if (status == STATUS_TIMEOUT)
		{
			// Obtemos um timeout de leitura se nenhum cartão foi selecionado ainda, então vamos selecionar um

			// Despertar o cartão novamente se estiver dormindo
			//			  byte atqa_answer[2];
			//			  byte atqa_size = 2;
			//			  PICC_WakeupA(atqa_answer, &atqa_size);

			if (!PICC_IsNewCardPresent() || !PICC_ReadCardSerial())
			{
				Serial.println(F("Nenhum cartão foi selecionado anteriormente e nenhum está disponível. Falha ao definir o UID."));
				return false;
			}

			status = PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (byte)1, &chave, &uid);
			if (status != STATUS_OK)
			{
				// Tentamos, hora de desistir
				if (registrarErros)
				{
					Serial.println(F("Falha na autenticação do cartão para leitura, não foi possível definir o UID: "));
					Serial.println(GetStatusCodeName(status));
				}
				return false;
			}
		}
		else
		{
			if (registrarErros)
			{
				Serial.print(F("PCD_Authenticate() falhou: "));
				Serial.println(GetStatusCodeName(status));
			}
			return false;
		}
	}

	// Lê o bloco 0
	byte bufferBloco0[18];
	byte contadorDeBytes = sizeof(bufferBloco0);
	status = MIFARE_Read((byte)0, bufferBloco0, &contadorDeBytes);
	if (status != STATUS_OK)
	{
		if (registrarErros)
		{
			Serial.print(F("MIFARE_Read() falhou: "));
			Serial.println(GetStatusCodeName(status));
			Serial.println(F("Você tem certeza de que seu KEY A para o setor 0 é 0xFFFFFFFFFFFF?"));
		}
		return false;
	}

	// Escreve o novo UID nos dados que acabamos de ler e calcula o byte BCC
	byte bcc = 0;
	for (uint8_t i = 0; i < tamanhoUid; i++)
	{
		bufferBloco0[i] = novoUid[i];
		bcc ^= novoUid[i];
	}

	// Escreve o byte BCC no buffer
	bufferBloco0[tamanhoUid] = bcc;

	// Interrompe o tráfego criptografado para que possamos enviar bytes crus
	PCD_StopCrypto1();

	// Ativa a porta de entrada do UID
	if (!MIFARE_OpenUidBackdoor(registrarErros))
	{
		if (registrarErros)
		{
			Serial.println(F("Ativar a porta de entrada do UID falhou."));
		}
		return false;
	}

	// Escreve o bloco 0 modificado de volta ao cartão
	status = MIFARE_Write((byte)0, bufferBloco0, (byte)16);
	if (status != STATUS_OK)
	{
		if (registrarErros)
		{
			Serial.print(F("MIFARE_Write() falhou: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}

	// Desperta o cartão novamente
	byte atqa_answer[2];
	byte atqa_size = 2;
	PICC_WakeupA(atqa_answer, &atqa_size);

	return true;
}

/**
 * Reseta o setor 0 inteiro para zeros, para que o cartão possa ser lido novamente por leitores.
 */
bool MFRC522::MIFARE_UnbrickUidSector(bool registrarErros)
{
	MIFARE_OpenUidBackdoor(registrarErros);

	byte bufferBloco0[] = {0x01, 0x02, 0x03, 0x04, 0x04, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	// Escreve o bloco 0 modificado de volta ao cartão
	MFRC522::StatusCode status = MIFARE_Write((byte)0, bufferBloco0, (byte)16);
	if (status != STATUS_OK)
	{
		if (registrarErros)
		{
			Serial.print(F("MIFARE_Write() falhou: "));
			Serial.println(GetStatusCodeName(status));
		}
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
// Funções de conveniência - não adicionam funcionalidade extra
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Retorna verdadeiro se um PICC responder ao PICC_CMD_REQA.
 * Somente "novos" cartões no estado IDLE são convidados. Cartões em estado HALT não são considerados.
 *
 * @return bool
 */
bool MFRC522::PICC_EstaNovoCartaoPresente()
{
	byte bufferATQA[2];
	byte tamanhoBuffer = sizeof(bufferATQA);

	// Redefine as taxas de transmissão
	PCD_EscreveRegistro(TxModeReg, 0x00);
	PCD_EscreveRegistro(RxModeReg, 0x00);
	// Redefine ModWidthReg
	PCD_EscreveRegistro(ModWidthReg, 0x26);

	MFRC522::StatusCode resultado = PICC_RequerA(bufferATQA, &tamanhoBuffer);
	return (resultado == STATUS_OK || resultado == STATUS_COLISAO);
} // Fim PICC_EstaNovoCartaoPresente()

/**
 * Invólucro simples ao redor de PICC_Select.
 * Retorna verdadeiro se um UID puder ser lido.
 * Lembre-se de chamar PICC_EstaNovoCartaoPresente(), PICC_RequerA() ou PICC_DespertarA() primeiro.
 * O UID lido está disponível na variável de classe uid.
 *
 * @return bool
 */
bool MFRC522::PICC_LerCartaoSerial()
{
	MFRC522::StatusCode resultado = PICC_Seleciona(&uid);
	return (resultado == STATUS_OK);
} // Fim PICC_LerCartaoSerial()
