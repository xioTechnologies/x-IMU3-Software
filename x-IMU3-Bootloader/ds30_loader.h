//-----------------------------------------------------------------------------
// Copyright 2011-2021 DigSol Sweden AB
//-----------------------------------------------------------------------------
//  This code may only be used together with ds30 Loader boot loader firmwares
//  purchased from DigSol Sweden AB.
//-----------------------------------------------------------------------------
#ifndef DS30_LOADER_H
#define DS30_LOADER_H


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ds30_build_conf.h"


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#if defined(_WIN32)
	#include <windows.h>
	typedef HANDLE DS30PORTFD;
#else
	typedef int DS30PORTFD;
#endif

typedef enum {
	DS30_COMM_NOT_SPECIFIED = 0,
	DS30_UART,
	DS30_SOCKETCAN,
	DS30_SOCKET,
	DS30_I2C,
	DS30_PCAN,
	DS30_KVASER,
} ds30_comm_type_t;


typedef enum {

	// Operations
    EV_OP_START = 0,				//triggered as the first thing in ds30_write()

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
    EV_COMM_TX_FAIL,				//transmit failed
    EV_COMM_TIMEOUT,				//receive timed out
    EV_WRONG_UNIT_CONNECTED,		//unexpected device id receiveed
    EV_INVALID_SETTINGS,			//one or more settings are not valid
    EV_INCOMPATIBLE_HEX_FILE,		//encrypted/unencrypted hex file/boot loader mismatch or hex file contains data that would overwrite the boot loader

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

	// Required
	const char*			file_name;
	const char* 		port_name;				//set to 0 if port_fd is used
	DS30PORTFD			port_fd;				//set to DS30_INVALID_PORTFD if port_name is used, ds30_write() will modify this when the port is opened and set back to -1 when the port is closed
	uint32_t            baud_rate;
	const char*			device_name;

	// Misc
	bool				debug_mode;
	ds30_comm_type_t	comm_type;
	uint8_t				hello_cmd;				//set to 0 unless using a custom hello command (requires license)

	// Operation
	bool				write_flash;
	bool				write_aux_flash;
	bool				write_eeprom;
    bool    			write_configs;
	bool				read_flash;
	bool				read_aux_flash;
	bool				read_eeprom;
	bool				check_for_bl;

	// CAN
	uint32_t			remote_id;
	uint32_t			local_id;
    bool				can_extended;
    uint8_t				can_dlc;
    bool				can_fd;

	// Advanced
    bool     			allow_bl_overwrite;
    bool				no_app_vector;
    uint32_t			bl_placement_p;			//used for older firmwares that do not send boot loader placement
    uint32_t			bl_size_p;				//used for older firmwares that do not send boot loader size
    bool		    	auto_baud;
    uint32_t			echo_verification;
    bool   				add_crc;
	bool				dont_write_empty_pages;
	bool				no_bl_vector;

	// Timing
	uint32_t			hello_timeout;
	uint32_t			polltime;
    uint32_t			timeout;
    uint32_t			port_open_delay;

	// Device reset
	const char* 		reset_command;
	uint32_t			reset_baud_rate;
	bool				reset_dtr;
	bool				reset_rts;
	uint32_t			reset_time;
	uint32_t			reset_id;
	uint8_t				reset_can_dlc;
	bool 				reset_can_extended;

	// Security
    const char* 		password;
} ds30_options_t;


//-----------------------------------------------------------------------------
// ds30 Loader event protocol
//-----------------------------------------------------------------------------
#if defined(BUILD_COCOA) && defined(__OBJC__)
#import <Foundation/Foundation.h>
@protocol ds30EventHandlerProt <NSObject>
- (void)ds30EventHandler:(const void* const) ds30_options : (const ds30_event_t)event :(const BOOL)success : (const void* const)data;
@end
#endif


//-----------------------------------------------------------------------------
// Function prototypes
// Return values: 0=success, else fail
//-----------------------------------------------------------------------------
void ds30_init(void);
void ds30_print_license(void);
void ds30_set_defaults( ds30_options_t* const ds30_options );
int ds30_write( ds30_options_t* const ds30_options );

#if defined(BUILD_COCOA) && defined(__OBJC__)
	void ds30_set_event_callback_obj( id <ds30EventHandlerProt> new_event_callback_obj );
#else
	void ds30_set_event_callback( void (*)( const ds30_options_t* const ds30_options, const ds30_event_t, const bool, const void* const ) );
#endif


//-----------------------------------------------------------------------------
// End of file
//-----------------------------------------------------------------------------
#endif //DS30_LOADER_H
