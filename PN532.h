/*
 * PN532.h
 *
 *  Created on: 21 июн. 2021 г.
 *      Author: Rafael Boltachev
 */

#ifndef PN532_PN532_H_
#define PN532_PN532_H_

#include "PN532_def.h"


// ---------------------------- HARDWARE -----------------------------

//I2C device handle
#ifndef PN532_I2C_HANDLE
#define PN532_I2C_HANDLE 				hi2c2
#endif

//I2C device address
#ifndef PN532_I2C_ADDR
#define PN532_I2C_ADDR					(0x24 << 1)
#endif

//Send data to PN532
#ifndef PN532_TRANSMIT
#define PN532_TRANSMIT(b, s, t) 		(HAL_I2C_Master_Transmit(&PN532_I2C_HANDLE, PN532_I2C_ADDR, b, s, t) == HAL_OK)
#endif

//Receive data from PN532
#ifndef PN532_RECEIVE
#define PN532_RECEIVE(b, s, t) 			(HAL_I2C_Master_Receive(&PN532_I2C_HANDLE, PN532_I2C_ADDR, b, s, t) == HAL_OK)
#endif

//Delay in milliseconds
#ifndef PN532_DELAY
#define PN532_DELAY						HAL_Delay
#endif

//Returns the time in milliseconds
#ifndef PN532_GET_TICK
#define PN532_GET_TICK					HAL_GetTick
#endif

//Difference with current time
#ifndef PN532_TICK_DIFF
#define PN532_TICK_DIFF(t)				( (PN532_GET_TICK() >= t) ? (PN532_GET_TICK() - t) : (0xFFFFFFFF - t + PN532_GET_TICK()) )
#endif

// ---------------------------- SOFTWARE -----------------------------

#ifndef BOOL
#define BOOL _Bool
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//Buffer size for a sent data
#ifndef PN532_SEND_BUF_SIZE
#define PN532_SEND_BUF_SIZE				64
#endif

//Buffer size for received data
#ifndef PN532_RECV_BUF_SIZE
#define PN532_RECV_BUF_SIZE				64
#endif

//Timeout for sending data to PN532
#ifndef PN532_SEND_TIMEOUT
#define PN532_SEND_TIMEOUT				100
#endif

//Timeout to receive Ready from PN532
#ifndef PN532_RECV_READY_TIMEOUT
#define PN532_RECV_READY_TIMEOUT		25
#endif

//Timeout for receiving data from PN532
#ifndef PN532_RECV_DATA_TIMEOUT
#define PN532_RECV_DATA_TIMEOUT			15
#endif


#define PN532_I2C_READY                     (0x01)


#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)

#define PN532_HOSTTOPN532                   (0xD4)
#define PN532_PN532TOHOST                   (0xD5)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)

#define PN532_WAKEUP                        (0x55)


#define PN532_MIFARE_ISO14443A              (0x00)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)
#define MIFARE_ULTRALIGHT_CMD_WRITE         (0xA2)

#define MIFARE_UID_MAX_LENGTH               MIFARE_UID_TRIPLE_LENGTH
#define MIFARE_UID_SINGLE_LENGTH            (4)
#define MIFARE_UID_DOUBLE_LENGTH            (7)
#define MIFARE_UID_TRIPLE_LENGTH            (10)
#define MIFARE_KEY_LENGTH                   (6)
#define MIFARE_BLOCK_LENGTH                 (16)

// NTAG2xx Commands
#define NTAG2XX_BLOCK_LENGTH                (4)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

#define PN532_GPIO_VALIDATIONBIT            (0x80)


#define PN532_PARAM_NADUSED_BIT					(0)
#define PN532_PARAM_DIDUSED_BIT					(1)
#define PN532_PARAM_AUTOMATIC_ATR_BIT			(2)
#define PN532_PARAM_AUTOMATIC_RATS_BIT			(4)
#define PN532_PARAM_ISO14443_4_BIT				(5)
#define PN532_PARAM_REMOVE_PRE_POST_AMBLE_BIT	(6)

/* Official PN532 Errors Definitions */
#define PN532_ERROR_NONE                                                (0x00)
// Time Out, the target has not answered
#define PN532_ERROR_TIMEOUT                                             (0x01)
// A CRC error has been detected by the CIU
#define PN532_ERROR_CRC                                                 (0x02)
// A Parity error has been detected by the CIU
#define PN532_ERROR_PARITY                                              (0x03)
// During an anti-collision/select operation (ISO/IEC14443-3 Type A and
// ISO/IEC18092 106 kbps passive mode), an erroneous Bit Count has been
// detected
#define PN532_ERROR_COLLISION_BITCOUNT                                  (0x04)
// Framing error during MIFARE operation
#define PN532_ERROR_MIFARE_FRAMING                                      (0x05)
// An abnormal bit-collision has been detected during bit wise
// anti-collision at 106 kbps
#define PN532_ERROR_COLLISION_BITCOLLISION                              (0x06)
// Communication buffer size insufficien
#define PN532_ERROR_NOBUFS                                              (0x07)
// RF Buffer overflow has been detected by the CI
#define PN532_ERROR_RFNOBUFS                                            (0x09)
// In active communication mode, the RF field has not been switched on
// in time by the counterpart (as defined in NFCIP-1 standard
#define PN532_ERROR_ACTIVE_TOOSLOW                                      (0x0a)
// RF Protocol error
#define PN532_ERROR_RFPROTO                                             (0x0b)
// Temperature error: the internal temperature sensor has detected
// overheating, and therefore has automatically switched off the
// antenna drivers
#define PN532_ERROR_TOOHOT                                              (0x0d)
// Internal buffer overflow
#define PN532_ERROR_INTERNAL_NOBUFS                                     (0x0e)
// Invalid parameter (range, format...)
#define PN532_ERROR_INVAL                                               (0x10)
// DEP Protocol: The PN533 configured in target mode does not support
// the command received from the initiator (the command received is not
// one of the following: ATR_REQ, WUP_REQ, PSL_REQ, DEP_REQ, DSL_REQ,
// RLS_REQ)
#define PN532_ERROR_DEP_INVALID_COMMAND                                 (0x12)
// DEP Protocol, MIFARE or ISO/IEC14443-4: The data format does not
// match to the specification.  Depending on the RF protocol used, it
// can be: Bad length of RF received frame, Incorrect value of PCB or
// PFB, Invalid or unexpected RF received frame, NAD or DID incoherence.
#define PN532_ERROR_DEP_BADDATA                                         (0x13)
// MIFARE: Authentication error
#define PN532_ERROR_MIFARE_AUTH                                         (0x14)
// Target or Initiator does not support NFC Secur
#define PN532_ERROR_NOSECURE                                            (0x18)
// I2C bus line is Busy. A TDA transaction is on going
#define PN532_ERROR_I2CBUSY                                             (0x19)
// ISO/IEC14443-3: UID Check byte is wrong
#define PN532_ERROR_UIDCHECKSUM                                         (0x23)
// DEP Protocol: Invalid device state, the system is in a state which
// does not allow the operation
#define PN532_ERROR_DEPSTATE                                            (0x25)
// Operation not allowed in this configuration (host controller
// interface)
#define PN532_ERROR_HCIINVAL                                            (0x26)
// This command is not acceptable due to the current context of the
// PN533 (Initiator vs. Target, unknown target number, Target not in the
// good state, ...)
#define PN532_ERROR_CONTEXT                                             (0x27)
// The PN533 configured as target has been released by its initiator
#define PN532_ERROR_RELEASED                                            (0x29)
// PN533 and ISO/IEC14443-3B only: the ID of the card does not match,
// meaning that the expected card has been exchanged with another one.
#define PN532_ERROR_CARDSWAPPED                                         (0x2a)
// PN533 and ISO/IEC14443-3B only: the card previously activated has
// disappeared.
#define PN532_ERROR_NOCARD                                              (0x2b)
// Mismatch between the NFCID3 initiator and the NFCID3 target in DEP
// 212/424 kbps passive.
#define PN532_ERROR_MISMATCH                                            (0x2c)
// An over-current event has been detected
#define PN532_ERROR_OVERCURRENT                                         (0x2d)
// NAD missing in DEP frame
#define PN532_ERROR_NONAD                                               (0x2e)
/* End: Official PN532 Errors Definitions */

//ADD my
//Send data to PN532 Error
#define PN532_ERROR_SEND_DATA                                               (0xe1)
//recv ACK from PN532 Error
#define PN532_ERROR_RECV_ACK   	                                            (0xe2)
//recv data from PN532 Error
#define PN532_ERROR_RECV_DATA                                               (0xe3)
//recv data from PN532 command error
#define PN532_ERROR_RECV_COMMAND                                            (0xe4)
//More than one card detected
#define PN532_ERROR_MORE_ONE_CARD											(0xe5)
//Found card with unexpectedly long UID!
#define PN532_ERROR_CARD_LONG_UID											(0xe6)
//not recv To host
#define PN532_ERROR_RECV_TOHOST												(0xe7)
//framing data len not good
#define PN532_ERROR_FRAMING_LEN												(0xe8)




//-------------------------------------------------------------------------
//The functions below work in blocking mode.
//All functions below return PN532_ERROR_NONE if there are no errors or the error codes described above.
//-------------------------------------------------------------------------

//Configures the SAM (Secure Access Module) (mode - see documentation, timeout - timeout in ms, use_irq - use IRQ pin)
uint8_t PN532_SetSamConfig(uint8_t mode, uint8_t timeout, uint8_t use_irq);
//Get the firmware version of the PN5xx chip
uint8_t PN532_GetFirmVersion(uint8_t * version);
//Reads the ID of the passive target the reader has deteceted. (uid buffer under the ID, uid_size its size. ID uid_len - read size)
uint8_t PN532_ReadPassTarget(uint8_t card_baud, uint8_t * uid, uint8_t uid_size, uint8_t *uid_len);
//Tries to authenticate a block of memory on a MIFARE card (key_number - MIFARE_CMD_AUTH_A or MIFARE_CMD_AUTH_B, key - secret key[6])
uint8_t PN532_MifareClassicAuthBlock(uint8_t *uid, uint8_t uid_len,  uint8_t block_number,  uint8_t key_number, uint8_t * key);
//Tries to read an entire 16-byte data block at the specified block address. (block_data read data[16])
uint8_t PN532_ReadDataBlock(uint8_t block_number, uint8_t * block_data);
//Tries to write an entire 16-byte data block at the specified block address. (block_data write data[16])
uint8_t PN532_WriteDataBlock(uint8_t block_number,  uint8_t * block_data);
//PN532_SetParameters (params - see documentation)
uint8_t PN532_SetParameters(uint8_t params);



//Ntag2xx authenticate (pwd - see documentation)
uint8_t PN532_Ntag2xxAuth(uint8_t * pwd, uint8_t* response);
//Tries to read an entire 4-byte page at the specified address.
uint8_t PN532_Ntag2xxReadBlock(uint8_t block_number, uint8_t * block_data);
//Tries to write an entire 4-byte page at the specified block address.
uint8_t PN532_Ntag2xxWriteBlock(uint8_t block_number, uint8_t * block_data);





//-------------------------------------------------------------------------
//The functions below can be used for non-blocking operations.
//-------------------------------------------------------------------------

//Configures the SAM (Secure Access Module) (mode - see documentation, timeout - timeout in ms, use_irq - use IRQ pin)
BOOL PN532_SendSamConfig(uint8_t mode, uint8_t timeout, uint8_t use_irq);
//Get the firmware version of the PN5xx chip
BOOL PN532_SendGetFirmwareVersion(void);
//Reads the ID of the passive target the reader has deteceted. (uid buffer under the ID, uid_size its size. ID uid_len - read size)
BOOL PN532_SendReadPassiveTarget(uint8_t card_baud);
//Tries to authenticate a block of memory on a MIFARE card (key_number - MIFARE_CMD_AUTH_A or MIFARE_CMD_AUTH_B, key - secret key[6])
BOOL PN532_SendMifareClassicAuthBlock(uint8_t *uid, uint8_t uid_len,  uint8_t block_number,  uint8_t key_number, uint8_t * key);
//Tries to read an entire 16-byte data block at the specified block address. (block_data read data[16])
BOOL PN532_SendReadDataBlock(uint8_t block_number);
//Tries to write an entire 16-byte data block at the specified block address. (block_data write data[16])
BOOL PN532_SendWriteDataBlock(uint8_t block_number,  uint8_t * block_data);
//PN532_SetParameters (params - see documentation)
BOOL PN532_SendSetParameters(uint8_t params);


//Ntag2xx authenticate (pwd - see documentation)
BOOL PN532_SendNtag2xxAuth(uint8_t * pwd);
//Tries to read an entire 4-byte page at the specified address.
BOOL PN532_SendNtag2xxReadBlock(uint8_t block_number);
//Tries to write an entire 4-byte page at the specified block address.
BOOL PN532_SendNtag2xxWriteBlock(uint8_t block_number, uint8_t * block_data);


//get PN532 recv buffer, returns a pointer to the receiving buffer
uint8_t * get_PN532_recv_buf(void);
//send data to PN532 with framing
BOOL PN532_send_data(uint8_t *data, uint8_t data_len);
//wait for byte PN532_I2C_READY
BOOL PN532_WaitReady(uint32_t timeout);
//get data from PN532, returns the number of bytes received
uint8_t PN532_recv(uint8_t * buf, uint8_t how);
//get ACK from PN532
BOOL PN532_recv_ack(void);
//parsing the PN532 frame, returns a pointer to data, and the number of data bytes data_len
uint8_t * PN532_parse_frame(uint8_t * buf, uint8_t buf_size, uint8_t * data_len);
//get data from PN532 and parse frame, returns a pointer to data, and the number of data bytes data_len
uint8_t * PN532_recv_data(uint8_t * buf, uint8_t buf_size, uint8_t * data_len);

#endif /* PN532_PN532_H_ */
