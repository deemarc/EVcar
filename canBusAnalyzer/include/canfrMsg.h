/**
 * @file canfrMsg.h
 * @brief  contains macros(#define), enums, and data strutures use by many modules
 */
#ifndef CANFRMSG_H
#define CANFRMSG_H

#include <canfr_typedef.h>
#include <native/queue.h>
#define CB_FR_RX_EVENT_HEADER_SIZE  	32 
#define CB_FR_MAX_EVENT_SIZE            512  
#define XL_FR_MAX_DATA_LENGTH         	254

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// T_FLEXRAY_FRAME structure flags / defines 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// defines for T_FLEXRAY_FRAME member flags 
#define CB_FR_FRAMEFLAG_STARTUP                   ((unsigned short) 0x0001)
#define CB_FR_FRAMEFLAG_SYNC                      ((unsigned short) 0x0002)
#define CB_FR_FRAMEFLAG_NULLFRAME                 ((unsigned short) 0x0004)
#define CB_FR_FRAMEFLAG_PAYLOAD_PREAMBLE          ((unsigned short) 0x0008)
#define CB_FR_FRAMEFLAG_FR_RESERVED               ((unsigned short) 0x0010)
        
#define CB_FR_FRAMEFLAG_REQ_TXACK                 ((unsigned short) 0x0020)   //!< used for Tx events only 
#define CB_FR_FRAMEFLAG_TXACK_SS                  XL_FR_FRAMEFLAG_REQ_TXACK   //!< indicates TxAck of SingleShot; used for TxAck events only 
#define CB_FR_FRAMEFLAG_RX_UNEXPECTED             XL_FR_FRAMEFLAG_REQ_TXACK   //!< indicates unexpected Rx frame; used for Rx events only 
        
#define CB_FR_FRAMEFLAG_NEW_DATA_TX               ((unsigned short) 0x0040)   //!< flag used with TxAcks to indicate first TxAck after data update 
#define CB_FR_FRAMEFLAG_DATA_UPDATE_LOST          ((unsigned short) 0x0080)   //!< flag used with TxAcks indicating that data update has been lost 
        
#define CB_FR_FRAMEFLAG_SYNTAX_ERROR              ((unsigned short) 0x0200) 
#define CB_FR_FRAMEFLAG_CONTENT_ERROR             ((unsigned short) 0x0400) 
#define CB_FR_FRAMEFLAG_SLOT_BOUNDARY_VIOLATION   ((unsigned short) 0x0800) 
#define CB_FR_FRAMEFLAG_TX_CONFLICT               ((unsigned short) 0x1000) 
#define CB_FR_FRAMEFLAG_EMPTY_SLOT                ((unsigned short) 0x2000) 
#define CB_FR_FRAMEFLAG_FRAME_TRANSMITTED         ((unsigned short) 0x8000) 

/**
 * @brief      data stucture for tcp recived packet
 * @anchor canfrControlFrameStruct_t
 */
typedef struct 
{
   char cmd;			///< storing canfrCmd
   char argSize;	///< actual size in byte of the followed parameter
} canfrControlFrameStruct_t;

typedef enum 
{
	CB_BUS_TYPE_NONE = 0,  
	CB_BUS_TYPE_CAN = 1,
	CB_BUS_TYPE_FLEXRAY = 4
} CBBusTypes;

/**
 * @brief      data structure for CAN Message to be sending out with required parameters
 * @anchor canSend_struc
 */
typedef struct 
{
  unsigned long can_id; ///< CAN Bus message ID
  /**
   * @brief      how many time should it be sent
   * - loops = 0, sent continuly until it receive command to stop it
   * - loops = 1, send single time
   * - loops > 1, send out loops time
   */
  unsigned long loops;
  unsigned long delay; ///< period of message
  unsigned char can_ifindex;///< which can channel should it be sent out
  /**
   * - Boolean rtr, send remote request[bit 0]
   * - Boolean extended, send extended frame[bit 1]
   */
  unsigned char msgFlag;///< CAN bus message flag
  unsigned char can_dlc;///< CAN bus message length(0-8)
  unsigned char reserveByte;///< as the name said
  unsigned char data[8];///< its data field
} canSend_struc;

enum canfrCmd
{
	DATA_RAW=0,
	CAN_START,
	CAN_STOP,
	CAN_SEND,
	CAN_RECV,
	CAN_LOG,
	CMD_SUCCESS = 60000,
	CMD_FAIL = 61111

};

enum commands
{
	CB_OPEN_DRIVER,
	CB_CLOSE_DRIVER,
	CB_OPEN_CAN_PORT,
	CB_OPEN_FR_PORT,	
	CB_CLOSE_PORT,
	CB_GET_DRIVER_CONFIG,
	CB_GET_APPLICATION_CONFIG,
	CB_GET_CHANNEL_INDEX,
	CB_GET_CHANNEL_MASK,
	CB_SET_NOIFICATION,
	CB_ACTIVATE_CHANNEL,
	CB_DEACTIVATE_CHANNEL,
	CB_ACTIVATE_CLOCK,
	CB_FLUSH_TRANSMIT_QUEUE,
	CB_REQUEST_CHIP_STATE,
	CB_CAN_TRANSMIT,
	CB_FR_TRANSMIT,
	CB_GET_RX_QUEUE_LEVEL,
	CB_FLUSH_RX_QUEUE,
	CB_RECEIVE_CAN,
	CB_RECEIVE_FR,	
	CB_FR_STARTUP_AND_SYNC,
	CB_FR_SETCONFIG,
	CB_FR_SET_MODE,
	CB_FR_ACTIVATE_SPY,
	CB_GET_EVENT_STRING,
	CB_SET_CHANNEL_BIT_RATE,
	CB_MAX_NUM_COMMANDS
};

enum responseCodes 
{
	CB_SUCCESS = 0,
	CB_PENDING = 1,
	CB_ERR_QUEUE_IS_EMPTY = 10,
	CB_ERR_QUEUE_IS_FULL = 11,
	CB_ERR_TX_NOT_POSSIBLE = 12,
	CB_ERR_NO_LICENSE = 14,
	CB_ERR_WRONG_PARAMETER = 101,
	CB_ERR_TWICE_REGISTER = 110,
	CB_ERR_INVALID_CHAN_INDEX = 111,
	CB_ERR_INVALID_ACCESS = 112,
	CB_ERR_PORT_IS_OFFLINE = 113,
	CB_ERR_CHAN_IS_ONLINE = 116,
	CB_ERR_NOT_IMPLEMENTED = 117,
	CB_ERR_INVALID_PORT = 118,
	CB_ERR_HW_NOT_READY = 120,
	CB_ERR_CMD_TIMEOUT = 121,
	CB_ERR_HW_NOT_PRESENT = 129,
	CB_ERR_NOTIFY_ALREADY_ACTIVE = 131,
	CB_ERR_NO_RESOURCES = 152,
	CB_ERR_WRONG_CHIP_TYPE = 153,
	CB_ERR_WRONG_COMMAND = 154,
	CB_ERR_INVALID_HANDLE = 155,
	CB_ERR_RESERVED_NOT_ZERO = 157,
	CB_ERR_INIT_ACCESS_MISSING = 158,
	CB_ERR_CANNOT_OPEN_DRIVER = 201,
	CB_ERR_WRONG_BUS_TYPE = 202,
	CB_ERR_DLL_NOT_FOUND = 203,
	CB_ERR_INVALID_CHANNEL_MASK = 204,
	CB_ERR_NOT_SUPPORTED = 205,
	CB_ERR_CONNECTION_BROKEN = 210,
	CB_ERR_CONNECTION_CLOSED = 211,
	CB_ERR_INVALID_STREAM_NAME = 212,
	CB_ERR_CONNECTION_FAILED = 213,
	CB_ERR_STREAM_NOT_FOUND = 214,
	CB_ERR_STREAM_NOT_CONNECTED = 215,
	CB_ERR_QUEUE_OVERRUN = 216,
	CB_ERROR = 255,
}responseCodes;

/* Structure of this type contains configuration
   information of the one low level parameters set */
typedef struct
{
    UINT8   G_COLD_START_ATTEMPTS;
    UINT8   GD_ACTION_POINT_OFFSET;
    UINT8   GD_CAS_RX_LOW_MAX;
    UINT8   GD_DYNAMIC_SLOT_IDLE_PHASE;
    UINT8   GD_MINISLOT;
    UINT8   GD_MINI_SLOT_ACTION_POINT_OFFSET;
    UINT16  GD_STATIC_SLOT;
    UINT8   GD_SYMBOL_WINDOW;
    UINT8   GD_TSS_TRANSMITTER;
    UINT8   GD_WAKEUP_SYMBOL_RX_IDLE;
    UINT8   GD_WAKEUP_SYMBOL_RX_LOW;
    UINT16  GD_WAKEUP_SYMBOL_RX_WINDOW;
    UINT8   GD_WAKEUP_SYMBOL_TX_IDLE;
    UINT8   GD_WAKEUP_SYMBOL_TX_LOW;
    UINT8   G_LISTEN_NOISE;
    UINT16  G_MACRO_PER_CYCLE;
    UINT8   G_MAX_WITHOUT_CLOCK_CORRECTION_PASSIVE;
    UINT8   G_MAX_WITHOUT_CLOCK_CORRECTION_FATAL;
    UINT16  G_NUMBER_OF_MINISLOTS;
    UINT16  G_NUMBER_OF_STATIC_SLOTS;
    UINT16  G_OFFSET_CORRECTION_START;
    UINT8   G_PAYLOAD_LENGTH_STATIC;
    UINT8   G_SYNC_NODE_MAX;
    UINT8   G_NETWORK_MANAGEMENT_VECTOR_LENGTH;
    boolean P_ALLOW_HALT_DUE_TO_CLOCK;
    UINT8   P_ALLOW_PASSIVE_TO_ACTIVE;
    Fr_channel_type  P_CHANNELS;
    UINT16  PD_ACCEPTED_STARTUP_RANGE;
    UINT8   P_CLUSTER_DRIFT_DAMPING;
    UINT8   P_DECODING_CORRECTION;
    UINT8   P_DELAY_COMPENSATION_CHA;
    UINT8   P_DELAY_COMPENSATION_CHB;
    UINT32  PD_LISTEN_TIMEOUT;
    UINT16  PD_MAX_DRIFT;
    UINT8   P_EXTERN_OFFSET_CORRECTION;
    UINT8   P_EXTERN_RATE_CORRECTION;
    UINT16  P_KEY_SLOT_ID;
    boolean P_KEY_SLOT_USED_FOR_STARTUP;
    boolean P_KEY_SLOT_USED_FOR_SYNC;
    UINT16  P_KEY_SLOT_HEADER_CRC;
    UINT16  P_LATEST_TX;
    UINT8   P_MACRO_INITIAL_OFFSET_A;
    UINT8   P_MACRO_INITIAL_OFFSET_B;
    UINT8   P_MICRO_INITIAL_OFFSET_A;
    UINT8   P_MICRO_INITIAL_OFFSET_B;
    UINT32  P_MICRO_PER_CYCLE;
    UINT16  P_OFFSET_CORRECTION_OUT;
    UINT16  P_RATE_CORRECTION_OUT;
    boolean P_SINGLE_SLOT_ENABLED;
    Fr_channel_type  P_WAKEUP_CHANNEL;
    UINT8   P_WAKEUP_PATTERN;
    UINT8   P_MICRO_PER_MACRO_NOM;
    UINT8   P_PAYLOAD_LENGTH_DYN_MAX;
} Fr_configuration_type;

extern Fr_configuration_type fr_config;

typedef struct
{
	UINT8 busItf;
	UINT32 baudRate;
} Canfr_setBitParam;

typedef struct 
{
	UINT8 busItf;
	UINT8 flags;
}Canfr_setModeParam;

typedef enum 
{
    FR_ERROR_POC_MODE = 1,
    FR_ERROR_SYNC_FRAMES_BELOWMIN = 2,
    FR_ERROR_SYNC_FRAMES_OVERLOAD = 3,
    FR_ERROR_CLOCK_CORR_FAILURE = 4,
    FR_ERROR_NIT_FAILURE = 5,
    FR_ERROR_CC_ERROR = 6,
    FR_START_CYCLE = 128,
    FR_RX_FRAME = 129,
    FR_TX_FRAME = 130,
    FR_TXACK_FRAME = 131,
    FR_INVALID_FRAME = 132,
    FR_WAKEUP = 133,
    FR_SYMBOL_WINDOW = 134,
    FR_ERROR = 135,
    FR_STATUS = 136,
    FR_NM_VECTOR = 138,
    FR_TRANCEIVER_STATUS = 139,
    FR_SPY_FRAME = 142,
    FR_SPY_SYMBOL = 143,
}CBfrEventTag;

enum CBfrEventSize
{ 
    FR_RX_EVENT_HEADER_SIZE = 32,
    FR_MAX_DATA_LENGTH = 254,
    FR_MAX_EVENT_SIZE = 512,
};

enum CBfrFlagsChip
{ 
	FR_CHANNEL_A = 1,
    FR_CHANNEL_B = 2,
    FR_CHANNEL_AB = 3,
    FR_CC_COLD_A = 4,
    FR_CC_COLD_B = 8,
    FR_CC_COLD_AB = 12,
    FR_SPY_CHANNEL_A = 16,
    FR_SPY_CHANNEL_B = 32,
    FR_QUEUE_OVERFLOW = 256,
};

enum CBfrErrorPocMode
{ 
    FR_ERROR_POC_ACTIVE = 0,
	FR_ERROR_POC_PASSIVE = 1,
	FR_ERROR_POC_COMM_HALT = 2,
};

enum fr_flameflags
{
    FR_FRAMEFLAG_STARTUP = 1,
    FR_FRAMEFLAG_SYNC = 2,
    FR_FRAMEFLAG_NULLFRAME = 4,
    FR_FRAMEFLAG_PAYLOAD_PREAMBLE = 8,
    FR_FRAMEFLAG_FR_RESERVED = 16,
    FR_FRAMEFLAG_TXACK_SS = 32,
    FR_FRAMEFLAG_RX_UNEXPECTED = 32,
    FR_FRAMEFLAG_REQ_TXACK = 32,
    FR_FRAMEFLAG_NEW_DATA_TX = 64,
    FR_FRAMEFLAG_DATA_UPDATE_LOST = 128,
    FR_FRAMEFLAG_SYNTAX_ERROR = 512,
    FR_FRAMEFLAG_CONTENT_ERROR = 1024,
    FR_FRAMEFLAG_SLOT_BOUNDARY_VIOLATION = 2048,
    FR_FRAMEFLAG_TX_CONFLICT = 4096,
    FR_FRAMEFLAG_EMPTY_SLOT = 8192,
    FR_FRAMEFLAG_FRAME_TRANSMITTED = 32768,
};

enum CBFrMode
{
	FR_MODE_NORMAL = 0,
	FR_MODE_COLD_NORMAL
};

enum CBFrStartupAttributes
{
	FR_MODE_NONE = 0,
	FR_MODE_WAKEUP,
	FR_MODE_COLDSTART_LEADING,
	FR_MODE_COLDSTART_FOLLOWING,
	FR_MODE_WAKEUP_COLDSTART_LEADING,
	FR_MODE_WAKEUP_COLDSTART_FOLLOWING
};


typedef struct s_cb_fr_status {
  unsigned int                          statusType;              
  unsigned int                          reserved;
} CB_FR_STATUS_EV;

typedef struct s_cb_fr_start_cycle {
  unsigned int                          cycleCount;
  int                                   vRateCorrection;
  int                                   vOffsetCorrection;
  unsigned int                          vClockCorrectionFailed;
  unsigned int                          vAllowPassivToActive;
  unsigned int                          reserved[3]; 
} CB_FR_START_CYCLE_EV;

typedef struct s_cb_fr_rx_frame {
  unsigned short                        flags;
  unsigned short                        headerCRC; 
  unsigned short                        slotID;
  unsigned char                         cycleCount;
  unsigned char                         payloadLength; 
  unsigned char	                        data[XL_FR_MAX_DATA_LENGTH]; 
} CB_FR_RX_FRAME_EV;

typedef struct s_cb_fr_tx_frame {
  unsigned short                        flags;
  unsigned short                        slotID;
  unsigned char                         offset; 
  unsigned char	                        repetition;
  unsigned char                         payloadLength;
  unsigned char	                        txMode;
  unsigned char                         incrementSize;
  unsigned char                         incrementOffset;
  unsigned char                         reserved0;
  unsigned char                         reserved1;
  unsigned char	                        data[XL_FR_MAX_DATA_LENGTH]; 
} CB_FR_TX_FRAME_EV;

typedef struct s_cb_fr_wakeup { 
  unsigned char                         cycleCount;              
  unsigned char                         wakeupStatus;            
  unsigned char                         reserved[6];
} CB_FR_WAKEUP_EV;

typedef struct s_cb_fr_symbol_window {
  unsigned int                          symbol;                  
  unsigned int                          flags;                   
  unsigned char                         cycleCount;             
  unsigned char                         reserved[7];
} CB_FR_SYMBOL_WINDOW_EV;

typedef struct s_cb_fr_nm_vector {
  unsigned char                         nmVector[12];
  unsigned char                         cycleCount;              
  unsigned char                         reserved[3];
} CB_FR_NM_VECTOR_EV;

typedef struct s_cb_fr_sync_pulse {
  unsigned int                          triggerSource;           
} CB_FR_SYNC_PULSE_EV;

typedef struct s_cb_fr_error {
  unsigned char                         tag;
  unsigned char                         cycleCount;
  unsigned char                         reserved[6];
} CB_FR_ERROR_EV;

typedef struct s_cb_fr_spy_frame {
  unsigned int                          frameLength;
  unsigned char                         frameError;	             
  unsigned char                         tssLength;	
  unsigned short                        headerFlags;	 
  unsigned short                        slotID;
  unsigned short                        headerCRC;
  unsigned char                         payloadLength; 
  unsigned char                         cycleCount;
  unsigned short                        reserved;
  unsigned int                          frameCRC;  
  unsigned char                         data[XL_FR_MAX_DATA_LENGTH];
} CB_FR_SPY_FRAME_EV;

typedef struct s_cb_fr_spy_symbol {
  unsigned short                        lowLength;
  unsigned short                        reserved;
 } CB_FR_SPY_SYMBOL_EV;
 
typedef struct s_cb_application_notification { 
  unsigned int 	notifyReason; 			
  unsigned int	reserved[7]; 
} CB_APPLICATION_NOTIFICATION_EV; 

typedef struct   
{
  CB_FR_STATUS_EV        	frStatus;					//POC status
  CB_FR_NM_VECTOR_EV  		frNmVector;  
  CB_FR_RX_FRAME_EV      	frRxFrame;   
  CB_FR_START_CYCLE_EV   	frStartCycle;
  CB_FR_RX_FRAME_EV      	frTxAckFrame;   
  CB_FR_TX_FRAME_EV     	frTxFrame;
}s_cb_fr_tag_data;

//-------- Event Struct for Flexray Message
typedef struct 
{
	unsigned short 		channelIndex; 
	CBuint64 			timeStamp; 
	CBuint64 			timeStampSync; 
	unsigned int 		userHandle; 
	unsigned int 		size; 
	CBfrEventTag 		tag; 
	unsigned short 		flagsChip; 
	s_cb_fr_tag_data	tagData; 
}CBfrEvent;

/**
 * @anchor busInterface
 * @brief type def enum for CAN bus channel
 * - ITF_RTCAN0, indicate CAN bus channel 1
 * - ITF_RTCAN1, indicate CAN bus channel 2
 * - ITF_RTCANALL, indicate CAN bus both channel
 */
typedef enum 
{
	ITF_RTCAN0 = 1,
	ITF_RTCAN1,
	ITF_RTCANALL
}busInterface;

/*enum canBusMode
{
	CAN_MODE_STOP =0,
	CAN_MODE_START,
	CAN_MODE_SLEEP

};*/





#endif
