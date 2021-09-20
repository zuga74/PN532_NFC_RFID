/*
 * myPN532.c
 *
 *  Created on: 21 июн. 2021 г.
 *      Author: lenovo
 */

#include "myPN532.h"
#include <string.h>


const uint8_t pn532_ack[] = {PN532_PREAMBLE, PN532_STARTCODE1, PN532_STARTCODE2, 0x00, 0xFF, PN532_POSTAMBLE};


static uint8_t PN532_send_buf[PN532_SEND_BUF_SIZE];
static uint8_t * PN532_send_buf_data = PN532_send_buf + 5;
static uint8_t PN532_recv_buf[PN532_RECV_BUF_SIZE];
uint8_t * get_PN532_recv_buf(void) { return PN532_recv_buf; }

void PN532_print_mem(uint8_t *mem, uint8_t cnt, char * str)
{
/*
    char PN532_print_mem_buf[128];

    if (cnt > 0) {
    	mem2str(mem, cnt, PN532_print_mem_buf, 128);
    	ulog("PN532: ");
    	ulog(str);
    	ulog(" ");
    	ulog(PN532_print_mem_buf);
    	//ulog_fmt("(%ld)\r\n", (int)HAL_GetTick());
    	ulog("\r\n");
    }
*/
}



//послать данные в PN532 с формироваеием фрейма
//send data to PN532 with framing
BOOL PN532_send_data(uint8_t *data, uint8_t data_len) {

    uint8_t i, checksum;

    checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
    PN532_send_buf[0] = PN532_PREAMBLE;
    PN532_send_buf[1] = PN532_PREAMBLE;
    PN532_send_buf[2] = PN532_STARTCODE2;
    PN532_send_buf[3] = data_len;
    PN532_send_buf[4] = ~data_len + 1;

    if (PN532_send_buf_data != data) memcpy(PN532_send_buf_data, data, data_len);

    for (i = 0; i < data_len; i++) checksum += data[i];

    PN532_send_buf[5 + data_len] = (~checksum) & 0xFF;
    PN532_send_buf[6 + data_len] = PN532_POSTAMBLE;

	PN532_print_mem(PN532_send_buf, 7 + data_len, "send");

    return PN532_TRANSMIT(PN532_send_buf, 7 + data_len, PN532_SEND_TIMEOUT);;
}


//дождаться байта PN532_I2C_READY
//wait for byte PN532_I2C_READY
BOOL PN532_WaitReady(uint32_t timeout) {
    uint8_t b;
    uint32_t tickstart = HAL_GetTick();

    while (HAL_Diff(tickstart) < timeout) {
    	if (PN532_RECEIVE(&b, 1, timeout)) {
            if (b == PN532_I2C_READY) return TRUE;
    	}
    	HAL_Delay(1);
    }
    return FALSE;
}



//получить данные из PN532
//get data from PN532
uint8_t PN532_recv(uint8_t * buf, uint8_t how)
{

	if (!PN532_WaitReady(PN532_RECV_READY_TIMEOUT)) return 0;

    if (PN532_RECEIVE(buf, how, PN532_RECV_DATA_TIMEOUT)) return how;

	return 0;
}


//получить ACK из PN532
//get ACK from PN532
BOOL PN532_recv_ack(void) {

	uint8_t cnt;

	memset(PN532_recv_buf, 0, PN532_RECV_BUF_SIZE);

	cnt = PN532_recv(PN532_recv_buf, sizeof(pn532_ack) + 1);
	if (cnt == 0) return FALSE;

	if (PN532_recv_buf[0] != PN532_I2C_READY) return 0;

	PN532_print_mem(PN532_recv_buf, cnt, "recv ack");

	return (0 == memcmp(PN532_recv_buf + 1, pn532_ack, sizeof(pn532_ack)));
}


//парсинг frame-а PN532
//parsing the PN532 frame
uint8_t * PN532_parse_frame(uint8_t * buf, uint8_t buf_size, uint8_t * data_len)
{
    uint8_t i, checksum, u8;

    if (buf[0] != PN532_PREAMBLE) return NULL;
    if (buf[1] != PN532_PREAMBLE) return NULL;
    if (buf[2] != PN532_STARTCODE2) return NULL;
    u8 = ~buf[3] + 1;
    if (buf[4] != u8) return NULL;

    checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
    for (i = 0; i < buf[3]; ++i)  checksum += buf[5 + i];
    u8 = (~checksum) & 0xFF;


    if (buf[5 + buf[3]] != u8) return NULL;


    *data_len = buf[3];
    return buf + 5;
}

//получить данные из PN532
//get data from PN532
uint8_t * PN532_recv_data(uint8_t * buf, uint8_t buf_size, uint8_t * data_len)
{
	uint8_t cnt, * data = NULL, i;

	cnt = PN532_recv(buf, buf_size);

	PN532_print_mem(buf, cnt, "recv data");

	if (cnt < 8) return NULL;

	if (buf[0] != PN532_I2C_READY) return 0;

	//data = PN532_parse_frame(buf + 1, cnt - 1, data_len);

	for (i = 1 ; i < cnt - 6; ++i) {
		data = PN532_parse_frame(buf + i, cnt - i, data_len);
		if (data) break;
	}

	return data;
}


//--------------------------- MIFARE ----------------------------------------


BOOL PN532_SendGetFirmwareVersion(void)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_GETFIRMWAREVERSION;

	return PN532_send_data(PN532_send_buf_data, 2);
}

uint8_t PN532_GetFirmVersion(uint8_t * version)
{
	uint8_t * data, data_len;

	//PN532: send: 0000FF02FED4022A00
	if (!PN532_SendGetFirmwareVersion()) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv data 01 00 00 FF 06 FA D5 03 32 01 06 07 E8 00
	data = PN532_recv_data(PN532_recv_buf, 14, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 6) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_GETFIRMWAREVERSION + 1)) return PN532_ERROR_RECV_COMMAND;

	memcpy(version, data + 2, 4);

	return PN532_ERROR_NONE;
}

BOOL PN532_SendSamConfig(uint8_t mode, uint8_t timeout, uint8_t use_irq)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_SAMCONFIGURATION;
	PN532_send_buf_data[2] = mode;
	PN532_send_buf_data[3] = timeout;
	PN532_send_buf_data[4] = use_irq;

	return PN532_send_data(PN532_send_buf_data, 5);
}

uint8_t PN532_SetSamConfig(uint8_t mode, uint8_t timeout, uint8_t use_irq)
{
	uint8_t * data, data_len;

	//0x01 -normal mode, 0x14 - timeout 50ms * 20 = 1 second,  0x01- use IRQ pin!
	//PN532: send: 00 00 FF 05 FB D4 14 01 14 01 02 00
	if (!PN532_SendSamConfig(mode, timeout, use_irq)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv data 01 00 00 FF 02 FE D5 15 16 00 01 02 E8 00 (GOOD)
	//PN532: recv data 01 00 00 FF 01 FF 7F 81 00 (BAD - Error frame)
	data = PN532_recv_data(PN532_recv_buf, 14, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 2) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_SAMCONFIGURATION + 1)) return PN532_ERROR_RECV_COMMAND;

	return PN532_ERROR_NONE;
}


BOOL PN532_SendReadPassiveTarget(uint8_t card_baud)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_INLISTPASSIVETARGET;
	PN532_send_buf_data[2] = 1; // max 1 cards at once (we can set this to 2 later)
	PN532_send_buf_data[3] = card_baud;

	return PN532_send_data(PN532_send_buf_data, 4);
}
uint8_t PN532_ReadPassTarget(uint8_t card_baud, uint8_t * uid, uint8_t uid_size, uint8_t *uid_len)
{
	uint8_t i, * data, data_len;

	//PN532: send: 00 00 FF 04 FC D4 4A 01 00 E1 00
	if (!PN532_SendReadPassiveTarget(card_baud)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv data 01 00 00 FF 0C F4 D5 4B 01 01 00 04 08 04 5C BB E5 D5 FD 00 //detect uid=15055708 uid_len=4
	data = PN532_recv_data(PN532_recv_buf, 14 + MIFARE_UID_MAX_LENGTH + 2, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 7) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_INLISTPASSIVETARGET + 1)) return PN532_ERROR_RECV_COMMAND;

	if (data[2] != 0x01) return PN532_ERROR_MORE_ONE_CARD;

	if (data_len < 7 + data[7]) return PN532_ERROR_FRAMING_LEN;

	*uid_len = data[7] < uid_size ? data[7] : uid_size;
    for (i = 0; i < *uid_len; i++) uid[i] = data[8 + i];


	return PN532_ERROR_NONE;
}

BOOL PN532_SendMifareClassicAuthBlock(uint8_t *uid, uint8_t uid_len,  uint8_t block_number,  uint8_t key_number, uint8_t * key)
{
	uint8_t i;

    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_INDATAEXCHANGE;
	PN532_send_buf_data[2] = 1;        // Max card numbers
	PN532_send_buf_data[3] = key_number; //MIFARE_CMD_AUTH_A or MIFARE_CMD_AUTH_B;
	PN532_send_buf_data[4] = block_number; // Block Number (1K = 0..63, 4K = 0..255
	memcpy(PN532_send_buf_data + 5, key, 6);
	for (i = 0; i < uid_len; i++) PN532_send_buf_data[11 + i] = uid[i]; /* 4 uint8_t card ID */

	return PN532_send_data(PN532_send_buf_data, 11 + uid_len);
}


uint8_t PN532_MifareClassicAuthBlock(uint8_t *uid, uint8_t uid_len,  uint8_t block_number,  uint8_t key_number, uint8_t * key)
{
	uint8_t * data, data_len;


	//PN532: send 00 00 FF 0F F1 D4 40 01 60 0A FF FF FF FF FF FF 5C BB E5 D5 B6 00
	//PN532: send 00 00 FF 0F F1 D4 40 01 60 1F FE E1 DE FE C8 ED 5C BB E5 D5 2B 00

	if (!PN532_SendMifareClassicAuthBlock(uid, uid_len, block_number, key_number, key)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv 01 00 00 FF 03 FD D5 41 00 EA 00
	data = PN532_recv_data(PN532_recv_buf, 11, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_INDATAEXCHANGE + 1)) return PN532_ERROR_RECV_COMMAND;

	if (data_len < 3) return PN532_ERROR_FRAMING_LEN;

	return data[2];
}

BOOL PN532_SendReadDataBlock(uint8_t block_number)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_INDATAEXCHANGE;
	PN532_send_buf_data[2] = 1;               // Card number
	PN532_send_buf_data[3] = MIFARE_CMD_READ; // Mifare Read command = 0x30
	PN532_send_buf_data[4] = block_number; // Block Number (0..63 for 1K, 0..255 for 4K)

	return PN532_send_data(PN532_send_buf_data, 5);
}


uint8_t PN532_ReadDataBlock(uint8_t block_number, uint8_t * block_data)
{
	uint8_t * data, data_len;

	//PN532: send 00 00 FF 05 FB D4 40 01 30 0A B1 00
	if (!PN532_SendReadDataBlock(block_number)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv data 01 00 00 FF 13 ED D5 41 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 EA 00
	//PN532: recv data 01 00 00 FF 13 ED D5 41 00 00 00 00 00 00 00 FF 07 80 69 FE E1 DE FE C8 ED 8B 00

	data = PN532_recv_data(PN532_recv_buf, 27, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_INDATAEXCHANGE + 1)) return PN532_ERROR_RECV_COMMAND;

	if (data_len < 19) return  PN532_ERROR_FRAMING_LEN;

	memcpy(block_data, data + 3, 16);

	return data[2];
}

BOOL PN532_SendWriteDataBlock(uint8_t block_number,  uint8_t * block_data)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_INDATAEXCHANGE;
	PN532_send_buf_data[2] = 1;                // Card number
	PN532_send_buf_data[3] = MIFARE_CMD_WRITE; // Mifare Write command = 0xA0
	PN532_send_buf_data[4] = block_number; // Block Number (0..63 for 1K, 0..255 for 4K)

	memcpy(PN532_send_buf_data + 5, block_data, 16); // Data Payload

	return PN532_send_data(PN532_send_buf_data, 21);
}

uint8_t PN532_WriteDataBlock(uint8_t block_number,  uint8_t * block_data)
{
	uint8_t * data, data_len;

	//PN532: send 00 00 FF 15 EB D4 40 01 A0 0A (ron b) FE E1 DE FE C8 ED (ron e) FF 07 80 69 (ron b) FE E1 DE FE C8 ED (ron e) 72 00
	if (!PN532_SendWriteDataBlock(block_number, block_data)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv data 01 00 00 FF 03 FD D5 41 00 EA 00 E1 DE FE C8 ED FF 07 80 69 FE E1 DE FE C8 ED 72 00
	data = PN532_recv_data(PN532_recv_buf, 28, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 3) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_INDATAEXCHANGE + 1)) return PN532_ERROR_RECV_COMMAND;


	return data[2];
}

BOOL PN532_SendSetParameters(uint8_t params)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_SETPARAMETERS;
	PN532_send_buf_data[2] = params;

	return PN532_send_data(PN532_send_buf_data, 3);
}

uint8_t PN532_SetParameters(uint8_t params)
{
	uint8_t  * data, data_len;

	if (!PN532_SendSetParameters(params)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	data = PN532_recv_data(PN532_recv_buf, PN532_RECV_BUF_SIZE, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 2) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_SETPARAMETERS + 1)) return PN532_ERROR_RECV_COMMAND;

	return PN532_ERROR_NONE;
}

//--------------------------- NTAG ----------------------------------------

BOOL PN532_SendNtag2xxAuth(uint8_t * pwd)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_INCOMMUNICATETHRU;
	PN532_send_buf_data[2] = 0x1B;
	PN532_send_buf_data[3] = pwd[0];
	PN532_send_buf_data[4] = pwd[1];
	PN532_send_buf_data[5] = pwd[2];
	PN532_send_buf_data[6] = pwd[3];

	return PN532_send_data(PN532_send_buf_data, 7);
}


uint8_t PN532_Ntag2xxAuth(uint8_t *pwd, uint8_t* response) {
	uint8_t  * data, data_len, i;

	//PN532: send 00 00 FF 07 F9 D4 42 1B 52 84 00 08 F1 00

	if (!PN532_SendNtag2xxAuth(pwd)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv 01 00 00 FF 03 FD D5 43 01 E7 00 00 08 F1 08 FC 70 30 05 78 80 71 00 5D 00
	//PN532: recv 01 00 00 FF 03 FD D5 43 01 E7 00 70 30 2B 08 FC 70 30 05 78 80 71 00 5D 00
	data = PN532_recv_data(PN532_recv_buf, PN532_RECV_BUF_SIZE, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 9) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_INCOMMUNICATETHRU + 1)) return PN532_ERROR_RECV_COMMAND;

	for (i = 0; i < 6; ++i) response[i] = data[3 + i];

	return data[2];
}


BOOL PN532_SendNtag2xxReadBlock(uint8_t block_number)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_INDATAEXCHANGE;
	PN532_send_buf_data[2] = 0x01;
	PN532_send_buf_data[3] = MIFARE_CMD_READ;
	PN532_send_buf_data[4] = block_number;

	return PN532_send_data(PN532_send_buf_data, 5);
}

uint8_t PN532_Ntag2xxReadBlock(uint8_t block_number, uint8_t * block_data)
{
	uint8_t  * data, data_len, i;

	//PN532: send 00 00 FF 05 FB D4 40 01 30 04 B7 00
	if (!PN532_SendNtag2xxReadBlock(block_number)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	//PN532: recv data 01 00 00 FF 05 FB D5 41 00 6E 00 7C 00
	data = PN532_recv_data(PN532_recv_buf, PN532_RECV_BUF_SIZE, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 3 + NTAG2XX_BLOCK_LENGTH) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_INDATAEXCHANGE + 1)) return PN532_ERROR_RECV_COMMAND;

    for (i = 0; i < NTAG2XX_BLOCK_LENGTH; i++)  block_data[i] = data[3 + 1];

	return data[2];
}


BOOL PN532_SendNtag2xxWriteBlock(uint8_t block_number, uint8_t * block_data)
{
    PN532_send_buf_data[0] = PN532_HOSTTOPN532;
	PN532_send_buf_data[1] = PN532_COMMAND_INDATAEXCHANGE;
	PN532_send_buf_data[2] = 0x01;
	PN532_send_buf_data[3] = MIFARE_ULTRALIGHT_CMD_WRITE;
	PN532_send_buf_data[4] = block_number;

    for (uint8_t i = 0; i < NTAG2XX_BLOCK_LENGTH; i++) PN532_send_buf_data[5 + i] = block_data[i];

	return PN532_send_data(PN532_send_buf_data, 9);
}


uint8_t PN532_Ntag2xxWriteBlock(uint8_t block_number, uint8_t * block_data)
{
	uint8_t  * data, data_len;

	if (!PN532_SendNtag2xxWriteBlock(block_number, block_data)) return PN532_ERROR_SEND_DATA;

	if (!PN532_recv_ack()) return PN532_ERROR_RECV_ACK;

	data = PN532_recv_data(PN532_recv_buf, PN532_RECV_BUF_SIZE, &data_len);
	if ( (data == NULL) || (data_len == 0) ) return PN532_ERROR_RECV_DATA;

	if (data_len < 3) return PN532_ERROR_FRAMING_LEN;

	if (data[0] != PN532_PN532TOHOST) return PN532_ERROR_RECV_TOHOST;
	if (data[1] != (PN532_COMMAND_INDATAEXCHANGE + 1)) return PN532_ERROR_RECV_COMMAND;

	return data[2];
}

