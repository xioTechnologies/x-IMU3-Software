//------------------------------------------------------------------------------
// Copyright 2011-2022 DigSol Sweden AB
//------------------------------------------------------------------------------
//  This code may only be used together with ds30 Loader boot loader firmwares
//  purchased from DigSol Sweden AB.
//------------------------------------------------------------------------------
#ifndef DS30_LOADER_H
#define DS30_LOADER_H


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "ds30_build_conf.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#if defined(_WIN32)
	#include <windows.h>
	#define DS30_INVALID_PORTFD INVALID_HANDLE_VALUE 
#else
	#define DS30_INVALID_PORTFD -1 
#endif


//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
#if defined(_WIN32)
	#include <windows.h>
	typedef HANDLE DS30PORTFD;
#else
	typedef int DS30PORTFD;
#endif

typedef enum {
	BL_NOT_SPECIFIED		= 0,
	BL_DS30_LOADER,
	BL_DS30_HEX_LOADER
} ds30_bl_type_t;


typedef enum {
	COMM_NOT_SPECIFIED		= 0,
	COMM_CUSTOM,
	COMM_I2C,
	COMM_KVASER,
	COMM_PCAN,
	COMM_SOCKET,
	COMM_SOCKETCAN,
	COMM_SPI,
	COMM_UART,
} ds30_comm_type_t;	


typedef enum {

	// Operations
    EV_OP_START				= 0,	//triggered as the first thing in ds30_write()

	EV_OP_PARSE_HEX_FILE,			//triggered after the hex file has been parased (result)
    EV_OP_PORT_OPEN_RESET,			//triggered after the port has been opened for sending reset command (result)
    EV_OP_PORT_OPEN,				//triggered after the port has been opened for boot loading, this will not occur if a reset command is used and the reset baud rate is the same as the boot loading baud rate (result)
    EV_OP_RESET_DEVICE,				//triggered after device reset (result)
	EV_OP_HELLO_RSP_RCV,			//triggered after hello response, before ok is received
    EV_OP_ESTABLISH_COMM,			//triggered after hello response and ok received, ready for boot loading (result)

	EV_OP_WRITE_FLASH,				//triggered after main flash is written (result)
    EV_OP_WRITE_AUX_FLASH,			//triggered after aux/boot flash is written (result)
    EV_OP_WRITE_EEPROM,				//triggered after eeprom is written (result)
    EV_OP_WRITE_CONFIGS,			//triggered after eeprom is written (result)
    EV_OP_WRITE_PAGE,				//triggered after a flash page has been written (result)

	EV_OP_READ_FLASH,				//triggered after the flash(es) has been read  (result)
    EV_OP_READ_PAGE,				//triggered after a flash page has been read (result)
	EV_OP_READ_EEPROM,				//triggered after a the eeprom has been read  (result)

    EV_OP_ENDED,					//triggered just before ds30_write() is about to exit (result)

	// Information
    EV_INFO_LIB,					//text information from the library, data = const char* const string
    EV_INFO_FW,						//text information from the firmware, data = const char* const string
	EV_INFO_PROGRESS_START,			//data = const uint32_t* const max_value
	EV_INFO_PROGRESS,				//data = const uint32_t* const value

	// Results
    EV_FAIL_COMM_TX,				//transmit failed
    EV_FAIL_COMM_TIMEOUT,			//receive timed out
    EV_FAIL_WRONG_UNIT_CONNECTED,	//unexpected device id receiveed
    EV_FAIL_INVALID_SETTINGS,		//one or more settings are not valid
    EV_FAIL_INCOMPATIBLE_HEX_FILE,	//encrypted/unencrypted hex file/boot loader mismatch or hex file contains data that would overwrite the boot loader, hex file not rectified
	EV_FAIL_FILE_READ,				//failed to read a line from the opened hex file
	EV_FAIL_BL_PROT,				//the hex file contains data that would overwrite the boot loader

	// Firmware responses
	EV_RSP_WRITE_VER_FAIL,			//write verification failed
	EV_RSP_COMM_CHECKSUM_FAIL,		//
	EV_RSP_OP_NOT_SUPPORTED,		//
    EV_RSP_BAD_PASSWORD,			//the correct password has not been received
	EV_RSP_BL_PROT_TRIP,			//
	EV_RSP_BAD_DATA_LEN,			//
	EV_RSP_BAD_MAC,					//the data failed the message authentication
	EV_RSP_UNKNOWN					//is not understood by the library, could be caused by a communication problem
} ds30_event_t;


// If ds30_main() is called from a separate thread, make sure the scope of the strings are sufficient
typedef struct {
	// Event callback
	void (*event_callback)( const void* const ds30_options, const ds30_event_t, const bool, const void* const );

	// This flag can be used to distinguish between multiple instances of ds30 Loader in the event handler
	uint32_t			user_flag;

	//											//ds30 Loader
	//											// ds30 HEX Loader
	// 											   
	//											//   UART
	//											//    SocketCAN (Linux)
	// 											    
	//											//      CAN (Windows)
	//											//       Socket
	//  											    
	//											//         I2C
	//											//          SPI
	// Basic
	ds30_bl_type_t		bl_type;				//xx xx xx xx 
	const char*			device_name;			//x- xx xx xx "PIC18F1320", "dsPIC33FJ128MC804"
	const char*			file_name;				//xx xx xx xx set to 0 if file_fd is used
    FILE*				file_fd;        		//xx xx xx xx set to 0 if file_name is used 

	// Optional
	bool				debug_mode;				//xx xx xx xx 

	// Communication
	ds30_comm_type_t	comm_type;				//xx xx xx xx 
	const char* 		port_name;				//xx xx xx xx set to 0 if port_fd is used, address for TCP Socket
	DS30PORTFD			port_fd;				//xx xx xx xx set to DS30_INVALID_PORTFD if port_name is used, ds30_write() will modify this when the port is opened and set back to DS30_INVALID_PORTFD when the port is closed

	// Communication UART
	uint32_t            baud_rate;				//xx x- x- --
    bool		    	auto_baud;				//xx x- -- --
    bool				echo_verification;		//xx x- -- --

	// Communication CAN
	uint32_t			remote_id;				//xx -x x- x-
	uint32_t			local_id;				//xx -x x- --
    bool				can_extended;			//xx -x x- --
    uint8_t				can_dlc;				//xx -x x- --
    bool				can_fd;					//xx -x x- --

	// Operation
	bool				write_flash;			//x- xx xx xx
	bool				write_aux_flash;		//x- xx xx xx
	bool				write_eeprom;			//x- xx xx xx
    bool    			write_configs;			//x- xx xx xx
	bool				read_flash;				//x- xx xx xx
	bool				read_aux_flash;			//x- xx xx xx
	bool				read_eeprom;			//x- xx xx xx
	bool				check_for_bl;			//x- xx xx xx

	// Advanced
	uint8_t				hello_cmd;				//x- xx xx xx set to 0 unless using a custom hello command (requires license)
	bool				dont_write_empty_pages;	//x- xx xx xx 
    uint32_t			bl_placement_p;			//x- xx xx xx used for older firmwares that do not send boot loader placement
    uint32_t			bl_size_p;				//x- xx xx xx used for older firmwares that do not send boot loader size
    bool				no_app_vector;			//x- xx xx xx 
	bool				no_bl_vector;			//x- xx xx xx 
    bool     			allow_bl_overwrite;		//x- xx xx xx 
    bool   				add_crc;				//x- xx xx xx 

	// Timing
	uint32_t			hello_timeout;			//x- xx xx xx  
	uint32_t			polltime;				//x- xx xx xx
    uint32_t			timeout;				//xx xx xx xx
    uint32_t			port_open_delay;		//xx xx xx xx
	uint32_t			cmd_delay;				//-x xx xx xx 

	// Device reset
	uint32_t			reset_time;				//xx xx xx xx 
	const char* 		reset_command;			//xx xx xx xx 
	uint32_t			reset_baud_rate;		//xx x- x- x-
	uint8_t				reset_can_dlc;			//xx -x x- --
	bool 				reset_can_extended;		//xx -x x- --
	uint32_t			reset_id;				//xx -x x- x-
	bool				reset_dtr;				//xx x- -- --
	bool				reset_rts;				//xx x- -- -- 

	// Device activation
	bool				activate_dtr;			//xx x- -- -- 
	bool				activate_rts;			//xx x- -- -- 

	// Security
    const char* 		password;				//xx xx xx xx
} ds30_options_t;


//------------------------------------------------------------------------------
// Function prototypes
// Return values: 0=success, else fail
//------------------------------------------------------------------------------
void ds30_init(void);
void ds30_print_license(void);
void ds30_set_defaults( ds30_options_t* const ds30_options );
int ds30_write( ds30_options_t* const ds30_options );


//------------------------------------------------------------------------------
// End of file
//------------------------------------------------------------------------------
#endif //DS30_LOADER_H
