#ifndef __QDI_OEM_H__
#define __QDI_OEM_H__
/*===========================================================================*/
/** @file qdi_oem.h

DESCRIPTION
  
OEM Header definition file for platform customization
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The OEM_Panel_mainpage.dox file contains the group/module descriptions
      that are displayed in the output PDF generated using Doxygen and LaTeX.
      To edit or update any of the group/module text in the PDF, edit the
      OEM_Panel_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
Copyright (c) 2012-2018 Qualcomm Technologies, Inc.
All rights reserved.
===========================================================================*/

/*=============================================================================
                             Edit History

when        who     what, where, why
--------   ---     ------------------------------------------------------------
05/16/16   DWM     (Tech Pubs) Edited new Doxygen comments and markup.
03/09/16   DWM     (Tech Pubs) Edited new Doxygen comments and markup.
01/15/15   DWM     (Tech Pubs) Edited new Doxygen comments and markup.
05/03/13   DWM     (Tech Pubs) Edited new Doxygen comments and markup.
06/14/12   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/

#include "qdi_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @cond internal_only */
// QDI OEM VERSION
/** @addtogroup versions
@{ */

/** Major revision number; part of the QDI panel API version number. */
#define QDI_PANEL_API_MAJOR_REVISION                          0x00000002

/** Minor revision number; part of the QDI panel API version number. */
#define QDI_PANEL_API_MINOR_REVISION                          0x00000001

/** Sub-revision number; part of the QDI panel API version number. */
#define QDI_PANEL_API_SUB_REVISION                            0x0000006A

/** QDI panel API version number. */
#define QDI_PANEL_API_VERSION                                 (QDI_PANEL_API_MAJOR_REVISION << 24 | QDI_PANEL_API_MINOR_REVISION << 16 | QDI_PANEL_API_SUB_REVISION)

/* Extracting major revision number from the QDI panel API version number*/
#define QDI_PANEL_API_GET_MAJOR_REVISION(_version_)           ((_version_ >> 24) & 0xFF) 

/* Extracting minor revision number from the QDI panel API version number*/
#define QDI_PANEL_API_GET_MINOR_REVISION(_version_)           ((_version_ >> 16) & 0xFF)

/* Extracting sub revision number from the QDI panel API version number*/
#define QDI_PANEL_API_GET_SUB_REVISION(_version_)             (_version_ & 0xFFFF)

/** @} */ /* end_addtogroup versions */
/** @endcond */

/** @cond internal_only */
/*===========================================================================

                        Defines and Structs

===========================================================================*/
/** @addtogroup constants_macros
@{ */

/** @name Definitions for the Color Correction Matrices
@{ */
/** Number of color correction matrix components. */
#define QDI_PANEL_CC_MATRIX_COMPONENTS                        3

/** Number of color correction matrix coefficients. */
#define QDI_PANEL_CC_MATRIX_COEFFICIENTS                      11

/** Maximum number of gamma correction components. */
#define QDI_PANEL_GC_MAX_COMPONENT_ENTRIES                    256

/** Number of panel gamma correction components. */
#define QDI_PANEL_PGC_COMPONENTS                              3

/** Number of panel gamma correction segments. */
#define QDI_PANEL_PGC_SEGMENTS                                16

/** Length of the panel name. */
#define QDI_PANEL_NAME_LENGTH                                 13

/** Number of panel gamut mapping components. */
#define QDI_PANEL_PGM_COMPONENTS                              3

/** Number of entries in six-zone color adjustment LUT. */
#define QDI_PANEL_SIX_ZONE_LUT_MAX_ENTRIES                    384

/** Length of the calibration arrays for ambient enhancement. */
#define QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET1_LEN     33

/** Length of the calibration arrays for ambient enhancement. */
#define QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET2_LEN     32

/** Number of dithering matrix components. */
#define QDI_PANEL_DITHER_MATRIX_ROWS                          4

/** Number of dithering color components. */
#define QDI_PANEL_DITHER_BITDEPTH                             3

/** Max number of entries in a response table. */
#define QDI_PANEL_RESPONSE_TABLE_MAX_LEN                      256

/** @} */ /* end_name Definitions for the Color Correction Matrices */

/** @name Definition for the histogram setting
@{ */
/** Number of histogram data bins. */
#define QDI_PANEL_HISTOGRAM_BINS                              256

/** @} */ /* end_name Definition for the histogram setting */

/** @name Definition for the GUID
@{ */

/** @} */ /* end_name Definition for the GUID */

/** @name Definition for the Lane Descriptor String
@{ */
/** Length of the string that contains the data extracted from LVDSLane0-3
    from the panel configuration file.
  
    See QDI_Panel_LVDSAttrType for the string format. This reserves
    enough room for the maximum string supported, which has a format like
    this example: G0 R5 R4 R3 R2 R1 R0.
*/

/** @} */ /* end_name Definition for the Lane Descriptor String */

/** @name Definition for maximum number of refresh rates used by the panel
@{ */
/** Specifies the maximum number of dynamic refresh rates that can be supported
    by QDI.
*/
#define QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES            16

/** @} */ /* end_name Definition for maximum number of refresh rates used by the panel */

/** @name Definition for the Ambient Light Enhancement Backlight Scale Flag
@{ */
/** Defines the scale that can be used in the BacklightLevel passed through the
    QDI_DisplayUtil_SetProperty() function. If this flag is passed along with 
    the Backlight Level, the scale can be 0 through BacklightScale. If this flag
    is not set, the scale used must be from 0 through 100 (which the driver
    internally adjusts 0 to 0xFFFF).
*/
#define QDI_OEM_ALENHANCEMENT_BACKLIGHT_SCALE_FLAG            0x80000000

/** @} */ /* end_name Definition for the AL Enhancement Backlight Scale Flag */

/* QDI DSI read/write flag for the QDI_DisplayUtil_ReadCmdBuf() and
   QDI_DisplayUtil_SendCmdBuf() functions. */

/** @name QDI DSI Read/Write Flags
@{ */
/** Flag that returns a complete packet, including the headers. */
#define QDI_DSI_CMD_FLAG_READ_RAW_PACKET                      0x00000001

/** Flag that forces low-power command packets. */
#define QDI_DSI_CMD_FLAG_DMA_FORCE_LP                         0x00000010
 
/** Flag that forces high-speed command packets. */
#define QDI_DSI_CMD_FLAG_DMA_FORCE_HS                         0x00000020

/** Flag that overrides the default polling-based approach and uses event-based approach to transfer DSI command packets */
#define QDI_DSI_CMD_FLAG_DMA_EVENT_BASED_TRIGGER       	      0x00000040

/** Flag to indicate scheduling of DSI command packets */
#define QDI_DSI_CMD_FLAG_DMA_SCHEDULE_COMMAND_TRIGGER       	0x00000080

/** Queues DSI write packets.

    If this flag is set when calling the DSI write function, the DSI write
    packets queue internally without being sent to the panel. The final
    DSI write call without this flag triggers hardware to send all queued 
    packets, including the final write packet.
    
    Use this flag for both Video mode and Command mode.
*/
#define QDI_DSI_CMD_FLAG_QUEUE_WRITE_PACKET                   0x00000100

/** Indicates that DSI commands using controller 0 for dual DSI case. */
#define QDI_DSI_CMD_FLAG_DUAL_DSI_CONTROLLER_0                0x00001000

/** Indicates that DSI commands using controller 1 for dual DSI case. */
#define QDI_DSI_CMD_FLAG_DUAL_DSI_CONTROLLER_1                0x00002000

/** Indicates that DSI read will not check the return packet ECC value. */
#define QDI_DSI_CMD_FLAG_READ_IGNORE_ECC_CHECK                0x00010000

/** Indicates that DSI read will ignore error ACK packet. */
#define QDI_DSI_CMD_FLAG_READ_IGNORE_ERROR_ACK                0x00020000

/** @} */ /* end_name QDI DSI Read/Write Flags */

/** @name QDI HDMI flags for QDI_Panel_HotPlugEventDataType
@{ */
/** Flag to indicate that HDCP is not supported by sink. */
#define QDI_HDMI_FLAG_HDCP_NOT_SUPPORTED                      0x00000001

/** @} */ /* end_name QDI HDMI flags for QDI_Panel_HotPlugEventDataType */

/** @name Definition for maximum brightness ranges supported by the panel
@{ */
/** Specifies the maximum number of brightness ranges that can be supported
    by QDI.
*/
#define QDI_PANEL_MAX_NUMBER_BRIGHTNESS_RANGE_LEVEL           10

/** @} */ /* end_name Definition for maximum maximum brightness ranges supported by the panel */
/** @} */ /* end_addtogroup constants_macros */
/** @endcond */

/** @addtogroup data_types_qdi
@{ */
/* -----------------------------------------------------------------------
 * Shared Enumeration Types
 * ----------------------------------------------------------------------- */

/** Defines the supported interfaces.
 */
typedef enum
{
  QDI_PANEL_INTERFACE_NONE = 0,       /**< None, or an unknown interface. */
  QDI_PANEL_INTERFACE_ACTIVE,         /**< Dumb panels, active refresh panel,
                                           video mode panel. */
  QDI_PANEL_INTERFACE_COMMAND,        /**< Smart panels, command mode panel
                                           (DSI command). */
  QDI_PANEL_INTERFACE_MAX,            /**< Enumeration marker for the max. */
  QDI_PANEL_INTERFACE_FORCE_32BIT = 0x7FFFFFFF /**< Forces the enumeration to
                                                    32 bits. */
} QDI_Panel_InterfaceModeType;


/** Defines the OEM backlight configuration options.
*/
typedef enum
{
  QDI_PANEL_BACKLIGHTTYPE_NONE = 0, /**< No backlight adjustment is available. */
  QDI_PANEL_BACKLIGHTTYPE_PMIC,     /**< Backlight is controlled through PMIC
                                         PWM or WLED. */
  QDI_PANEL_BACKLIGHTTYPE_I2C,      /**< Backlight is controlled through I2C. */
  QDI_PANEL_BACKLIGHTTYPE_DSI,      /**< Backlight is controlled through the DSI
                                         command. */
  QDI_PANEL_BACKLIGHTTYPE_ACPI,     /**< Backlight is controlled through the
                                         ACPI method. */  
  QDI_PANEL_BACKLIGHTTYPE_MAX,      /**< Enumeration marker for the max. */
  QDI_PANEL_BACKLIGHTTYPE_FORCE_32BIT = 0x7FFFFFFF /**< Forces the enumeration
                                                        to 32 bits. */
} QDI_Panel_BacklightType;


/** Defines the power saving configuration information that the OEM is allowed to override.
*/
typedef enum
{
  QDI_PANEL_POWERSAVING_DEFAULT                 = 0x00000000, 
  /**< RESERVED */    	
  QDI_PANEL_POWERSAVING_RESERVED1               = 0x00000001, 
  /**< RESERVED */  
  QDI_PANEL_POWERSAVING_INFO_LEVEL0             = 0x00000002,
  /**< Allows only level-0, no other level transitions are allowed. Equivalent to disabling power-saving. 
       Data transfer is allowed by default in this state. */ 

  QDI_PANEL_POWERSAVING_INFO_LEVEL1             = 0x00000004,
  /**< Allows up to level-1 conservative power-saving (lower latency). Level-1 takes precedence over level-0  
       if both level-1 and level-0 bits are set. Data transfer is allowed by default in this state. */

  QDI_PANEL_POWERSAVING_INFO_LEVEL2             = 0x00000008,
  /**< Allows up to level-2 aggressive power-saving (higher latency).
       Level-2 takes presedence over level-0 and level-1 if all level-2, level-1,
       and level-0 bits are set. The default is to allow power-savings (level-2). 
       Data transfer is allowed by default in this state. */
       
  QDI_PANEL_POWERSAVING_INFO_IDLE_OPTIMIZATION  = 0x00000010,
  /**< Allows up to Idle (static screen) Optimization 
       This level allows power saving when device is in PowerSaving Level-2 
       by dropping refresh rate to lowest supported, along with the Power 
       saving achieved by Level-2. */
  
  QDI_PANEL_POWERSAVING_RESERVED16              = 0x00000020,
                                                
  QDI_PANEL_POWERSAVE_INFO_LINK_ACTIVE          = 0x00000040,
  /**< Keep the link active even during power savings 1 and 2 cases. */

  QDI_PANEL_POWERSAVE_INFO_LINK_DISABLE         = 0x00000080,
  /**< Disable the link during power savings 1 and 2 cases; takes precedence over the link active flag. */

   /**< RESERVED */	
  QDI_PANEL_POWERSAVING_INFO_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32-bits. */ 

} QDI_Panel_PowerSavingOverrideType;


/**  Defines the I2C slaves.
*/
typedef enum
{
  QDI_PANEL_I2C_SLAVE_NONE = 0,    /**< No I2C slave is available. */
  QDI_PANEL_I2C_SLAVE_BACKLIGHT,   /**< I2C slave is for the Backlight driver */
  QDI_PANEL_I2C_SLAVE_EDID,        /**< I2C slave is for the Panel EDID EEPROM */
  QDI_PANEL_I2C_SLAVE_CONFIG_MAX,  /**< Enumeration marker for the maximum. */
  QDI_PANEL_I2C_SLAVE_CONFIG_FORCE_32BIT = 0x7FFFFFFF /**< Forces the enumeration
                                                           to 32 bits. @newpage */
}QDI_Panel_I2cSlaveType;


/** Panel event callbacks. Each event is represented by a single bit.
*/
typedef enum {
    QDI_PANEL_EVENT_UPDATE_REQUEST         = 0x00000001,
    /**< Panel screen update is requested. This request can be for any change,
         including cursor movements, updated surfaces, etc. (IRQL = Device) */

    QDI_PANEL_EVENT_UPDATE_COMPLETE        = 0x00000002,
    /**< Panel screen update is complete.

         For panels that require a command initialization sequence, this event
         is triggered when the initialization sequence is to be sent. This
         event is made during the Mode Set operation, during the transition
         from Idle (F1) to Active (F0). */

    QDI_PANEL_EVENT_D_STATE_NOTIFICATION   = 0x00000004,
    /**< D state notification. This is triggered at the entry of D state. */

    QDI_PANEL_EVENT_INIT_SEQUENCE          = 0x00000010,
    /**< Panel initialization sequence is requested. 

         For panels that require command sequences, this event is triggered
         when the panel is being turned off. This event is made after the data
         has been stopped but before the power rails are cut to the panel. This
         event occurs during the transition from Active (F0) to Idle (F1). \n
         (IRQL = Passive) */

    QDI_PANEL_EVENT_TERM_SEQUENCE          = 0x00000020,
    /**< Panel term sequence is requested. (IRQL = Passive) */

    QDI_PANEL_EVENT_RESET_START            = 0x00000100,
    /**< During ESD detection and recovery this event gets triggered when
         the internal ESD thread is attempting to reset and recover the
         display. This event indicates that the reset sequence has started.
         (IRQL = Passive) */

    QDI_PANEL_EVENT_RESET_COMPLETE         = 0x00000200,
    /**< ESD reset sequence is complete. (IRQL = Passive) */

    QDI_PANEL_EVENT_AUDIO_NOTIFY           = 0x00000400,
    /**< Requests an audio change for the external panel. (IRQL = Passive) */

    QDI_PANEL_EVENT_HOT_PLUG               = 0x00001000,
    /**< External panel was connected. (IRQL <= Dispatch) */

    QDI_PANEL_EVENT_HOT_UNPLUG             = 0x00002000,
    /**< External panel was disconnected. (IRQL <= Dispatch) */

    QDI_PANEL_EVENT_HDCP_ENABLE            = 0x00004000,
    /**< Driver has enabled HDCP. (IRQL = Passive) */

    QDI_PANEL_EVENT_HDCP_DISABLE           = 0x00008000,
    /**< Driver has disabled HDCP. (IRQL = Passive) */

    QDI_PANEL_EVENT_HIST_DATA_AVAIL        = 0x00010000,
    /**< Histogram data collection is complete and the data is available
         for drivers to fetch. (IRQL = Passive) */

    QDI_PANEL_EVENT_POST_POWER             = 0x00020000,
    /**< Performs necessary operations post power. When the
         QDI_Panel_SetPower() function is deprecated, this event is to take
         its place. \n (IRQL = Passive) */
        
    QDI_PANEL_EVENT_PRE_POWER              = 0x00040000,
    /**< Performs necessary operations pre power. When the QDI_Panel_SetPower()  
         function is deprecated, this event is to take its place. \n
         (IRQL = Passive) */

    QDI_PANEL_EVENT_DRIVER_LOADED          = 0x00080000,
    /**< Display initialization is complete. (IRQL = Passive) */

    QDI_PANEL_EVENT_READ_COMPLETE          = 0x00100000,
    /**< Panel read complete. */

    QDI_PANEL_EVENT_READ_ERROR             = 0x00200000,
    /**< Panel error during read. */

    QDI_PANEL_EVENT_READ_TIMEOUT           = 0x00400000,
    /**< Panel timeout occurred during read. */

    QDI_PANEL_EVENT_WRITE_COMPLETE         = 0x01000000,
    /**< Panel write complete. */

    QDI_PANEL_EVENT_WRITE_ERROR            = 0x02000000,
    /**< Panel error occurred during write. */

    QDI_PANEL_EVENT_WRITE_TIMEOUT          = 0x04000000,
    /**< Panel timeout occurred during write. */

    QDI_PANEL_EVENT_AMBIENT_LEVEL_CHANGE   = 0x08000000,
    /**< Change in Ambient level occurred. */

    QDI_PANEL_EVENT_VSYNC_IRQ              = 0x10000000,
    /**< Video synchronization (Vsync) interrupt is received. (IRQL = DIRQL) */
    
    QDI_PANEL_EVENT_VSYNC_ENABLED          = 0x20000000,
    /**< Vsync interrupt is enabled. (IRQL = Passive) */
    
    QDI_PANEL_EVENT_VSYNC_DISABLED         = 0x40000000,
    /**< Vsync interrupt is disabled. (IRQL = Passive) */

    QDI_PANEL_EVENT_HDCP_STATUS            = 0x80000000,
    /**< Requests the HDCP status. */

    QDI_PANEL_EVENT_FORCE_32BIT            = 0x7FFFFFFF
   /**< Forces the enumeration to 32 bits. */

} QDI_Panel_EventTypes;


/** Defines the DSI host connection to the type of panel.
*/
typedef enum {
    QDI_PANEL_LANE0123MAP = 0x00000000,
    /**< 0 -- Connected panel's connection order to the host is 0,1,2,3. */

    QDI_PANEL_LANE3012MAP,
    /**< 1 -- Connected panel's connection order to the host is 3,0,1,2. */

    QDI_PANEL_LANE2301MAP,
    /**< 2 -- Connected panel's connection order to the host is 2,3,0,1. */

    QDI_PANEL_LANE1230MAP,
    /**< 3 -- Connected panel's connection order to the host is 1,2,3,0. */

    QDI_PANEL_LANE0321MAP,
    /**< 4 -- Connected panel's connection order to the host is 0,3,2,1. */

    QDI_PANEL_LANE1032MAP,
    /**< 5 -- Connected panel's connection order to the host is 1,0,3,2. */

    QDI_PANEL_LANE2103MAP,
    /**< 6 -- Connected panel's connection order to the host is 2,1,0,3. */

    QDI_PANEL_LANE3210MAP,
    /**< 7 -- Connected panel's connection order to the host is 3,2,1,0. */

    QDI_PANEL_LANEMAP_FORCE_32BIT   = 0x7FFFFFFF
   /**< Forces the enumeration to 32 bits. */

} QDI_DSILaneMapType;


/** Defines the DSI host error codes that are generated in the DSI controller.
*/
typedef enum
{
    QDI_DSI_HOST_ERROR_DPHY_UNLOCK              = 0x00000001,
    /**< DSI PLL unlock. */

    QDI_DSI_HOST_ERROR_DLN0_CONTENTION_LP1      = 0x00000002,
    /**< Contention was detected while lane 0 was driven high. */

    QDI_DSI_HOST_ERROR_DLN0_CONTENTION_LP0      = 0x00000004,
    /**< Contention was detected while lane 0 was driven low. */

    QDI_DSI_HOST_ERROR_DLN0_CONTROL             = 0x00000008,
    /**< Incorrect line state sequence was detected. */

    QDI_DSI_HOST_ERROR_DLN0_SYNC_ESC            = 0x00000010,
    /**< Number of received bits in Low Power mode is not a multiple of eight. */

    QDI_DSI_HOST_ERROR_DLN0_ESC                 = 0x00000020,
    /**< Unrecognized escape entry command was received. */

    QDI_DSI_HOST_ERROR_DLN0_LP_FIFO_OVERFLOW    = 0x00000040,
    /**< FIFO for low-speed data on lane 0 overflow. */

    QDI_DSI_HOST_ERROR_DLN0_HS_FIFO_OVERFLOW    = 0x00000080,
    /**< FIFO for high-speed data on lane 0 overflow. */

    QDI_DSI_HOST_ERROR_DLN1_HS_FIFO_OVERFLOW    = 0x00000100,
    /**< FIFO for high-speed data on lane 1 overflow. */

    QDI_DSI_HOST_ERROR_DLN2_HS_FIFO_OVERFLOW    = 0x00000200,
    /**< FIFO for high-speed data on lane 2 overflow. */

    QDI_DSI_HOST_ERROR_DLN3_HS_FIFO_OVERFLOW    = 0x00000400,
    /**< FIFO for high-speed data on lane 3 overflow. */

    QDI_DSI_HOST_ERROR_VID_MDP_FIFO_UNDERFLOW   = 0x00000800,
    /**< Video mode engine FIFO underflow. */

    QDI_DSI_HOST_ERROR_VID_MDP_FIFO_OVERFLOW    = 0x00001000,
    /**< Video mode engine FIFO overflow. */

    QDI_DSI_HOST_ERROR_CMD_MDP_FIFO_UNDERFLOW   = 0x00002000,
    /**< Command mode engine MDP FIFO underflow. */

    QDI_DSI_HOST_ERROR_CMD_DMA_FIFO_UNDERFLOW   = 0x00004000,
    /**< Command mode engine DMA FIFO underflow. */

    QDI_DSI_HOST_ERROR_INTERLEAVE_OP_CONTENTION = 0x00008000,
    /**< Command mode packets and video mode packets were generated at the
         same time. */

    QDI_DSI_HOST_ERROR_BTA_TIMEOUT              = 0x00010000,
    /**< Basic trading area  timeout. */

    QDI_DSI_HOST_ERROR_HS_TX_TIMEOUT            = 0x00020000,
    /**< High-speed forward transmission timeout. */

    QDI_DSI_HOST_ERROR_LP_RX_TIMEOUT            = 0x00040000,
    /**< Low-power reverse transmission timeout. */

    QDI_DSI_HOST_ERROR_PACKET                   = 0x00080000,
    /**< Error report was returned from the DSI display module. */

    QDI_DSI_HOST_ERROR_RDBK_INCOMPLETE_PACKET   = 0x00100000,
    /**< An incomplete read packet was received. */

    QDI_DSI_HOST_ERROR_RDBK_DATA_CRC            = 0x00200000,
    /**< Incorrect CRC was detected in the read packet. */

    QDI_DSI_HOST_ERROR_RDBK_DATA_MULTI_ECC      = 0x00400000,
    /**< Multi-bit ECC was detected in the read packet and was not corrected. */

    QDI_DSI_HOST_ERROR_RDBK_DATA_ECC            = 0x00800000,
    /**< Single-bit ECC was detected in the read packet and was corrected by
         the DSI controller. */

    QDI_DSI_HOST_ERROR_FORCE_32BIT              = 0x7FFFFFFF
   /**< Forces the enumeration to 32 bits. */

} QDI_DSIHostErrorType;


/** Defines the DSI panel error codes that are generated in the DSI display
    module.
*/
typedef enum
{
    QDI_DSI_PANEL_ERROR_PROTOCOL_VIOLATION      = 0x00000001,
    /**< DSI protocol violation. */

    QDI_DSI_PANEL_ERROR_INVALID_TX_LENGTH       = 0x00000002,
    /**< Invalid transmission length. */

    QDI_DSI_PANEL_ERROR_INVALID_VC_ID           = 0x00000004,
    /**< DSI virtual channel ID is invalid. */

    QDI_DSI_PANEL_ERROR_INVALID_DATA_TYPE       = 0x00000008,
    /**< DSI data type was not recognized. */

    QDI_DSI_PANEL_ERROR_CRC                     = 0x00000010,
    /**< Checksum error (long packet only). */

    QDI_DSI_PANEL_ERROR_MULTI_ECC               = 0x00000020,
    /**< Multi-bit ECC error was detected and not corrected. */

    QDI_DSI_PANEL_ERROR_SINGLE_ECC              = 0x00000040,
    /**< Single-bit ECC error was detected and corrected. */

    QDI_DSI_PANEL_ERROR_CONTENTION              = 0x00000080,
    /**< Contention error. */

    QDI_DSI_PANEL_ERROR_FALSE_CONTROL           = 0x00000100,
    /**< False control error. */

    QDI_DSI_PANEL_ERROR_PERIPHERAL_TIMEOUT      = 0x00000200,
    /**< Peripheral timeout error. */

    QDI_DSI_PANEL_ERROR_LP_TX_SYNC              = 0x00000400,
    /**< Low-power transmit synchronization error. */

    QDI_DSI_PANEL_ERROR_ESC_CMD                 = 0x00000800,
    /**< Escape mode entry command error. */

    QDI_DSI_PANEL_ERROR_EOT_SYNC                = 0x00001000,
    /**< EoT synchronization error. */

    QDI_DSI_PANEL_ERROR_SOT_SYNC                = 0x00002000,
    /**< SoT synchronization error. */

    QDI_DSI_PANEL_ERROR_SOT                     = 0x00004000,
    /**< SoT error.*/

    QDI_DSI_PANEL_ERROR_FORCE_32BIT             = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. @newpage */

} QDI_DSIPanelErrorType;


/** Defines the logic IDs for the DSI client panel. Multiple display panels
    can be attached to one DSI host core.
*/
typedef enum
{
  QDI_DSIChannelID_NONE = 0,     /**< No channel ID. */
  QDI_DSIChannelID_Video0,       /**< DSI Video mode channel. */    
  QDI_DSIChannelID_CMD0,         /**< DSI Command mode channel 0. */
  QDI_DSIChannelID_CMD1,         /**< DSI Command mode channel 1. */
  QDI_DSIChannelID_MAX,          /**< Enumeration marker for the max. */
  QDI_DSIChannelID_FORCE_32BIT = 0x7FFFFFFF /**< Forces the enumeration to
                                                 32 bits. */
} QDI_DSIChannelIDType;


/** Defines the DSI virtual channel IDs.
*/
typedef enum
{
  QDI_DSIDisplayVC_0  = 0x0,     /**< Virtual channel 0. */
  QDI_DSIDisplayVC_1,            /**< Virtual channel 1. */
  QDI_DSIDisplayVC_2,            /**< Virtual channel 2. */
  QDI_DSIDisplayVC_3,            /**< Virtual channel 3. */
  QDI_DSIDisplayVC_MAX,          /**< Enumeration marker for the max. */
  QDI_DSIDisplayVC_FORCE_32BIT = 0x7FFFFFFF /**< Forces the enumeration to
                                                 32 bits. @newpage */
} QDI_DSIDisplayVCType;


/** Enumeration for all of the supported DSI color formats.
*/
typedef enum
{
  QDI_DSICOLOR_RGB_111_3BPP = 0, /**< RGB111 format at 3 bpp. */
  QDI_DSICOLOR_RBG_111_3BPP,     /**< RBG111 format at 3 bpp. */
  QDI_DSICOLOR_BGR_111_3BPP,     /**< BGR111 format at 3 bpp. */
  QDI_DSICOLOR_BRG_111_3BPP,     /**< BRG111 format at 3 bpp. */
  QDI_DSICOLOR_GRB_111_3BPP,     /**< GRB111 format at 3 bpp. */
  QDI_DSICOLOR_GBR_111_3BPP,     /**< GBR111 format at 3 bpp. */
  
  QDI_DSICOLOR_RGB_332_8BPP,     /**< RGB332 format at 8 bpp. */
  QDI_DSICOLOR_RBG_332_8BPP,     /**< RBG332 format at 8 bpp. */
  QDI_DSICOLOR_BGR_332_8BPP,     /**< BGR332 format at 8 bpp. */
  QDI_DSICOLOR_BRG_332_8BPP,     /**< BRG332 format at 8 bpp. */
  QDI_DSICOLOR_GRB_332_8BPP,     /**< GRB332 format at 8 bpp. */
  QDI_DSICOLOR_GBR_332_8BPP,     /**< GBR332 format at 8 bpp. */
  
  QDI_DSICOLOR_RGB_444_12BPP,    /**< RGB444 format at 12 bpp. */
  QDI_DSICOLOR_RBG_444_12BPP,    /**< RBG444 format at 12 bpp. */
  QDI_DSICOLOR_BGR_444_12BPP,    /**< BGR444 format at 12 bpp. */
  QDI_DSICOLOR_BRG_444_12BPP,    /**< BRG444 format at 12 bpp. */
  QDI_DSICOLOR_GRB_444_12BPP,    /**< GRB444 format at 12 bpp. */
  QDI_DSICOLOR_GBR_444_12BPP,    /**< GBR444 format at 12 bpp. */
  
  QDI_DSICOLOR_RGB_565_16BPP,    /**< RGB565 format at 16 bpp. */
  QDI_DSICOLOR_RBG_565_16BPP,    /**< RBG565 format at 16 bpp. */
  QDI_DSICOLOR_BGR_565_16BPP,    /**< BGR565 format at 16 bpp. */
  QDI_DSICOLOR_BRG_565_16BPP,    /**< BRG565 format at 16 bpp. */
  QDI_DSICOLOR_GRB_565_16BPP,    /**< GRB565 format at 16 bpp. */
  QDI_DSICOLOR_GBR_565_16BPP,    /**< GBR565 format at 16 bpp. */
  
  QDI_DSICOLOR_RGB_666_18BPP,    /**< RGB666 format at 18 bpp. */
  QDI_DSICOLOR_RBG_666_18BPP,    /**< RBG666 format at 18 bpp. */
  QDI_DSICOLOR_BGR_666_18BPP,    /**< BGR666 format at 18 bpp. */
  QDI_DSICOLOR_BRG_666_18BPP,    /**< BRG666 format at 18 bpp. */
  QDI_DSICOLOR_GRB_666_18BPP,    /**< GRB666 format at 18 bpp. */
  QDI_DSICOLOR_GBR_666_18BPP,    /**< GBR666 format at 18 bpp. */

  QDI_DSICOLOR_RGB_666_24BPP,    /**< RGB666 format at 24 bpp. */
  QDI_DSICOLOR_RBG_666_24BPP,    /**< RBG666 format at 24 bpp. */
  QDI_DSICOLOR_BGR_666_24BPP,    /**< BGR666 format at 24 bpp. */
  QDI_DSICOLOR_BRG_666_24BPP,    /**< BRG666 format at 24 bpp. */
  QDI_DSICOLOR_GRB_666_24BPP,    /**< GRB666 format at 24 bpp. */
  QDI_DSICOLOR_GBR_666_24BPP,    /**< GBR666 format at 24 bpp. */

  QDI_DSICOLOR_RGB_888_24BPP,    /**< RGB888 format at 24 bpp. */
  QDI_DSICOLOR_RBG_888_24BPP,    /**< RBG888 format at 24 bpp. */
  QDI_DSICOLOR_BGR_888_24BPP,    /**< BGR888 format at 24 bpp. */
  QDI_DSICOLOR_BRG_888_24BPP,    /**< BRG888 format at 24 bpp. */
  QDI_DSICOLOR_GRB_888_24BPP,    /**< GRB888 format at 24 bpp. */
  QDI_DSICOLOR_GBR_888_24BPP,    /**< GBR888 format at 24 bpp. */
}QDI_DSIColorFormatType;


/** Defines the DSI Traffic mode types.
*/
typedef enum
{
  QDI_DSIVideoTrafficMode_NonBurst_VSPulse = 0,
  /**< Non-burst mode with a vertical synchronization pulse. */

  QDI_DSIVideoTrafficMode_NonBurst_VSEvent,
  /**< Non-burst mode with a vertical synchronization event. */

  QDI_DSIVideoTrafficMode_Burst,
  /**< Burst mode. */

  QDI_DSIVideoTrafficMode_MAX,
  /**< Enumeration marker for the max. */

  QDI_DSIVideoTrafficMode_FORCE_32BIT = 0x7FFFFFFF
 /**< Forces the enumeration to 32 bits. */

} QDI_DSITrafficModeType;


/** Clock lane mode configuration. This configuration is used to control  
    clock lane behavior during normal operation.
 */
typedef enum
{
  QDI_DSI_ClockLane_Default = 0x0,
  /**< Clock lane transitions from LP11 to high-speed only when high-speed
       data is being transferred. */

  QDI_DSI_ClockLane_ForceHS,
  /**< Keep clock lane at high-speed. */

  QDI_DSI_ClockLane_ForceHS_PostInit,
  /**< Keep clock lane at high-speed after the panel initialization
       sequence completes. */

  QDI_DSI_ClockLane_MAX,
  /**< Enumeration marker for the max. */

  QDI_DSI_ClockLane_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_DSI_ClockLaneType;


/** DSI PHY SSC mode configuration.
    This enum configures the SSC for the DSI PLL.
*/
typedef enum
{
    QDI_DSI_PHY_SSCDisable = 0x0,
    /**< SSC mode disabled. */

    QDI_DSI_PHY_SSCDownspread,
    /**< SSC type is down spread. */

    QDI_DSI_PHY_SSCCenterSpread,
    /**< SSC type is center spread. */

    QDI_DSI_PHY_SSCConfig_MAX,
    /**< Enumeration marker for the max. @newpage */

} QDI_DSI_PHY_SSCConfig;


/** Defines the scenarios to enable or disable the high speed pixel data transfer in video mode configuration
 */
typedef enum
{
  QDI_DSI_PIXEL_TX_DISABLED_DURING_INIT_TERM  =  0x00000000,
  /**< Video pixel data transfer is disabled during initialization and termination
       sequence. */
  
  QDI_DSI_PIXEL_TX_ENABLED_DURING_INIT        =  0x00000001,
  /**< Video pixel data transfer is enabled during the initialization sequence and 
       disabled during the termination sequence. */

  QDI_DSI_PIXEL_TX_ENABLED_DURING_TERM        =  0x00000002,
  /**< Video pixel data transfer is enabled during the termination sequence and 
       disabled during the initialization sequence. */

  QDI_DSI_PIXEL_TX_ENABLED_DURING_INIT_TERM   =  (QDI_DSI_PIXEL_TX_ENABLED_DURING_INIT | 
                                                  QDI_DSI_PIXEL_TX_ENABLED_DURING_TERM),
  /**< Video pixel data transfer is enabled during the initialization and the
       termination sequence. */

  QDI_DSI_PIXEL_TX_TIMING_FORCE_32BIT         =  0x7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_DSI_PixelTxTiming;

/** Defines the PMIC device type.
*/
typedef enum 
{
  QDI_PMIC_DEVICEID_NONE       = 0,          /**< No device type. */
  QDI_PMIC_DEVICEID_8921,                    /**< Device type is PMIC8921. */
  QDI_PMIC_DEVICEID_8038,                    /**< Device type is PMIC8038. */
  QDI_PMIC_DEVICEID_8941,                    /**< Device type is PMIC8941. */  
  QDI_PMIC_DEVICEID_8841,                    /**< Device type is PMIC8841. */ 
  QDI_PMIC_DEVICEID_8026,                    /**< Device type is PMIC8026. */ 
  QDI_PMIC_DEVICEID_8926,                    /**< Device type is PMIC8926. */ 
  QDI_PMIC_DEVICEID_8084,                    /**< Device type is PMIC8084. */
  QDI_PMIC_DEVICEID_8962,                    /**< Device type is PMIC8962. */
  QDI_PMIC_DEVICEID_8994,                    /**< Device type is PM8994.   */
  QDI_PMIC_DEVICEID_8994I,                   /**< Device type is PMI8994.  */
  QDI_PMIC_DEVICEID_8916,                    /**< Device type is PMIC8916. */
  QDI_PMIC_DEVICEID_8110,                    /**< Device type is PMIC8110. */
  QDI_PMIC_DEVICEID_8909,                    /**< Device type is PMIC8909. */
  QDI_PMIC_DEVICEID_8950,                    /**< Device type is PM8950.   */
  QDI_PMIC_DEVICEID_8950I,                   /**< Device type is PMI8950.  */
  QDI_PMIC_DEVICEID_8998        = 0x14,      /**< Device type is PM8998.   */
  QDI_PMIC_DEVICEID_8998I       = 0x15,      /**< Device type is PMI8998.  */
  QDI_PMIC_DEVICEID_PM855       = 0x1E,      /**< Device type is PM855.    */
  QDI_PMIC_DEVICEID_PM855L      = 0x1F,      /**< Device type is PM855L.   */
  QDI_PMIC_DEVICEID_PM855B      = 0x20,      /**< Device type is PM855B.   */
  QDI_PMIC_DEVICEID_PM855P      = 0x26,      /**< Device type is PM855P.   */
  QDI_PMIC_DEVICEID_PM855A      = 0x27,      /**< Device type is PM855A.   */
  QDI_PMIC_DEVICEID_MAX,                     /**< Enumeration marker for the max. */
  QDI_PMIC_DEVICEID_FORCE_32BIT = 0X7FFFFFFF /**< Forces the enumeration to
                                                  32 bits. @newpage */
} QDI_PMICDeviceIdType;


/** Defines the miscellaneous display flags. There is one feature per bit.
*/
typedef enum 
{
  QDI_PANEL_FLAG_NONE                      = 0x00000000,
  /**< Default flag state. */

  QDI_PANEL_FLAG_FORCE_DISPLAY_REINIT      = 0x00000001,
  /**< Bypasses a seamless transition from the firmware to the OS driver. Always
       force a reinitialization of the display. */

  QDI_PANEL_FLAG_DISABLE_SBC               = 0x00000002,
  /**< Disables force smooth brightness capabilities. */

  QDI_PANEL_FLAG_DISABLE_CABL              = 0x00000004,
  /**< Disables adaptive backlight level capabilities. */

  QDI_PANEL_FLAG_DISABLE_HW_CURSOR         = 0x00000008,
  /**< Disables hardware cursors.  */

  QDI_PANEL_FLAG_DUAL_MODE_SUPPORT         = 0x00000010,
  /**< Enables Video and Command mode support.  */

  QDI_PANEL_FLAG_DSI_DCS_POLLING_TRANSFER  = 0x00000100,
  /**< Disables the event-based and fallback to polling-based DSI DCS sequence. */
  /** @cond */
  QDI_PANEL_FLAG_DSI_DCS_DMA_ONLY          = 0x00000200,
  /* Deprecated. DSI command data is only fetched from memory via the DMA VBIF interface. */
  /** @endcond */
  QDI_PANEL_FLAG_DSI_DCS_FIFO_ONLY         = 0x00000400,
  /**< Deprecated -- Use #QDI_PANEL_DSI_FLAG_DSI_DCS_FIFO_ONLY instead. DSI command data is only fetched via TPG FIFO.  */

  QDI_PANEL_FLAG_ENABLE_DFS_IDLESCREEN     = 0x00000800,
  /**< Enables the dynamic refresh rate for an idle screen. */

  QDI_PANEL_FLAG_MIRROR_DUAL_PIPE_CONFIG   = 0x00001000,
  /**< Swap-layer source ROI for dual-pipe configuration in the layer mixer. */

  QDI_PANEL_FLAG_DISABLE_POST_PROCESSING   = 0x00002000,
  /**< Disable all color management and post-processing features. */

  QDI_PANEL_FLAG_INJECT_DEFAULT_AUDIO_MODE = 0x00004000,
  /**< Flag to request to add a default audio mode. */    

  QDI_PANEL_FLAG_STEREO_MODE_SUPPORT       = 0x00008000,
  /**< Flag to add support for Stereo/VR/AR use cases. */ 

  QDI_PANEL_FLAG_MULTIPANEL_SYNC           = 0x00010000,
  /**< Flag to specify panel supports multipanel-sync */   

  QDI_PANEL_FLAG_FRAME_REPEAT              = 0x00020000,
  /**< Flag to enable repeating of frames with vsync when no new frames are queued */    

  QDI_PANEL_FLAG_DISABLE_DSC               = 0x00040000,
  /**< Flag to disable Display Stream Compression (DSC) */

  QDI_PANEL_FLAG_DISABLE_MST               = 0x00080000,
  /**< Flag to disable Multi Stream Transport (MST) */

  QDI_PANEL_FLAG_STRICT_EDID_CHECK         = 0x00100000,
  /**< Flag to enforce strict EDID check            */
} QDI_Panel_FlagsType;

/** Defines the miscellaneous DSI flags used in QDI_Panel_DSIAttrType. There is one feature per bit.
*/
typedef enum
{
  QDI_PANEL_DSI_FLAG_NONE                       = 0x00000000,
  /**< Default flag state. */

  QDI_PANEL_DSI_FLAG_DSI_DCS_FIFO_ONLY          = 0x00000001,
  /**< DSI command data is only to be fetched via TPG FIFO. */

  QDI_PANEL_DSI_FLAG_DSI_DISABLE_BURST_MODE     = 0x00000002,
  /**< Disable the DSI HS burst mode. */

  QDI_PANEL_DSI_FLAG_DISABLE_PPS_CMD            = 0x00000004,
  /**< Skip sending PPS command for DCS panel. */

  QDI_PANEL_DSI_FLAG_NO_WAIT                    = 0x00000008,
  /**< Flag that forces no-wait transfer, for high IRQL. */

} QDI_Panel_DSI_FlagsType;


/** PMIC control method for backlight control.
 */
typedef enum 
{
  QDI_PMIC_MODULE_CONTROLTYPE_NONE = 0,
  /**< No Backlight control method. */

  QDI_PMIC_MODULE_CONTROLTYPE_LPG,
  /**< Backlight control through the LPG module. */

  QDI_PMIC_MODULE_CONTROLTYPE_WLED,
  /**< Backlight controlled through the WLED module. */

  QDI_PMIC_MODULE_CONTROLTYPE_PWM,
  /**< Backlight control through the PWM module. */

  QDI_PMIC_MODULE_CONTROLTYPE_MAX,
  /**< Enumeration marker for the maximum control type. */

  QDI_PMIC_MODULE_CONTROLTYPE_FORCE_32BIT = 0X7FFFFFFF 
  /**< Forces the enumeration to 32 bits. */

} QDI_PmicModuleControlType;


/** PMIC control method interface modules (used as a secondary power source).
 */
typedef enum 
{
  QDI_PMIC_INTERFACE_CONTROLTYPE_NONE = 0,
  /**< No interface module required. */

  QDI_PMIC_INTERFACE_CONTROLTYPE_IBB_LAB_LCD,
  /**< Enable interface module IBB/LAB for LCD. */

  QDI_PMIC_INTERFACE_CONTROLTYPE_IBB_LAB_OLED,
  /**< Enable interface module IBB/LAB for OLED . */

  QDI_PMIC_INTERFACE_CONTROLTYPE_MAX,
  /**< Enumeration marker for the maximum. */

  QDI_PMIC_INTERFACE_CONTROLTYPE_FORCE_32BIT = 0X7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_PmicInterfaceControlType;


/** List of external Vsync sources.
 */
typedef enum _QDI_ExternalVsyncSelectType
{
    QDI_EXTERNAL_VSYNC_SELECT_TYPE_VSYNC_0_GPIO    = 0x00000000,         
    /**< Vsync 0 from GPIO. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_VSYNC_1_GPIO    = 0x00000001,         
    /**< Vsync 1 from GPIO. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_VSYNC_2_GPIO    = 0x00000002,         
    /**< Vsync 2 from GPIO. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_VSYNC_INTF_0    = 0x00000003,         
    /**< Vsync from interface 0. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_VSYNC_INTF_1    = 0x00000004,         
    /**< Vsync from interface 1. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_VSYNC_INTF_2    = 0x00000005,         
    /**< Vsync from interface 2. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_VSYNC_INTF_3    = 0x00000006,         
    /**< Vsync from interface 3. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_MAX,
    /**< Enumeration marker for the max. */

    QDI_EXTERNAL_VSYNC_SELECT_TYPE_FORCE_32BIT  = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. @newpage */

} QDI_ExternalVsyncSelectType;


/** Power votes for components.
 */
typedef enum
{
  QDI_PANEL_POWER_VOTE_NONE = 0,
  /**< Reserved. */

  QDI_PANEL_POWER_VOTE_ACTIVE,           
  /**< Active power vote for a component. */

  QDI_PANEL_POWER_VOTE_IDLE,             
  /**< Idle power vote for a component. */

  QDI_PANEL_POWER_VOTE_MAX,
  /**< Enumeration marker for the max. */

  QDI_PANEL_POWER_VOTE_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_Panel_PowerVoteType;


/** Component type to be controlled.
 */
typedef enum
{
  QDI_PANEL_COMPONENT_NONE = 0,
  /**< Reserved. */

  QDI_PANEL_COMPONENT_DISPLAY,               
  /**< Display power component.  */

  QDI_PANEL_COMPONENT_MEMORY_REFRESH,        
  /**< Refresh power component memory. */

  QDI_PANEL_COMPONENT_ENGINE,                
  /**< Engine component (Reserved). */

  QDI_PANEL_COMPONENT_MAX,
  /**< Enumeration marker for the max. */

  QDI_PANEL_COMPONENT_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_Panel_ComponentType;

/** Adaptive brightness features
*/
typedef enum
{
  QDI_ADAPTIVE_BRIGHTNESS_FEATURE_NONE = 0,
  /**< Reserved for future use. */

  QDI_ADAPTIVE_BRIGHTNESS_FEATURE_CABL,
  /**< CABL.  */

  QDI_ADAPTIVE_BRIGHTNESS_FEATURE_FOSS,
  /**< FOSS.  */

  QDI_ADAPTIVE_BRIGHTNESS_FEATURE_MAX,
  /**< Enumeration marker for the maximum number of brightness features. */

  QDI_ADAPTIVE_BRIGHTNESS_FEATURE_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_AdaptiveBrightnessFeatureType;

/** Defines gamut mapping modes.
*/
typedef enum
{
  QDI_PANEL_GM_MODE_NONE        = 0x00000000,
  /**< No gamut mapping mode support. */

  QDI_PANEL_GM_MODE_UNIFORM     = 0x00000001,
  /**< Uniform mode gamut mapping flag. */

  QDI_PANEL_GM_MODE_NON_UNIFORM = 0x00000002,
  /**< Non-uniform mode gamut mapping flag. */

  QDI_PANEL_GM_MODE_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32-bits. */
  
} QDI_Panel_GamutMappingModeType;

/** Defines gamma correction types.
*/
typedef enum 
{
  QDI_PANEL_GC_TYPE_NONE        = 0,
  /**< No gamma correction support. */

  QDI_PANEL_GC_TYPE_SEGMENT_LUT,
  /**< Segmented LUT gamma correction type. */

  QDI_PANEL_GC_TYPE_LINEAR_LUT,
  /**< Linear LUT gamma correction type . */
  
  QDI_PANEL_GC_TYPE_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32-bits. */
  
} QDI_Panel_GammaCorrectionType;

/** Defines types of polynomial color correction.
*/
typedef enum
{
  QDI_PANEL_PCC_TYPE_NONE        = 0,
  /**< No polynomial color correction support. */

  QDI_PANEL_PCC_TYPE_MATRIX_3X11,
  /**< Polynomial color correction type, matrix 3x11. */

  QDI_PANEL_PCC_TYPE_MATRIX_3X8,
  /**< Polynomial color correction type, matrix 3x8. */
  
  QDI_PANEL_PCC_TYPE_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32-bits. */ 

} QDI_Panel_ColorCorrectionType;


/** Defines dithering modes.
*/
typedef enum 
{
  QDI_PANEL_DITHER_MODE_NONE        = 0x00000000,
  /**< No dithering mode support. */

  QDI_PANEL_DITHER_MODE_SPATIAL     = 0x00000001,
  /**< Spatial dithering mode. */

  QDI_PANEL_DITHER_MODE_TEMPORAL    = 0x00000002,
  /**< Temporal dithering mode. */
  
  QDI_PANEL_DITHER_MODE_FORCE_32BIT = 0x7FFFFFFF
  /**< Forces the enumeration to 32-bits. */
  
} QDI_Panel_DitheringModeType;

/** Defines HDCP status.
    Used in #QDI_PANEL_EVENT_HDCP_STATUS.
*/
typedef enum 
{
  QDI_PANEL_HDCP_STATUS_IDLE              = 0x00000000,
  /**< HDCP is not enabled. */

  QDI_PANEL_HDCP_STATUS_AUTHENTICATED     = 0x00000001,
  /**< HDCP has been authenticated. */

  QDI_PANEL_HDCP_STATUS_FAILED            = 0x00000002,
  /**< HDCP authentication failed. */

  QDI_PANEL_HDCP_STATUS_IN_PROGRESS       = 0x00000003,
  /**< In the process of HDCP authentication. */
  
  QDI_PANEL_HDCP_STATUS_FORCE_32BIT       = 0x7FFFFFFF
  /**< Forces the enumeration to 32-bits. @newpage */
  
} QDI_Panel_HDCPStatusType;

/** Defines HDCP level that is being negotiated by display hardware.
    Used in #QDI_PANEL_EVENT_HDCP_ENABLE.
*/
typedef enum 
{
  QDI_PANEL_HDCP_VERSION_NONE             = 0,
  /**< HDCP version unspecified. */

  QDI_PANEL_HDCP_VERSION_1p4,
  /**< HDCP 1.4. */

  QDI_PANEL_HDCP_VERSION_2p2,
  /**< HDCP 2.2. */

  QDI_PANEL_HDCP_VERSION_MAX,
  /**< Enumeration marker for maximum version supported. */

  QDI_PANEL_HDCP_VERSION_FORCE_32BIT      = 0x7FFFFFFF
  /**< Forces the enumeration to 32-bits. @newpage */
  
} QDI_Panel_HDCPVersionType;

/** Defines to specify the panel command sequence type.
    Used in #QDI_Panel_DSIAttrType.
*/
typedef enum
{
   QDI_PANEL_SEQUENCE_INIT              = 0x000000,
   /**< Panel initialization sequence. */

   QDI_PANEL_SEQUENCE_TERM,
   /**< Panel termination sequence. */

   QDI_PANEL_SEQUENCE_ESD,
   /**< Panel ESD detection sequence. */

   QDI_PANEL_SEQUENCE_VSYNC_ENABLE,
   /**< Panel Vsync enable sequence.  */

   QDI_PANEL_SEQUENCE_VSYNC_DISABLE,
   /**< Panel Vsync disable sequence.  */

   QDI_PANEL_SEQUENCE_QSYNC_ENABLE,
   /**< Panel Qsync enable sequence.  */

   QDI_PANEL_SEQUENCE_QSYNC_DISABLE,
   /**< Panel Qsync disable sequence.  */

   QDI_PANEL_SEQUENCE_SYNC_SET_MASTER,
   /**< Set synchronized panel as master.  */

   QDI_PANEL_SEQUENCE_SYNC_SET_SLAVE,
   /**< Set synchronized panel as slave.  */

   QDI_PANEL_SEQUENCE_MAX,
   /**< Enumeration marker for the maximum number of sequence types. */

   QDI_PANEL_SEQUENCE_VERSION_FORCE_32BIT      = 0x7FFFFFFF
   /**< Forces the enumeration to 32-bits. @newpage */

} QDI_Panel_SequenceTypes;


/** @} */ /* end_addtogroup data_types_qdi */

/* -----------------------------------------------------------------------
** Shared Structures
** ----------------------------------------------------------------------- */

/** @addtogroup data_types_edid
@{ */
/* -----------------------------------------------------------------------
** EDID 1.3 Headers
** ----------------------------------------------------------------------- */

#pragma pack(push)
#pragma pack(1)

/** Defines the EDID established timings.
*/
typedef struct {
    uint8 TimingI;              /**< Established timings table I. */
    uint8 TimingII;             /**< Established timings table II. */
    uint8 ManufacturesTiming;   /**< Manufacturer's timings table. */
} EDID_EstablishedTimings;

/** Defines the EDID standard timings.
*/
typedef struct {
   uint8  HorizontalPixels;     /**< Horizontal pixel width. */
   uint8  AspectRatio_Refresh;  /**< Aspect ratio and field refresh rate.
                                     @newpagetable */
} EDID_StandardTimings;

/** Defines the EDID detailed timings.
*/
typedef struct {
   uint16  PixelClock;
   /** Pixel clock frequency. */
   
   uint8   HPixels;
   /**< Lower eight bits of the horizontal addressable video pixels. */
   
   uint8   HBlanking;
   /**< Lower eight bits of the horizontal blanking pixels. */
   
   uint8   HPixels_Blanking;
   /**< Higher four bits of the horizontal active and blanking pixels. */
   
   uint8   VPixels;
   /**< Lower eight bits of the vertical addressable video pixels. */
   
   uint8   VBlanking;
   /**< Lower eight bits of the vertical blanking pixels. */
   
   uint8   VPixels_Blanking;
   /**< Higher four bits of the vertical active and blanking pixels. */
   
   uint8   HFrontPorch;
   /**< Lower eight bits of the horizontal front porch. */
   
   uint8   HSyncWidth;
   /**< Lower eight bits of the horizontal synchronization width. */
   
   uint8   VFrontPorch_SyncWidth;
   /**< Lower four bits of the vertical front porch and synchronization width. */
   
   uint8   HVPorchSyncWidths;
   /**< Higher two bits of the vertical/horizontal porch and synchronization
        widths. */
   
   uint8   HImageSize;
   /**< Lower eight bits of the video image width. */
   
   uint8   VImageSize;
   /**< Lower eight bits of the video image height. */
   
   uint8   HVImageSize;
   /**< Upper four bits of the video image width/height. */
   
   uint8   HBorder;
   /**< Horizontal border size. */
   
   uint8   VBorder;
   /**< Vertical border size. */
   
   uint8   SignalTimingInterface;
   /**< Detailed timing signal information. */
   
} EDID_DetailedTiming;

/**  Defines the EDID display descriptor.
*/
typedef struct {
   uint16  DescriptorHeader;  /**< Header for this descriptor: 0x0 indicates
                                   that this is a descriptor block. */
   uint8   Reserved1;         /**< Reserved. */
   uint8   TagId;             /**< Indicates the tag for this descriptor. */
   uint8   Reserved2;         /**< Reserved. */
   uint8   Descriptor[13];    /**< Descriptor string. @newpagetable */
} EDID_DisplayDescriptor;

/** Defines the EDID.
*/
typedef struct {
  uint8   Header[8];              /**< EDID header: 00 FF FF FF FF FF FF 00. */
  uint16  ManufactureID;          /**< EISA three-character ID. */
  uint16  ProductCode;            /**< Vendor-assigned code. */
  uint32  SerialNumber;           /**< Thirty-two-bit serial number. */
  uint8   WeekOfManufacture;      /**< Week number. */
  uint8   YearOfManufacture;      /**< Year number. */
  uint8   EdidVersion;            /**< EDID structure version. */
  uint8   EdidRevision;           /**< EDID structure revision. */
  uint8   VideoInputDefinition;   /**< Information on how the host's video
                                       outputs are to be configured to drive
                                       the attached display. */
  uint8   MaxHorizontalImageSize;  /**< Maximum horizontal size in centimeters. */
  uint8   MaxVerticalImageSize;    /**< Maximum vertical size in centimeters. */
  uint8   DisplayTransferCharacteristic;  /**< Characteristics of the display
                                               transfer. */
  uint8   FeatureSupport;          /**< Indicates which features are supported. */
  uint8   RedGreenLowBits;         /**< Rx1 Rx0 Ry1 Ry0 Gx1 Gx0 Gy1Gy0. */
  uint8   BlueWhiteLowBits;        /**< Bx1 Bx0 By1 By0 Wx1 Wx0 Wy1 Wy0. */
  uint8   RedX;                    /**< Red-x bits 9 to 2. */
  uint8   RedY;                    /**< Red-y bits 9 to 2. */
  uint8   GreenX;                  /**< Green-x bits 9 to 2. */
  uint8   GreenY;                  /**< Green-y bits 9 to 2. */
  uint8   BlueX;                   /**< Blue-x bits 9 to 2. */
  uint8   BlueY;                   /**< Blue-y bits 9 to 2. */
  uint8   WhiteX;                  /**< White-x bits 9 to 2. */
  uint8   WhiteY;                  /**< White-y bits 9 to 2. */

  EDID_EstablishedTimings EstablishedTimings;  /**< Established timings. */
  EDID_StandardTimings    StandardTiming[8];   /**< Standard timings. */
  EDID_DetailedTiming     DetailedTiming;      /**< Detailed timings. */
  
  /** Detailed descriptors. */
  union 
  {
      EDID_DetailedTiming    DetailedTimings[3];
      /**< Detailed timing descriptors. */

      EDID_DisplayDescriptor DisplayDescriptors[3];
      /**< Detailed display descriptors. */
      
  }EDID_DetailedInfo;
  
  uint8   ExtensionFlag;    /**< Number of optional 128-byte EDID extension
                                 blocks that follows. */
  uint8   Checksum;         /**< EDID checksum. */

} EDID_Config;

#pragma pack(pop)

/** @} */ /* end_addtogroup data_types_edid */

/** @addtogroup data_types_qdi
@{ */
/** Defines the generic panel command sequence.
*/
typedef struct
{
  uint32                 uReserved;    /**< Reserved. */
} QDI_Panel_CommandType;

/** Defines PMIC WLED parameters.
*/
typedef struct
{
  uint32                        uWledInternalModResolution;
  /**< Internal digital modulator configured. */

  uint32                        uWledModulationClkSel;
  /**< Digital modulator input clock frequency control. */

  uint32                        uWledDimmingMethod;
  /**< Modulator dimming mode control. */

  uint32                        uWledOvp; 
  /**< Control for the over-voltage protection threshold. */

  uint32                        uWledIlim;
  /**< Current limit after soft start is complete. */

  uint32                        uWledFeedbackCtrl; 
  /**< Forces selection of a LED output as feedback node for the boost. */

  uint32                        uWlepLoopCompRes;
  /**< Control to select the compensation resistor. */

  uint32                        uWledVrefControl;
  /**< Reference voltage for boost feedback. */

  uint32                        uWledFullScaleCurrent;
  /**< Select current sink for the LED.  */

  uint32                        uWledModulatorSrcSel;
  /**< 1 for external PWM, 0 for internal digital modulator. */

  bool32                        bWledCabcEnable; 
  /**< Enable content adaptive backlight control for current sinks. */
  
  uint32                        uAvddVoltage;
  /**< Default WLED AVDD Voltage in mV, OLED only. */

}QDI_Panel_PMIC_WledConfigType;

/** Defines the backlight driver.

    @note1hang If bSetAdvanceConfig is set, all parameters of this structure must
    be set to the proper values.
*/
typedef struct
{
  QDI_PMICDeviceIdType          eModel;
  /**< PMIC model number. */

  uint32                        uPMICNum;
  /**< PMIC chip number. This is set to zero if it is a single PMIC chip.
       When present, the PMIC interface (PMI) module is typically chip ID 1. */

  QDI_PmicModuleControlType     eControlType;
  /**< PMIC backlight control type, such as WLED or LPG. */  

  uint32                        uBankSelect;
  /**< Values for the Light Pulse Generator (LPG) bank and White Light Emitting Diode (WLED).

       @values{for LPG} 0 to 7 for bank number

       @values{for WLED}
       - Bit 0 -- string 1 
       - Bit 1 -- string 2
       - Bit 2 -- string 3 @tablebulletend */
       
  uint32                        uPWMSize;
  /**< PWM size in bits. The value 0 defaults to a 9-bit PWM. */

  uint32                        bEnGlitchRemoval;
  /**< Glitch removal. */

  uint32                        uPWMFrequency;
  /**< PWM frequency requested by the panel's backlight controller.
       @newpagetable */

  bool32                        bSetAdvancedConfig;
  /**< Sets advanced PMIC configuration for backlight control. */

  /** Advanced configuration. */   
  union
  {
    QDI_Panel_PMIC_WledConfigType sWledConfig;
    /**< WLED-specific configuration parameters.*/

  /* other QDI_PmicModuleControlType advance configuration */
  } uAdvancedConfig;

} QDI_Panel_PMIC_ConfigType;


/** Defines the backlight-specific settings.
*/
typedef struct
{
  QDI_Panel_BacklightType        eBacklightType;
  /**< Backlight type. */

  /** Backlight control methods. */
  union {
    QDI_Panel_PMIC_ConfigType    sPMICConfig;
    /**< Configuration for PMICs. */

    QDI_Panel_CommandType        sDSIConfig;
    /**< Configuration for command panels. @newpagetable */
  } uCntrl;

  uint32                         uStepVal;
  /**< Step value; backlight adjustment granularity as a percent (e.g., 
       10%&nbsp;=&nbsp;10 discrete values). */

  uint32                         uLowPowerVal;
  /**< Default backlight value for Low Power mode (0% to 100%). */

  uint32                         uDefaultVal;
  /**< Default backlight value (0% to 100%). */
  
} QDI_Panel_Backlight_ConfigType;


/** Timing information for active panels.
*/
typedef struct
{
    /* Horizontal fields */
    uint32     iHsyncPulseWidthDclk;
    /**< Horizontal synchronization pulse width in dot_clks. */

    uint32     iHsyncBackPorchDclk;
    /**< Left edge including horizontal synchronization pulse width. */

    uint32     iHsyncFrontPorchDclk;
    /**< Right edge of an inactive LCD. */

    uint32     iHsyncSkewDclk;
    /**< Delay horizontal synchronization with respect to vertical
         synchronization. */

    uint32     iHLeftBorderDClk;
    /**< Number of left border pixels. */

    uint32     iHRightBorderDClk;
    /**< Number of right border pixels. */    

    /* Vertical fields */    
    uint32     iVsyncPulseWidthLines;
    /**< Vertical synchronization pulse width in lines. */

    uint32     iVsyncFrontPorchLines;
    /**< Blanking period before hardware vertical synchronization. */

    uint32     iVsyncBackPorchLines;
    /**< Blanking period after hardware vertical synchronization. */

    uint32     iVTopBorderLines;
    /**< Number of top border lines. */

    uint32     iVBottomBorderLines;
    /**< Number of bottom border lines. */

    /* Misc fields */
    uint32     iBorderColorRgb888;
    /**< Border color used when bordering an active region. */
    
    uint32     iUnderflowColorRgb888;
    /**< Color produced when an underflow occurs. */

    bool32     bInterlaced;
    /**< Interlace mode. */

    /* Signal polarity fields */
    bool32     bIsHsyncActiveLow;
    /**< Inverse horizontal synchronization control signal. */

    bool32     bIsVsyncActiveLow;
    /**< Inverse vertical synchronization control signal. */

    bool32     bIsDataEnableActiveLow;
    /**< Inverse data enable signal. */

    uint32     uInterfaceEnableDelay;
    /**< Delay in milliseconds -- Required after the function is enabled. @newpagetable */

} QDI_Panel_ActiveTimingType;

/** Defines the audio parameters and HDMI packet information.
*/
typedef struct
{
  bool32                           bAudioInfoPacket;
  /**< Enables/disables an Audio InfoFrame packet. */

  bool32                           bInhibitDownMix;
  /**< Enables/disables audio output downmixing. */

  bool32                           bReserved[4];
  /**< Reserved for future use. */

  QDI_HDMI_AudioSampleRateType     eAudioSampleRate;
  /**< Audio sample rate. */

  QDI_HDMI_AudioChannelNumType     eAudioChannelNum;
  /**< Audio channel number. */

  QDI_HDMI_AudioFormatType         eAudioFormat;
  /**< Audio format. */

  QDI_HDMI_AudioSampleBitDepthType eAudioSampleBitDepth;
  /**< Audio bit depth. */

  QDI_HDMI_AudioChannelAllocType   eChannelAllocCode;
  /**< Audio channel-to-speaker mapping. */

  QDI_HDMI_AudioLevelShiftType     eLevelShiftValue;
  /**< Audio attenuation level. */

  uint32                           uReserved[6];
  /**< Reserved for future use. */

} QDI_Panel_HdmiAudioAttrType;


/** Defines the vendor-specific parameters from a vendor-specific data block
    in the EDID.
*/
typedef struct
{
  bool32                                  bVideoOnly;
  /**< Specifies whether the HDMI display supports DVI mode (video only) or
       HDMI mode (audio and video). */

  QDI_HDMI_LipSyncInfoType                sLipSyncInfo;
  /**< Video and audio latency information. */

  uint8                                   uSupportedDeepColorModeMask;
  /**< Pixel depth for each Deep Color mode that the HDMI display can support
       (QDI_HDMI_DeepColorModeType). */

  uint8                                   uSupportedContentFilterMask;
  /**< Specifies whether the sink supports content filters
       (QDI_HDMI_ContentFilterType). */

  uint16                                  uPhysicalAddr; 
  /**< Sixteen-bit physical address of the source device; the upper 16 bits are
       ignored. */

  bool32                                  bAISupport; 
  /**< Specifies whether ACP, ISRC1, and ISRC2 packets are supported. */

  bool32                                  bDVIDualSupport;
  /**< Specifies whether the sink supports DVI Dual mode. */

  QDI_Display_ModeAspectRatioType         ePreferredAspectRatio; 
  /**< Preferred aspect ratio reported by the sink. */

  uint32                                  uMaxScreenWidthInMillimeter;
  /**< Maximum screen width in millimeters reported by the sink. */

  uint32                                  uMaxScreenHeightInMillimeter;
  /**< Maximum screen height in millimeters reported by the sink. */

  uint8                                   uManufacturerId[2];  
  /**< Manufacturer ID. Using the EISA ID, uManufacturerId[0] is the low
       byte. */

  uint16                                  uProductId;
  /**< Product code used to differentiate between different models from the
       manufacturer. */

  uint8                                   uNameDescriptionLength;
  /**< Sink name description length. */

  uint8                                   uNameDescription[13];
  /**< Sink name description. */

  uint32                                  uReserved[2];
  /**< Reserved for future use. */

} QDI_Panel_HdmiVendorAttrType;


/** Defines the HDMI parameters.
*/
typedef struct
{
  bool32                                  bDisableHDMI;
  /**< HDMI core is disabled or not available. */
  
  uint32                                  uHDMIModeId;
  /**< HDMI-specific mode enumeration. */

  uint32                                  uAttributeType;
  /**< Indicates which attributes to configure. */

  bool32                                  bContentProtectionEnable;
  /**< Enable HDCP in QDI and do not send HDCP interrupt to the KMD. */

  uint32                                  uMaxNumReAuthentication;
  /**< Maximum number of retries if authentication fails. */

  QDI_Panel_HdmiAudioAttrType             sHdmiAudioAttr;
  /**< HDMI audio attributes. */

  QDI_Panel_HdmiVendorAttrType            sHdmiVendorSpecificAttr;
  /**< HDMI vendor-specific attributes. */

  QDI_Display_HDMI_AVIPacketPropType      sHdmiAVIPacketInfo; 
  /**< HDMI AVI packet information. */

  uint32                                  uHDMIPixelClockFrequency;
  /**< HDMI pixel clock frequency. */

  bool32                                  bDisableAviInfoFramePacket;
  /**< HDMI AVI infoframe packet is disabled. */

  bool32                                  bEnableOutVoltageSwingCtrl;
  /**< Enables the uOutVoltageSwingCtrl overwrite. */

  uint32                                  uOutVoltageSwingCtrl;
  /**< HDMI PHY output voltage swing control value from 0 to 7. @newpagetable */

   QDI_Display_HDCPLevelType              eHDCPLevel;
  /**< HDCP level to negotiate with sink. */

}QDI_Panel_HdmiAttrType;


/** Defines the DSI Tear Enable (TE) configuration.
*/
typedef struct
{
    bool32                        bTECheckEnable;
    /**< Enables the MDP tear check block. */

    bool32                        bDedicatedTEPin;
    /**< Indicates whether an external TE pin or an embedded TE pin is used. */

    /* MDP Vsync TE block configuration */
    uint32                        vSyncStartPos;
    /**< Line number from which the TE kick-off condition is evaluated for
         vertical synchronization. */

    uint32                        vSyncContinueLines;
    /**< Minimum number of lines the write pointer must be above the read
         pointer to continue line updates. */

    uint32                        vSyncStartLineDivisor;
    /**< Indicates the part of visible height to be used as the start
         threshold for vertical synchronization. */

    uint32                        vSyncPosSafetyMargin;
    /**< Optimizes the start position value for vertical synchronization. */

    uint32                        vSyncBelowSafetyMargin;
    /**< Optimizes the start threshold value for vertical synchronization. */

    uint32                        vSyncPercentVariance;
    /**< Percent by which the refresh rate is to be reduced to increase the
         vertical synchronization count. */

    uint32                        vSyncRdPtrIrqLine;
    /**< Read pointer to the IRQ line request for displaying vertical
         synchronization. */

    QDI_ExternalVsyncSelectType   vSyncSelect;
    /**< External Vsync source select. */

    uint32                        uVsyncInitValue;
    /**< Specify the init value to which the read pointer gets loaded at vsync edge */

    /* Reserved fields for future use */
    uint32                        uTEMode;                /**< Reserved. */
    uint32                        uTEPolarity;            /**< Reserved. */
    uint32                        uTEHSyncTriggerCount;   /**< Reserved. */
    uint32                        uTEMinVSyncWidth;       /**< Reserved. */
    uint32                        uTEMaxVSyncWidth;       /**< Reserved. */

} QDI_Panel_DSITEInfo;

/** Define the panel command sequence format.
*/
typedef struct 
{
    uint32                   uPacketLength;
    /**< Size of the command sequence pointed by pPacketData in Bytes. */
    uint8                   *pPacketData;
    /**< Pointer to buffer containing the command sequence. */

} QDI_Panel_SequenceFormatType;

/** Define the panel command sequence parameters.
*/
typedef struct 
{
    QDI_Panel_SequenceFormatType   *pSequence;      
    /**< Pointer to the sequence to be executed. */
    uint32                          uBufferLength;
    /**< Size of the sequence buffer length in Bytes. */

} QDI_Panel_SequenceInfo;

/** DSI settings for Video and Command modes.
*/
typedef struct
{
    /* Common DSI Configuration */
    uint32                   uRefreshRate;
    /**< Panel refresh rate in Q16.16 representation. */

    uint32                   uBitClockFrequency;
    /**< DSI bit clock frequency in Hertz (overrides uRefreshRate). */

    uint32                   uBitClkScalePercent;
    /**< Percent by which the calculated bitclk value is to be incremented. */

    QDI_DSIChannelIDType     eChannelId;
    /**< Display channel ID. */

    QDI_DSIDisplayVCType     eDisplayVC;
    /**< Virtual channel ID.*/

    QDI_DSIColorFormatType   eColorFormat;
    /**< Panel color format. */

    uint32                   uDataLaneNum;
    /**< Number of data lanes used. */

    QDI_DSILaneMapType       uLaneRemap;
    /**< Configuration of DSI host lanes connectivity to the panel. */

    bool32                   bLP11AtInit;
    /**< Initializes the DSI core to LP11 during reset. */

    uint32                   uDSITransferRetryCntr;
    /**< DSI send/read command transfer retry counter on failure */

    /* DSI LS/HS Configuration. */
    bool32                   bPacketTransferHS;
    /**< Forces command transfers during high-speed operations. */
    
    QDI_DSI_ClockLaneType    eClockHSForceRequest;
    /**< Controls clock lane configuration. */

    bool32                   bDisableEotpAfterHSXfer;
    /**< Disables appending EoTp at the end of each forward high-speed data burst (default is enabled). */
                                                              
    /* DSI Timing parameter overrides. */
    bool32                   bTimingHSZeroOverride;
    /**< Timing override for THS_Zero. */

    uint32                   uTimingHSZeroValue;
    /**< THS_Zero value. */

    bool32                   bTimingHSExitOverride;
    /**< Timing override for THS_Exit. */

    uint32                   uTimingHSExitValue;
    /**< THS_Exit value. */

    bool32                   bTimingHSPrepareOverride; 
    /**< Timing override for THS_Prepare. */

    uint32                   uTimingHSPrepareValue;    
    /**< THS_Prepare value. */

    bool32                   bTimingHSTrailOverride;   
    /**< Timing override for THS_Trail. */

    uint32                   uTimingHSTrailValue;      
    /**< THS_Trail value. */

    bool32                   bTimingHSRequestOverride; 
    /**< Timing override for THS_Request. */

    uint32                   uTimingHSRequestValue;    
    /**< THS_Request value. */

    bool32                   bTimingCLKZeroOverride;   
    /**< Timing override for TClk_Zero. */

    uint32                   uTimingCLKZeroValue;      
    /**< TClk_Zero value. */

    bool32                   bTimingCLKTrailOverride;  
    /**< Timing override for TClk_Trail. */

    uint32                   uTimingCLKTrailValue;     
    /**< TClk_Trail value. */    

    bool32                   bTimingCLKPrepareOverride;
    /**< Timing override for TClk_Prepare. */

    uint32                   uTimingCLKPrepareValue;   
    /**< TClk_Prepare value. */

    bool32                   bTimingCLKPreOverride;    
    /**< Timing override for TClk_Pre. */

    uint32                   uTimingCLKPreValue;       
    /**< TClk_Pre value. */

    bool32                   bTimingCLKPostOverride;   
    /**< Timing override for TClk_Post. */

    uint32                   uTimingCLKPostValue;      
    /**< TClk_Post value. */

    bool32                   bTimingTAGoOverride;      
    /**< Timing override for TTA_Go. */    

    uint32                   uTimingTAGoValue;         
    /**< TTA_Go value. */    

    bool32                   bTimingTASureOverride;    
    /**< Timing override for TTA_Sure. */

    uint32                   uTimingTASureValue;       
    /**< TTA_Sure value. */

    bool32                   bTimingTAGetOverride;     
    /**< Timing override for TTA_Get. */

    uint32                   uTimingTAGetValue;        
    /**< TTA_Get value. */

    uint32                   uInitMasterTime;          
    /**< DSI initialization master time in milliseconds (init_master_time >= (init_slave_time + internal_delay). */

    /* Video Mode specific configuration. */
    QDI_DSITrafficModeType   eTrafficMode;             
    /**< DSI Video Traffic mode. */  

    bool32                   bSendHsaHseAfterVsVe;     
    /**< Sends HSA and HSE following VSA/VSE. */

    bool32                   bLowPowerModeInHFP;       
    /**< LP11 is in HFP period.  */

    bool32                   bLowPowerModeInHBP;       
    /**< LP11 is in HBP period,*/

    bool32                   bLowPowerModeInHSA;       
    /**< LP11 is in HSA period. */

    bool32                   bLowPowerModeInBLLPEOF;   
    /**< LP11 is in BLLP period at the end frame. */

    bool32                   bLowPowerModeInBLLP;      
    /**< LP11 is in BLLP period. */

    bool32                   bLastLineInterleave;      
    /**< Command mode operation interleaved on last line of video stream. */

    uint32                   uDmaDelayAfterVsync;      
    /**< Delay time (in milliseconds) to send packets after Vsync completion. */

    bool32                   bForceCmdInVideoHS;       
    /**< Deprecated -- Use ePixelTxTiming instead. */

    QDI_DSI_PixelTxTiming    ePixelTxTiming;           
    /**< Define the scenarios to enable or disable the pixel data transfer in video mode configuration. */

    /* Command Mode specific configuration. */
    bool32                   bSwapInterface;           
    /**< Maps the primary DSI to the secondary interface. */

    bool32                   bUsingTrigger;            
    /**< Enables the trigger for this interface. */

    QDI_Panel_DSITEInfo      sTE;                      
    /**< DSI Tear Enable configuration. */

    int32                    iSlaveControllerSkewLines;
    /**< Number of skewed lines between two controllers for a dual-DSI use-case.
         - iSlaveControllerSkewLines >  0 -- the master controller starts the data
           transfer ahead of slave controller.
         - iSlaveControllerSkewLines == 0 -- the master and slave controllers
           start the data transfer at approximately the same time
         - iSlaveControllerSkewLines <  0 -- the master controller starts the data
           transfer after slave the controller @tablebulletend */

    bool32                   bPhyDCDCMode; 
    /**< Specifies the DSI Regulator mode.
        - TRUE  -- DCDC mode
        - FALSE -- LDO mode (Default) @tablebulletend */
   
    uint32                   uCommandModeIdleTime;
    /**< Idle time to be inserted between command-mode MDP packets. */

    /* Refresh rate Configuration */
    uint32                   uNumberofRefreshRates;
    /**< Number of dynamic refresh rates. */

    uint32                   aDynamicRefreshRates[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];        
    /**< List of dynamic refresh rates for this panel. */

    bool32                   bEnterULPSPowerDown;
    /**< Transition to DSI ULPS mode while powering-down. */

    /* DSI Controller Mapping Mode*/
    uint8                    uControllerMapping[4];
    /**< Define which DSI controller(s) to be used for this DSI panel. */

    uint32                   uControllerMappingMask;
    /**< Bit msk for uControllerMapping -- bit0 is for dsi0,  bit1 is for dsi1. */ 
    
    /* DSI Escape clock configuration */
    uint32                   uEscapeClockDivisor;
    /**< Divisor when escape clock is sourced from the PLL, default is 0, source from CXO. */

    uint32                   uEscapeClockFreq;
    /**< Escape clock frequency in Hz for timing adjustments, default is 0, CXO frequency. */
    
    /* DSI Strength control */
    uint32                   uDataStrengthLP; 
    /**< Drive strength of the LP data signal, chipset specific, leave as zero for default. */

    uint32                   uDataStrengthHS;
    /**< Drive strength of the HS data signal, chipset specific, leave as zero for default. */    

    uint32                   uClockStrengthHS;
    /**< Drive strength of the HS clock signal, chipset specific, leave as zero for default. @newpagetable */    

    uint32                   uPhyLDORefVoltageID;
    /**< DSI LDO regulator reference voltage ID configuration. */

    uint32                   uNullPacketInsertionLength;
    /**< Word count of the auto-generated NULL packet. */

    QDI_Panel_DSI_FlagsType  uFlags;
    /**< Miscellaneous DSI flags. See #QDI_Panel_DSI_FlagsType. */

    QDI_DSI_PHY_SSCConfig    ePhySSCConfig;
    /**< DSI PHY spread spectrum clocking (SSC) configuration. */

    QDI_Panel_SequenceInfo   sPanelSequences[QDI_PANEL_SEQUENCE_MAX];
    /**< Panel command sequence. */

    uint32                   uTransferRate;
    /**< Transfer rate based on the bitclock frequency, in Hz */

} QDI_Panel_DSIAttrType;


/** Panel color correction matrix type.
*/
typedef struct
{
    QDI_Display_IDType  eDisplayId;      
    /**< ID of the display to which this color correction is applied. */

    bool32              bEnabled;        
    /**< Flag to enable or disable color correction. */

    int64               sCCMatrix[QDI_PANEL_CC_MATRIX_COMPONENTS][QDI_PANEL_CC_MATRIX_COEFFICIENTS];
    /**< Color correction matrix -- Passes the color correction coefficients
         in the panel color correction buffer in order.
         
         @note1 Any changes to the panel color correction order yields unpredictable results.

         Color coefficient indexes are defined as follows:
         - Index 0 -- Red color coefficients
         - Index 1 -- Green color coefficients
         - Index 2 -- Blue color coefficients

         @vertspace{3}

         <strong>QDI_PANEL_PCC_SUPPORT_TYPE_MATRIX_3X11 matrix</strong>

         QDI_PANEL_PCC_SUPPORT_TYPE_MATRIX_3X11 matrix for the following components:
            - Red coefficients   = a01 + a11 Rd + a21 Gd + a31 Bd + a41 RdGd + 
                                   a51 GdBd + a61 BdRd + a71 R2d + a81 G2d + a91 B2d + a101 RdGdBd_0 
            - Green coefficients = a02 + a12 Rd + a22 Gd + a32 Bd + a42 RdGd +
                                   a52 GdBd + a62 BdRd + a72 R2d + a82 G2d + a92 B2d + a102 RdGdBd_0 
            - Blue coefficients  = a03 + a13 Rd + a23 Gd + a33 Bd + a43 RdGd +
                                   a53 GdBd + a63 BdRd + a73 R2d + a83 G2d + a93 B2d + a103 RdGdBd_0 
         @vertspace{3}

         The following is the correct order of the coefficients for QDI_PANEL_PCC_SUPPORT_TYPE_MATRIX_3X11:
          - sCCMAatrix[0][0]  = Constant
          - sCCMAatrix[0][1]  = Red           
          - sCCMAatrix[0][2]  = Green        
          - sCCMAatrix[0][3]  = Blue      
          - sCCMAatrix[0][4]  = Red   * Green
          - sCCMAatrix[0][5]  = Green * Blue
          - sCCMAatrix[0][6]  = Blue  * Red
          - sCCMAatrix[0][7]  = Red   * Red
          - sCCMAatrix[0][8]  = Green * Green
          - sCCMAatrix[0][9]  = Blue  * Blue
          - sCCMAatrix[0][10] Upper32 = Red * Green * Blue (Sign + 7 MSB bits)
          - sCCMAatrix[0][10] Lower32 = Red * Green * Blue (32 LSB bits) 

         @contcell
         <strong>QDI_PANEL_PCC_SUPPORT_TYPE_MATRIX_3X8 matrix</strong>

         QDI_PANEL_PCC_SUPPORT_TYPE_MATRIX_3X8 matrix for the following components: \n
          R' = R * (a11 + a17 * G) + G * (a12 + a18 * B) + B * \n
          @indent (a13 + a19 * R) + a110 * R * G * B + c1

          G' = R * (a21 + a27 * G) + G * (a22 + a28 * B) + B * \n
          @indent (a23 + a29 * R) + a210 * R * G * B + c2

          B' = R * (a31 + a37 * G) + G * (a32 + a38 * B) + B * \n
          @indent (a33 + a39 * R) + a310 * R * G * B + c3

         @vertspace{3}
         The following is the correct order of the coefficients for QDI_PANEL_PCC_SUPPORT_TYPE_MATRIX_3X8:
          - sCCMAatrix[0][0]  = Constant
          - sCCMAatrix[0][1]  = Red
          - sCCMAatrix[0][2]  = Green
          - sCCMAatrix[0][3]  = Blue
          - sCCMAatrix[0][4]  = Red * Green
          - sCCMAatrix[0][5]  = Red * Blue
          - sCCMAatrix[0][6]  = Blue * Green
          - sCCMAatrix[0][7]  = Red * Blue * Green
          - sCCMAatrix[0][8]    Reserved, must be zero
          - sCCMAatrix[0][9]    Reserved, must be zero 
          - sCCMAatrix[0][10]   Reserved, must be zero

         @tablebulletend @newpagetable */

} QDI_Panel_ColorCorrectionMatrixType;


/** Panel dithering matrix.

    Set the coefficients for the 4x4 dithering matrix so that each element
    of the #uDitheringMatrix is a row in the matrix.
   @verbatim
uDitheringMatrix[n] = Element (n,0) bits [15:12] | Element (n,1) bits [11:8] |
                      Element (n,2) bits [7:4]   | Element (n,3) bits [3:0]

- uDitheringMatrix[0]  = Element (0,0) | Element (0,1) | Element (0,2) | Element (0,3)
- uDitheringMatrix[1]  = Element (1,0) | Element (1,1) | Element (1,2) | Element (1,3)
- uDitheringMatrix[2]  = Element (2,0) | Element (2,1) | Element (2,2) | Element (2,3)
- uDitheringMatrix[3]  = Element (3,0) | Element (3,1) | Element (3,2) | Element (3,2)
   @endverbatim
   @vertspace{12}
*/
typedef struct
{
    QDI_Display_IDType             eDisplayId;      
    /**< ID of the display to which the dithering is applied. */

    bool32                         bEnabled;
    /**< Flag to enable or disable dithering. */

    QDI_Panel_DitheringModeType    eMode;
    /**< Flag to set dithering mode. */

    uint32                         uBitDepth[QDI_PANEL_DITHER_BITDEPTH];
    /**< Bit reduction strength for color components:
         - uBitDepth[0] = Color 0 (Green)
         - uBitDepth[1] = Color 1 (Blue)
         - uBitDepth[2] = Color 2 (Red)

         Valid values represent bit reduction strength as follows:
         - 0x0 -- Bit reduction strength is 4 bits
         - 0x1 -- Bit reduction strength is 3 bits
         - 0x2 -- Bit reduction strength is 2 bits
         - 0x3 -- Bit reduction strength is 0 bits @tablebulletend */

    uint32                         uDitheringMatrix[QDI_PANEL_DITHER_MATRIX_ROWS];

    /**< Dithering 4x4 matrix.
    @tablebulletend @newpagetable */

} QDI_Panel_DitheringMatrixType;



/** Panel gamma correction segment curve parameters.
*/
typedef struct
{
    bool32       bEnabled;   /**< Flag to indicate whether this segment is
                                  enabled; currently not used. */
    int32        start;      /**< Start position of the segment. */
    int32        gain;       /**< Gain or slope of the segment. */
    int32        offset;     /**< Offset of the segment. */
} QDI_Panel_GammaCorrectionSegment;


/** Panel gamma correction matrix for three color components.
*/
typedef struct
{
    QDI_Display_IDType                  eDisplayId;
    /**< ID of the display to which this gamma correction is applied. */

    bool32                              bEnabled;
    /**< Flag to enable or disable panel gamma correction. */

    QDI_Panel_GammaCorrectionSegment    sRedCr[QDI_PANEL_PGC_SEGMENTS];
    /**< Panel gamma correction parameters for color 0. */

    QDI_Panel_GammaCorrectionSegment    sGreenY[QDI_PANEL_PGC_SEGMENTS];
    /**< Panel gamma correction parameters for color 1. */

    QDI_Panel_GammaCorrectionSegment    sBlueCb[QDI_PANEL_PGC_SEGMENTS];
    /**< Panel gamma correction parameters for color 2. @newpagetable */

} QDI_Panel_GammaCorrectionTableType;

/** Panel linear gamma correction table settings.
*/
typedef struct
{
    QDI_Display_IDType                  eDisplayId;
    /**< ID of the display to which this gamma correction is applied. */

    bool32                              bEnable;
    /**< Flag to enable or disable panel gamma correction. */

    uint32                              uNumCoefficients;
    /**< Number of elements in each gamma correction vector. */

    uint32                              uCoefficientSizeInBytes;
    /**< Gamma correction element size in bytes. */

    void                                *pRedCr;
    /**< Gamma component for Red/Cr. */

    void                                *pGreenY;
    /**< Gamma component for Green/Y. */

    void                                *pBlueCb;
    /**< Gamma component for Blue/Cb. */
} QDI_Panel_LinearGammaCorrectionTableType;

/** Panel inverse gamma correction table for three color components.
*/
typedef struct
{
    QDI_Display_IDType  eDisplayId;
    /**< ID of the display to which this inverse gamma correction table is applied. */

    bool32              bEnabled;
    /**< Flag to enable or disable inverse gamma correction. */

    uint32              uRedCr[QDI_PANEL_GC_MAX_COMPONENT_ENTRIES];
    /**< Gamma data for Red/Cr. */

    uint32              uGreenY[QDI_PANEL_GC_MAX_COMPONENT_ENTRIES];
    /**< Gamma data for Green/Y. */

    uint32              uBlueCb[QDI_PANEL_GC_MAX_COMPONENT_ENTRIES];
    /**< Gamma data for Blue/Cb. */

} QDI_Panel_InverseGammaCorrectionTableType;

/** Panel gamut mapping table for color components.

    The pSegmentsRedCr, pSegmentsGreenY, pSegmentsBlueCb parameters point to the
    segment tables. Each element includes offset and scale fields as follows:
    - Offset --  Bits  0:11 format U12
    - Scale  --  Bits 12:28 format U17
*/
typedef struct
{
    QDI_Display_IDType               eDisplayId;
    /**< ID of the display to which this gamut mapping table is applied. */

    bool32                           bEnabled;
    /**< Flag to enable/disable gamut mapping. */

    QDI_Panel_GamutMappingModeType   eMode;
    /**< Flag to set the gamut mapping mode. */

    bool32                           bGamutBeforePCCReserved;
    /**< Flag to indicate the gamut mapping order and polynomial color correction.
         
         @values 
         - TRUE -- Gamut mapping followed by polynomial correction.
         - FALSE -- Polynomial correction followed by gamut mapping. 

         @note1 Reserved. By default, gamut mapping takes place before
                color correction. */

    uint32                           uNumSamplesPerColor;
    /**< Number of samples per color component in the gamut mapping table. */

    uint16                          *pRedCr;
    /**< Gamut mapping data for Red/Cr. \n 
         size of entries = uNumSamplesPerColor ^ 3 */

    uint16                          *pGreenY;
    /**< Gamut mapping data for Green/Y. \n 
         size of entries = uNumSamplesPerColor ^ 3 */

    uint16                          *pBlueCb;
    /**< Gamut mapping data for Blue/Cb. \n 
         size of entries = uNumSamplesPerColor ^ 3 */

    uint32                           uNumSegmentsPerColor;
    /**< Number of segments per color component. 
    
         uNumSegmentsPerColor = uNumSamplesPerColor -1 */

    void                            *pSegmentsRedCr;
    /**< Segments the data of Red/Cr in non-uniform gamut mapping.
         Only required by non-uniform mode. */

    void                            *pSegmentsGreenY; 
    /**< Segments the data of Green/Y in non-uniform gamut mapping.
         Only required by non-uniform mode. */

    void                            *pSegmentsBlueCb;
    /**< Segments the data of Blue/Cb in non-uniform gamut mapping.
         Only required by non-uniform mode. @newpagetable */

} QDI_Panel_GamutMappingTableType;


/** Panel histogram configuration.
*/
typedef struct
{
    QDI_Display_IDType  eDisplayId;
    /**< ID of the display to which the histogram configuration is applied. */

    bool32              bEnabled;                
    /**< Enable/disable the histogram collection.
    
         When enabled, histogram data is collected periodically over the number
         of frames specified. When a collection is completed, the event
         notification QDI_PANEL_EVENT_HIST_DATA_AVAIL is sent together with the
         collected histogram data. */
    
    uint32              uFrameCnt;
    /**< Number of frames over which histogram data is collected. */

} QDI_Panel_HistogramConfigurationType;


/** Panel histogram data.
*/
typedef struct
{
    QDI_Display_IDType  eDisplayId;
    /**< ID of the display from which the histogram data is read. */
    
    uint32              uFrameCnt;
    /**< Number of frames over which histogram data is collected. */

    uint32              aHistData[QDI_PANEL_HISTOGRAM_BINS];
    /**< Histogram data read from the histogram bins. @newpagetable */
} QDI_Panel_HistogramDataType;


/** eDP link training type.
*/
typedef enum {
  QDI_EDP_LINKTRAININGTYPE_NO_TRAINING,   /**< No link training.   */
  QDI_EDP_LINKTRAININGTYPE_FULL_TRAINING, /**< Full link training. */
  QDI_EDP_LINKTRAININGTYPE_FAST_TRAINING, /**< Fast link training. */
  QDI_EDP_LINKTRAININGTYPE_MAX,           /**< Enumeration marker for the max. */
  QDI_EDP_LINKTRAININGTYPE_FORCE_32BIT    = 0x7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_EdpLinkTrainingType;


/** Configuration for the eDP interface coming from the panel configuration.
*/
typedef struct {
  uint32                            uRefreshRate;
  /**< eDP refresh rate in Q16.16 representation. */

  uint32                            uPixelClockFrequency;
  /**< Pixel clock frequency in Hertz. Used to override #uRefreshRate. */

  uint32                            uNumberOfLanes;
  /**< Number of eDP link lanes to use. */

  QDI_EdpLinkTrainingType           eLinkTraining;
  /**< Enables eDP link training. */

  bool32                            bASSREnable;
  /**< Enables an alternate scrambler seed reset. */

  bool32                            bEnhancedFrameEnable;
  /**< Enables an enhanced frame. */

  uint32                            uMaxLinkRate;
  /**< Maximum acceptable link rate. */

  bool32                            bReadDPCD;
  /**< Flag to read DPCD from the panel. */

  bool32                            bReadEDID;
  /**< Flag to read EDID from the panel. */

  uint32                            uLinkRateInKhz;
  /**< Link rate in kHz. */

  bool32                            bHPDActiveLow;
  /**< Indicates that a hot plug detected from the panel is active when the
       signal is low. */ 

  uint32                            uNumberofRefreshRates;
  /**< Number of dynamic refresh rates. To be deprecated (retained for backward
       compatibility with bluegdr).*/
  
   uint32                            aDynamicRefreshRates[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< List of dynamic refresh rates for eDP. To be deprecated (retained for
       backward compatibility with bluegdr). */

  uint32                            uPowerupWaitinMs;
  /**< Wait time between panel power up and auxiliary operation. */

  uint32                            uMaxAuxRetry;
  /**< Number of retries before auxiliary operation success. */

  uint32                            uLaneSwingLevel;
  /**< Lane voltage swing level. */

  uint32                            uPreemphasisLevel;
  /**< Lane pre-emphasis level. */

  bool32                            bEnableSSCMode;
  /**< Enables SSC mode. */

} QDI_Panel_EDPAttrType;

/** DP cable connection status.
*/
typedef enum
{
  QDI_DP_CONNECTION_POLARITY_DISCONNECTED,
  /**< Disconnected. */

  QDI_DP_CONNECTION_POLARITY_POLARITY_DEFAULT,
  /**< Connected in normal polarity. */

  QDI_DP_CONNECTION_POLARITY_POLARITY_REVERSE,
  /**< Connected in reverted polarity. */

  QDI_DP_CONNECTION_POLARITY_MAX,
  /**< Enumeration marker for the max. */

  QDI_DP_CONNECTION_POLARITY_32BIT       = 0x7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_DP_ConnectionPolarityType;

/** DP cable connection pin assignment.
*/
typedef enum
{
  QDI_DP_CONNECTION_PINASSIGNMENT_INVALID,
  /**< Not connected */

  QDI_DP_CONNECTION_PINASSIGNMENT_DFPD_A,
  /**< Downstream Facing Port DP Pin A */

  QDI_DP_CONNECTION_PINASSIGNMENT_DFPD_B,
  /**< Downstream Facing Port DP Pin B */

  QDI_DP_CONNECTION_PINASSIGNMENT_DFPD_C,
  /**< Downstream Facing Port DP Pin C */

  QDI_DP_CONNECTION_PINASSIGNMENT_DFPD_D,
  /**< Downstream Facing Port DP Pin D */

  QDI_DP_CONNECTION_PINASSIGNMENT_DFPD_E,
  /**< Downstream Facing Port DP Pin E */

  QDI_DP_CONNECTION_PINASSIGNMENT_DFPD_F,
  /**< Downstream Facing Port DP Pin F */

  QDI_DP_CONNECTION_PINASSIGNMENT_MAX,
  /**< Enumeration marker for the max. */

  QDI_DP_CONNECTION_PINASSIGNMENT_32BIT   = 0X7FFFFFFF
  /**< Forces the enumeration to 32 bits. */

} QDI_DP_ConnectionPinAssignment;

/** DP diagnostic logging configuration
*/
typedef enum
{
   QDI_DP_DIAGNOSTICLEVEL_0 = 0,    
   /**< Default no additonal diagnostic logging */
   
   QDI_DP_DIAGNOSTICLEVEL_1 = 1,
   /**< Source/Sink CRC */

   QDI_DP_DIAGNOSTICLEVEL_2 = 2,
   /**< Sink PSR Status and Sink Link Status */
   
   QDI_DP_DIAGNOSTICLEVEL_3 = 3,
   /**< Sink EDID/DPCD logging */
   
} QDI_DP_DiagnosticLevelType;

/*
 * DP HPD Notification type.
 */
typedef enum 
{
    QDI_DP_HPD_NOTIFICATION               = 0x00000001,
    /**< HPD notification */

    QDI_DP_HPD_REPLUG_NOTIFICATION        = 0x00000002,
    /**< HPD re-plug notification */

    QDI_DP_HPD_IRQ_NOTIFICATION           = 0x00000003,
    /**< HPD IRQ notification */
} QDI_DP_HPDNotificationType;

/** Configuration for the DP interface coming from the panel configuration.
*/
typedef struct {
  uint32                            uRefreshRate;
  /**< eDP refresh rate in Q16.16 representation. */

  uint32                            uPixelClockFrequency;
  /**< Pixel clock frequency in Hertz. Used to override #uRefreshRate. */

  uint32                            uNumberOfLanes;
  /**< Number of eDP link lanes to use. */

  QDI_EdpLinkTrainingType           eLinkTraining;
  /**< Enables eDP link training. */

  bool32                            bASSREnable;
  /**< Enables an alternate scrambler seed reset. */

  bool32                            bEnhancedFrameEnable;
  /**< Enables an enhanced frame. */

  uint32                            uMaxLinkRate;
  /**< Maximum acceptable link rate. */

  bool32                            bReadDPCD;
  /**< Flag to read DPCD from the panel. */

  bool32                            bReadEDID;
  /**< Flag to read EDID from the panel. */

  uint32                            uLinkRateInKhz;
  /**< Link rate in kHz. */

  bool32                            bHPDActiveLow;
  /**< Indicates that a hot plug detected from the panel is active when the
  signal is low. */

  uint32                            uNumberofRefreshRates;
  /**< Number of dynamic refresh rates. To be deprecated (retained for backward
  compatibility with bluegdr).*/

  uint32                            aDynamicRefreshRates[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< List of dynamic refresh rates for eDP. To be deprecated (retained for
  backward compatibility with bluegdr). */

  uint32                            uPowerupWaitinMs;
  /**< Wait time between panel power up and auxiliary operation. */

  uint32                            uMaxAuxRetry;
  /**< Number of retries before auxiliary operation success. */

  uint32                            uLaneSwingLevel;
  /**< Lane voltage swing level. */

  uint32                            uPreemphasisLevel;
  /**< Lane pre-emphasis level. */

  bool32                            bEnableSSCMode;
  /**< Enables SSC mode. */

  QDI_Panel_HdmiVendorAttrType      sExtVendorSpecificAttr;
  /**< DP EDID vendor-specific attributes. */

  uint32                            uAttributeType;
  /**< Indicates which attributes to configure. */

  bool32                            bContentProtectionEnable;
  /**< Enable/disable HDCP in QDI. */

  QDI_Display_HDCPLevelType         eHDCPLevel;
  /**< HDCP level to negotiate with sink. */

  QDI_Panel_HdmiAudioAttrType       sDPAudioAttr;
  /**< DP audio attributes. */

  QDI_DP_ConnectionPolarityType     eConnectionPolarity;
  /**< Connection polarity. */

  QDI_DP_ConnectionPinAssignment    eConnectionPinAssignment;
  /**< Connection pin assignment. */

  uint32                            uBitDepth;
  /**< Bit depth. */

  uint32                            uTestPatternType;
  /**< Test pattern type. */

  QDI_DP_DiagnosticLevelType        eDiagLevel;
  /**< Diagnostic level */
} QDI_Panel_DPAttrType;

/** @cond internal_only */
/** Configuration for the Content Adaptive Backlight Level (CABL).
*/
typedef struct {

   bool32   bEnabled;
  /**< Enables CABL on the device at boot up.
       If set to TRUE, CABL runs when the device in using battery (not charging).
       If set to FALSE, CABL remains constantly turned off.*/

  // Legacy CABL parameters set
  uint32  uMinUserLevel;
  /**< User (original) minimum backlight level.

       @values 0 to 255

       This is the lowest level that can be configured as the original
       backlight level and still keep CABL active. Any level lower than this
       deactivates CABL.

       This field is important so CABL can be configured when to deactivate
       itself (turn itself off) to save processing power when CABL savings are
       not significant. */

  uint32  uMinBacklightLevel;
  /**< Minimum backlight level while CABL is active.

       @values 0 to 255

       This is the lowest backlight level the CABL algorithm outputs even
       when other calculations give a lower level.

       This field limits CABL so it does not go too low and thus cause quality
       degradation. This number must be lower than uMinUserLevel. */

  uint32  uFilterThreshold;
  /**< Maximum backlight change on each CABL iteration during normal
       operation. This value is applied to all quality levels. To define
       a different threshold for each quality levels, use 
       uPerQualityLevelFilterThreshold.

       @values 0 to 255

       This parameter configures the maximum backlight change that can occur
       on each CABL iteration during regular CABL operation. This prevents big
       backlight changes, which might be noticeable to the user. */
  /* Note - the name of the parameter is a legacy name. */

  uint32 uPerQualityLevelFilterThreshold;
  /**< Maximum backlight change that can occur on each CABL iteration.
       Bits 0-7 for low quality level.
       Bits 8-15 for medium quality level.
       Bits 16-23 for high quality level.

       @values 0 to 255 */

  uint32  uDeactivatingSlope;
  /**< Maximum backlight change that can occur on each CABL iteration when
       deactivating CABL.

       @values 0 to 255 */

  uint32  uBacklightChangeSlope;
  /**< Maximum backlight change that can occur on each CABL iteration when
       gradually changing the original backlight level.

       @values 0 to 255 */

  uint32  uPixelDistortionRate;
  /**< Ratio of pixels to distort. 

       @values 0 to 255

       This parameter configures the ratio of pixels to be distorted by CABL.
       uPixelDistortionRate/1024*100 is the percentage of pixels to be boosted.
       */

  uint32  uBacklightScaleRatioUpperLimit;
  /**< Maximum backlight reduction ratio that is allowed to CABL. 

       @values 0 to 255

       This parameter configures the maximum backlight change ratio that CABL
       is to calculate. This makes sure that CABL applies at least a minimum 
       amout of power saving regardless of the content.*/

  uint32 uLuxEndPoint;
  /**< Ambient light level at which to disable CABL.

      @values 0 to 255

      CABL automatically switches on if ambient light is lower than uLuxEndPoint, 
      and automatically switch off if higher.*/

  uint32  uReservedLT;
  /**< Reserved parameter that defines the look up table type.
    
       This parameter is not exposed to the OEMs but is still made available 
       just in case it is needed. */

} QDI_Panel_CABLConfigType;
/** @endcond */

/** Configuration to dynamically activate or deactivate CABL.
*/

typedef struct
{
    bool32                     bActivate;
    /**< TRUE  -- Activate CABL. /n
         FALSE -- Deactivate CABL. */

    QDI_PowerSavingModeType    ePowerSave;
    /**< Reserved for future use. */

}
QDI_Panel_CABLActivateType;

/** Returns the current processing information for CABL.
*/
typedef struct
{
  uint64                     uInfoPart1;
  /**< CABL processing information. */

  uint64                     uReserved;
  /**< Reserved for future use. */
} QDI_Panel_CablProcessingInfoType;

/** Defines a response table with the X axis, Y axis, and the number of points.
*/
typedef struct
{
    uint32 aInputAxis[QDI_PANEL_RESPONSE_TABLE_MAX_LEN];
    /**< X axis of the response table. */

    uint32 aOuputAxis[QDI_PANEL_RESPONSE_TABLE_MAX_LEN];
    /**< Y axis of the response table. */

    uint32 uLength;
    /**< Length of the response table. */
} QDI_Panel_ResponseTableType;

/** @cond internal_only */
/** FOSS configuration
*     Configuration  for FOSS.
*/
typedef struct {
  bool32   bEnabled;
  /**< Enables FOSS on the device at boot up. \n
       If TRUE, FOSS can be turned on or off dynamically during runtime. \n
       If FALSE, FOSS remains constantly disabled. */

  uint32   uOpMode;
  /**< Operation Mode -- basic, contrast optimized, or contrast enhancement only. */

  uint32   uContrastStrength;
  /**< Contrast strength applied by FOSS. */

  uint32   uLuxEndPoint;
  /**< Ambient light level at which to disable FOSS.
       FOSS automatically switches on if ambient light is lower than
       uLuxEndPoint, and automatically switches it off if higher. */

  uint32   uLutUpdateDelayCount;
  /**< Adaptive LUT update delay count. */

  bool32   bGradientDetection;
  /**< Enables gradient detection */

  uint32   uGradientThreshold;
  /**< Gradient threshold (range: 0 -- 100).
       If gradient detection is enabled and calculated gradient score exceeds 
       the gradient threshold, FOSS processes the image to minimize the banding
       effect. */

  uint32   uStaticSceneThreshold;
  /**< Static scene threshold (range: 0 -- 255).
       Scene change score below this threshold will be considered as static scene. */

  QDI_Panel_ResponseTableType sPowerProfileTable;
  /**< Power profile of the current OLED panel. */

  QDI_Panel_ResponseTableType sPowerSavingRatioTable;
  /**< Power saving ratio slope. */

  QDI_Panel_ResponseTableType sLutUpdateSpeedTable;
  /**< Temporal speed slope. */

} QDI_Panel_FOSSConfigType;
/** @endcond */

/** Configuration to dynamically activate or deactivate FOSS.
*/
typedef struct
{
  bool32                     bActivate;
  /**< TRUE  -- Activate FOSS. /n
       FALSE -- Deactivate FOSS. */
} QDI_Panel_FOSSActivateType;


/** AdaptiveBrightness
*     Configuration for adaptive brightness features CABL and FOSS.
*/
typedef struct
{
  QDI_AdaptiveBrightnessFeatureType eFeature;
  /**< Adaptive brightness feature type. */

  union
  {
    QDI_Panel_CABLConfigType sCABLConfig;
    /**< CABL configuration. */

    QDI_Panel_FOSSConfigType sFOSSConfig;
    /**< FOSS configuration. @newpagetable */
  } uConfig;

} QDI_Panel_AdaptiveBrightnessType;

/** Sunlight Visibility Improvement (SVI) configuration parameters. 
*/
typedef struct
{
  bool32   bEnabled;
  /**< Enables SVI on the device at boot up.

       If TRUE, SVI can be turned on or off dynamically 
       during runtime. If FALSE, SVI remains constantly disabled. */

  uint32   uContrastStrength;
  /**< Contrast enhancement strength level. */

  uint32   uBrightnessStrength;
  /**< Brightness boosting strength level. */

  uint32   uFilterStepSizeUIMode;
  /**< Size of the SVI's incremental updates in UI mode. */

  uint32   uFilterStepSizeVideoMode;
  /**< Size of the SVI's incremental updates in video mode. */

  QDI_Panel_ResponseTableType sSensorResponseTable;
  /**< Sensor response table. */

  QDI_Panel_ResponseTableType sBacklightResponseTable;
  /**< Backlight response table. */

  uint32   uPanelReflectanceRatio;
  /**< Panel reflectance ratio. */

  uint32   uPanelPeakBrightness;
  /**< Display panel peak brightness. */

  uint32   uIndoorMaxLuxLevel;
  /**< Ambient light level during SVI's brightness calibration. */

  uint32   uIndoorMaxBrightnessLevel;
  /**< Brightness level chosen during SVI's brightness calibration. */
  
  uint32   uOutdoorMaxLuxLevel;
  /**< Ambient light level for which SVI applies maximum boosting. */

  uint32   uOutdoorMaxBrightnessLevel;
  /**< Specifies the maximum ambient light based boosting applied by SVI. */

  uint32   uLuxStartPoint;
  /**< Ambient light level at which to enable SVI.

       SVI automatically switches on if ambient light is higher than
       uLuxEndPoint, and automatically switch off if it is lower. */

  QDI_Panel_ResponseTableType sLutUpdateSpeedTable;
  /**< Temporal speed slope. */

  uint32   aPrivateData[8];
  /**< Reserved for future use. */

}QDI_Panel_SVIConfigType;


/** Defines assertive display enhancement configuration, calibration, and 
    tuning information.
 */
typedef struct
{
  bool32   bEnabled;                                                        
  /**< Enables or disables ambient enhancement. */

  uint32   uCoreID;                                                         
  /**< Reserved for future use. */

  uint32   uMaxIterations;                                                  
  /**< Number of iterations needed by the AD core to converge. */
  
  /**< AD Private Data */

  uint32   ADInitParam001[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET1_LEN];
  uint32   ADInitParam002[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET1_LEN];
  uint32   ADInitParam003[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET1_LEN];
  uint32   ADInitParam004[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET1_LEN];
  uint32   ADInitParam005[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET2_LEN];  
  uint32   ADInitParam006[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET2_LEN];
  uint32   ADInitParam007[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET1_LEN];
  uint32   ADInitParam008[QDI_PANEL_AL_ENHANCEMENT_CALIBRATION_LUT_SET1_LEN];
  uint32   ADInitParam009;
  uint32   ADInitParam010;
  uint32   ADInitParam011;
  uint32   ADInitParam012;
  uint32   ADInitParam013;
  uint32   ADInitParam014;
  uint32   ADInitParam015;
  uint32   ADInitParam016;
  uint32   ADInitParam017;
  uint32   ADInitParam018;
  uint32   ADInitParam019;
  uint32   ADInitParam020;
  uint32   ADInitParam021;
  uint32   ADInitParam022;
  uint32   ADInitParam023;
  uint32   ADInitParam024;
  uint32   ADInitParam025;
  uint32   ADInitParam026;
  uint32   ADInitParam027;
  uint32   ADInitParam028;
  uint32   ADInitParam029;
  uint32   ADInitParam030;
  uint32   ADInitParam031;
  uint32   ADInitParam032;
  uint32   ADInitParam033;
  uint32   ADInitParam034;
  uint32   ADInitParam035;
  uint32   ADInitParam036;
  uint32   ADInitParam037;
  uint32   ADInitParam038;
  uint32   ADInitParam039;
  uint32   ADInitParam040;
  uint32   ADInitParam041;
  uint32   ADInitParam042;
  uint32   ADInitParam043;
  uint32   ADInitParam044;
  uint32   ADInitParam045;
  uint32   ADInitParam046; 
  uint32   ADInitParam047;
  uint32   ADInitParam048;
  uint32   ADInitParam049;
  uint32   ADInitParam050; 
  uint32   ADInitParam051;
  uint32   ADInitParam052;
  uint32   ADInitParam053;
  uint32   ADInitParam054;
  uint32   ADInitParam055;
  uint32   ADInitParam056;
  uint32   ADInitParam057;
  uint32   ADInitParam058;
  uint32   ADInitParam059;
  uint32   ADInitParam060;
  uint32   ADInitParam061;
  uint32   ADInitParam062;
  uint32   ADInitParam063;
  uint32   ADInitParam064;
  uint32   ADInitParam065;
  uint32   ADInitParam066;
  uint32   ADInitParam067;
  uint32   ADInitParam068;
  uint32   ADInitParam069;
  uint32   ADInitParam070;
  uint32   ADInitParam071;
  uint32   ADInitParam072;
  uint32   ADInitParam073;
  uint32   ADInitParam074;
  uint32   ADInitParam075;

  /**< Mode Configuration Dependent Parameters */

  uint32   ADCfgParam001;
  uint32   ADCfgParam002;
  uint32   ADCfgParam003;
  uint32   ADCfgParam004;
  uint32   ADCfgParam005;
  uint32   ADCfgParam006;
  uint32   ADCfgParam007;
  uint32   ADCfgParam008;
  uint32   ADCfgParam009;
  uint32   ADCfgParam010;
  uint32   ADCfgParam011;
  uint32   ADCfgParam012;
  uint32   ADCfgParam013;
  uint32   ADCfgParam014;
  uint32   ADCfgParam015;
  uint32   ADCfgParam016;
  uint32   ADCfgParam017;
  uint32   ADCfgParam018;
  uint32   ADCfgParam019;
  uint32   ADCfgParam020;
  uint32   ADCfgParam021;
  uint32   ADCfgParam022;
  uint32   ADCfgParam023;
  uint32   ADCfgParam024;
  uint32   ADCfgParam025;
  uint32   ADCfgParam026;
  uint32   ADCfgParam027;
  uint32   ADCfgParam028;
  uint32   ADCfgParam029;
  uint32   ADCfgParam030;
  uint32   ADCfgParam031;
  uint32   ADCfgParam032;
  uint32   ADCfgParam033;
  uint32   ADCfgParam034;
  uint32   ADCfgParam035;
  uint32   ADCfgParam036;
  uint32   ADCfgParam037;
  uint32   ADCfgParam038;
  uint32   ADCfgParam039;
  uint32   ADCfgParam040;
  uint32   ADCfgParam041;
  uint32   ADCfgParam042;
  uint32   ADCfgParam043;
  uint32   ADCfgParam044;
  uint32   ADCfgParam045;
  uint32   ADCfgParam046;
 
} QDI_Panel_ADConfigType;

/** 
  Defines ambient light enhancement (Assertive Display or SVI) configuration.
*/
typedef struct
{
    QDI_Panel_ADConfigType          sADConfig;
    /**< Assertive Display configuration */

    QDI_Panel_SVIConfigType         sSVIConfig;
    /**< SVI configuration */

} QDI_Panel_ALEnhancementConfigType;


/** Secondary power source configuration via PMI modules.
*/

typedef struct
{
    QDI_PmicInterfaceControlType    eModuleType;
    /**< PMI module configuration. @newpagetable */

} QDI_PMI_PowerCfg;


/** Configures the dynamic EDID reading feature.
*/
typedef struct {
  bool32  bEnabled;
  /**< Enables/disables dynamic EDID reading. */

  uint32  uPreferredTimingMode;
  /**< Refers to the preferred Timing mode, or the second, third, or fourth
       detailed timing descriptor. */

  uint32  uStartAddress;
  /**< Start address for EDID information. */

}QDI_Panel_DynamicEDIDConfigType;

/** Defines the sharpening threshold configuration parameters
*/

typedef struct
{
  uint32       uSharpingEdgeThresh;         /**< Sharpening Edge Threshold value with range 1 to 1024 */
  uint32       uSharpingSmoothThresh;       /**< Sharpening Smoothness Threshold value with range 1 to 1024 */
  uint32       uSharpingNoiseThresh;        /**< Sharpening Noise Threshold value with range 1 to 256 */
} QDI_SharpeningThresholdParams;



/** Configuration for the dynamic blanking rate adjustment.
*/
typedef struct {
  uint32     uDynamicBlankingRateCntr;
  /**< Number of dynamic blanking adjustment rates supported by the panel. */

  uint32     uDynamicBlankingConfigRates[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< List of dynamic blanking configuration adjustment rates supported by the
       panel. */

  uint32     uVFrontPorchList[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< Vertical front porch adjustment corresponding to the vertical blanking
       configuration rate. */

  uint32     uVBackPorchList[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< Vertical back porch adjustment corresponding to the vertical blanking
       configuration rate. */

  uint32     uVSyncPulseList[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< Vertical sync pulse adjustment corresponding to the vertical blanking
       configuration rate. */

  uint32     uHFrontPorchList[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< Horizontal front porch adjustment corresponding to the horizontal
       blanking configuration rate. */

  uint32     uHBackPorchList[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< Horizontal back porch adjustment corresponding to the horizontal blanking
       configuration rate. */

  uint32     uHSyncPulseList[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< Horizontal sync pulse adjustment corresponding to the horizontal blanking
       configuration rate. */

}QDI_Panel_DynamicBlankingConfigType;


/** Configuration for the dynamic PLL rate adjustment
*/
typedef struct {
  uint32     uDynamicPLLAdjustmentRateCntr;
  /**< Number of dynamic PLL adjustment rates supported by the panel. */

  uint32     uDynamicPLLConfigRates[QDI_PANEL_MAX_NUMBER_DYNAMIC_REFRESH_RATES];
  /**< List of dynamic PLL adjustment configuration rates supported by the panel. */
}QDI_Panel_DynamicPLLConfigType;


/** Configuration for the dynamic refresh rate adjustment
*        1. Dynamic PLL rate adjustment method
*        2. Dynamic blanking adjustment method
*/
typedef struct {
  QDI_Panel_DynamicPLLConfigType              sDynamicPLLConfig;
  /**< Dynamic PLL adjustment configuration for this panel. Precedence over vertical blanking. */

  QDI_Panel_DynamicBlankingConfigType         sDynamicBlankingConfig;
  /**< Dynamic blanking adjustment configuration for this panel. */
}QDI_Panel_DynamicRateCfgType;



/** Configuration params for brightness range
*/
typedef struct
{

  uint32                                uMinimumLevel;
  /**< Minimum brightness level for this range defined in millinits (1/1000 of a nit). */
  uint32                                uMaximumLevel;
  /**< Maximum brightness level for this range defined in millinits (1/1000 of a nit). */
  uint32                                uStepSize;
  /**< The size of incremental brightness levels between minimum and maximum level in
       this range defined in millinits (1/1000 of a nit). */
  QDI_BrightnessRangeFlagType           uFlags;
  /**< Flag to indicate the property of the brightenss range. */

}QDI_Panel_BrightnessRangeConfigType;

/** Defines the brightness-specific settings.
*/
typedef struct
{

  uint32                                uMaxLuminance;
  /**< Maximum luminance of the display in millinits (1/1000 of a nit). */

  uint32                                uMinLuminance;
  /**< Minimum luminance of the display in millinits (1/1000 of a nit). */ 

  uint32                                uNumRanges;                                     
  /**< Total number of valid brightness range levels in sRangeParams. */

  QDI_Panel_BrightnessRangeConfigType   sRangeParams[QDI_PANEL_MAX_NUMBER_BRIGHTNESS_RANGE_LEVEL];  
  /**< Configuration of each brightness range, If the panel supports two ranges then 
       the two ranges should not overlap and the minimum level of second range should be greater 
       than maximum level of first range.*/

}QDI_Panel_BrightnessConfigType;

/** Defines the panelsync-specific settings.
*/
typedef struct
{
  uint32                              uPanelSyncTimeoutMs;
  /**< Timeout for panel sync. */

  uint32                              uPanelSyncRecoveryThreshold;
  /**< Min threshold for sync recovery. */

  uint32                              uPanelSyncRecoveryFrames;
  /**< Min number of frames xfered for recovery. */

  uint32                              uReserved[4];
  /**< Reserved. */

}QDI_Panel_SyncConfigType;


/** Defines the DSC parameters for the display
*/
typedef struct
{
  /* DSI DSC Configuration */
  bool32                   bDSCEnable;
  /**< Display Stream Compression Enabled on panel */

  uint32                   uDSCProfileID;
  /**< Display Stream Compression Profile ID supported by panel */
  /**< Below table summarizes the profile id configuration
         bLMSplitEnable   bBlockPredEnable      uCompressionRatio 
      {  FALSE,           1,                    2                   },  //ProfileID = 0x00 configuration : single layer mixer, active high input block prediction enable with 50% compression ratio
      {  TRUE,            1,                    2                   },  //ProfileID = 0x01 configuration : dual layer mixer, active high input block prediction enable with 50% compression ratio
      {  FALSE,           1,                    2                   },  //ProfileID = 0x02 configuration : single layer mixer, active high input block prediction enable with 50% compression ratio
      {  TRUE,            1,                    2                   },  //ProfileID = 0x03 configuration : dual layer mixer, active high input block prediction enable with 50% compression ratio
      {  FALSE,           1,                    3                   },  //ProfileID = 0x04 configuration : single layer mixer, active high input block prediction enable with 33.33% compression ratio
      {  TRUE,            1,                    3                   },  //ProfileID = 0x05 configuration : dual layer mixer, active high input block prediction enable with 33.33% compression ratio
      {  FALSE,           1,                    3                   },  //ProfileID = 0x06 configuration : single layer mixer, active high input block prediction enable with 33.33% compression ratio
      {  TRUE,            1,                    3                   },  //ProfileID = 0x07 configuration : dual layer mixer, active high input block prediction enable with 33.33% compression ratio
   */    

  uint32                   uDSCSliceWidth;             
  /**< Display Stream Compression Slice Width */

  uint32                   uDSCSliceHeight;
  /**< Display Stream Compression Slice Height */

  uint32                   uDSCMajorVersion;
  /**< Display Stream Compression Major version */

  uint32                   uDSCMinorVersion;
  /**< Display Stream Compression Minor version */

  uint32                   uDSCScrVersion; 
  /**< Display Stream Compression SCR version */

  uint32                   uDSCCompressionRatio;
  /**< Display Stream Compression Ratio, This is dynamically generated. @Caution, This is read-only; do not change it */

  bool32                   bDSCBlockPred;
  /**< Display Stream Compression Block Prediction enabled. @Caution, This is read-only; do not change it */

  uint32                   uDSCSlicePerPacket;
  /**< Display Stream Compression Slice per packet */
} QDI_DSCConfigType;

/** Defines the physical parameters of the display device.
*/
typedef struct
{
  uint32                              uPanelId;
  /**< Unique panel identification ID. */
  
  uint8                               cPanelName[QDI_PANEL_NAME_LENGTH];
  /**< Human readable panel name identifier. */
  
  QDI_DisplayConnectType              ePhysConnect;
  /**< Physical connection of the display. */

  QDI_RotateFlipType                  ePanelOrientation;
  /**< Physical panel orientation. */

  /* Interface information. */
  QDI_PixelFormatType                 eColorFormat;
  /**< Color format used by the display. */

  QDI_ColorOrderingType               eColorOrder;
  /**< Ordering of the color components. */

  QDI_ColorComponentsPackingType      eColorComponentsPacking;
  /**< Packing of the color components -- tight or loose. */

  QDI_ColorComponentAlignmentType     eColorComponentAlignment;
  /**< Color component alignment within its byte -- MSB or LSB. */

  QDI_Panel_InterfaceModeType         eInterfaceMode;
  /**< Defines the interface mode -- active, command, etc. */
  
  /* Panel information. */
  uint32                              iVisWidthPx;
  /**< Visible display width in pixels. */

  uint32                              iVisHeightPx;
  /**< Visible display height in pixels. */

  QDI_RectType                        sActiveRoi;
  /**< Visible portion of the display surface. */  
  
  /* Additional panel features */
  
  QDI_Panel_Backlight_ConfigType      sBacklight;
  /**< Backlight configuration. */

  QDI_PMI_PowerCfg                    sPMIPowerCfg;
  /**< Secondary power source via the PMIC interface module. */

  QDI_Panel_DynamicEDIDConfigType     sDynamicEDIDConfig;
  /**< Dynamic EDID configuration for the panel. */

  EDID_Config                         sEDID;
  /**< EDID configuration for the panel. */

  QDI_Panel_ActiveTimingType          sActiveTiming;
  /**< Standard configuration for active panels (common to all active
       panels). */

  QDI_Panel_AdaptiveBrightnessType    sAdaptiveBrightness;
  /**< Adaptive brightness configuration. */

  /** Panel-specific configuration. */
  union 
  {
    QDI_Panel_HdmiAttrType            sHdmi;  /**< HDMI configuration.            */
    QDI_Panel_DSIAttrType             sDsi;   /**< DSI configuration.             */
    QDI_Panel_EDPAttrType             sEdp;   /**< eDP configuration parameters.  */
    QDI_Panel_DPAttrType              sDp;    /**< DP configuration parameters.   */
  } uAttrs;

  QDI_Panel_FlagsType                 eDisplayFlags;
  /**< Miscellaneous display flags (see #QDI_Panel_FlagsType). */
  
  uint32                              uESDDetectionTime;
  /**< ESD detection and recovery time interval in milliseconds. */

  uint32                              uESDDetectionFailureRetryCount;
  /**< Number of ESD detection failure retries to attempt before resetting the
       display. */

  QDI_Panel_ALEnhancementConfigType   sALEnhancement;
  /**< Configuration type of the ambient enhancement feature. */
  
  QDI_SharpeningThresholdParams       sSharpeningThresholdParams;
  /**< Configuration for sharpening paramas for Default, Sharpeing and Smooting modes */

  QDI_Panel_PowerSavingOverrideType   ePowerSavingOverride;
  /**< Configuration for over-riding the default power-saving information. */ 

  uint32                              uDisplayRecoveryThreshold;
  /**< Display recovery threshold frame counter configuration triggers the recovery mechanism on 
       hitting the threshold value. Currently, the threshold is configured to a default value for 
       command-mode panels. If the value is set to zero then the recovery feature is disabled. */               

  QDI_Panel_DynamicRateCfgType        sDynamicRefreshRateConfig;
  /**< Dynamic refresh rate configuration to adjust the panel refresh rate dynamically */

  QDI_Panel_BrightnessConfigType      sBrightnessConfig;
  /**< Brightness configuration */

  QDI_Panel_SyncConfigType            sPanelSyncConfig;
  /**< Configurable parameters for panelsync */

  QDI_DSCConfigType                   sDSCConfig;
  /**< DSC configuration */
} QDI_Panel_AttrType;


/** Display frame and scanline statistics.
*/
typedef struct 
{
    uint32              uLineCount; 
    /**< Current scanline number, starting from 0 to the frame height. */

    uint32              uFrameCount; 
    /**< Total number of frames. */
 
} QDI_Panel_DisplayFrameInfoType;

/** Display power management functions. This configuration is used to control
    component voting to the kernel mode driver.
*/
typedef struct 
{
    QDI_Panel_ComponentType   eComponent;
    /**< Display ID for power management, i.e., display, memory refresh, and engine. */
    
    uint32                    uComponentId;
    /**< Reserved for future use. */

    QDI_Panel_PowerVoteType   ePowerVote;
    /**< Power management request type. */    

    uint32                    uFlags;
    /**< Reserved for future use. */
 
} QDI_Panel_PowerMgmtType;


/** Display commit configuration. This configuration is used to trigger an
    update to the display.
*/
typedef struct 
{
    bool32                     bAsync;
    /**< Force the display commit to occur asynchronously. */

    uint32                     uFlags;
    /**< Reserved for future use. */

} QDI_Panel_CommitConfigType;



/** FOSS Dynamic configuration
*     For dynamic FOSS configuration  update.
*/
typedef struct
{
  uint32   uOpMode;
  /**< Operation Mode: basic or contrast optimized */

  uint32   uContrastStrength;
  /**< Contrast strength applied by FOSS */

  uint32   uLutUpdateDelayCount;
  /**< Adaptive LUT update delay count */

  bool32   bGradientDetection;
  /**< Enables gradient detection. */

  uint32   uGradientThreshold;
  /**< Gradient threshold (range: 0 -- 100).
  If gradient detection is enabled and the calculated gradient score exceeds
  the gradient threshold, FOSS processes the image to minimize the banding
  effect. */

  uint32   uStaticSceneThreshold;
  /**< Static scene threshold (range 0 -- 255).
       Scene changes scored below this threshold are handled as static scene. */

} QDI_Panel_FOSSDynamicConfigType;


/** Returns the current FOSS processing information.
*/
typedef struct
{
  uint64                     uInfoPart1;
  /**< Reserved for future use. */
  uint64                     uInfoPart2;
  /**< Reserved for future use. */
} QDI_Panel_FossProcessingInfoType;

/** Configuration used to dynamically enable or disable the Ambient Light (AL)
    Enhancement feature.
*/
typedef struct 
{
    bool32                     bALEnhancementEnable;
    /**< Dynamically enables or disables AL enhancement. \n
         - TRUE  -- Enables AL enhancement
         - FALSE -- Disables AL enhancement @tablebulletend */

    uint32                     uFlags;
    /**< Reserved for future use. @newpagetable */

} QDI_Panel_ALEnhancementEnable;

/** Ambient Light Enhancement-SVI Data Configuration used to dynamically modify
    the SVI response.
*/
typedef struct
{

  uint32                     uBrightnessStrength;
  /**< Strength of SVI brightness boosting. A value of 0 represents the minimum
       brightness enhancement and 256 represents the maximum.*/

  uint32                     uContrastStrength;
  /**< Strength of SVI contrast boosting. A value of 0 represents the 
       minimum contrast enhancement and a value of 256 represents the maximum.*/

  uint32                     uFilterStepSizeUIMode;
  /**< SVI transition speed during UI display. A value of 1
       represents the slowest speed and 16 represents the fastest. */

  uint32                     uFilterStepSizeVideoMode;
  /**< SVI transition speed during video playback. 
       The range is 1 through 16. A value of 1 represents the slowest
       speed and 16 represents the fastest. */

  uint32                     IndoorMaxBrightnessLevel;
  /**< Controls the SVI response to ambient light input.
       The range for this value is 0 through 256. A value of 0
       represents minimal boosting is applied and 256 represents
       maximum boosting. */
     
  uint32                     uOutdoorMaxLuxLevel;
  /**< Ambient light level for which SVI applies maximum boosting. */

  uint32                     uOutdoorMaxBrightnessLevel;
  /**< Specifies the maximum ambient light based boosting applied by SVI. */
} QDI_Panel_ALESviDynamicConfigType;

/** Returns the current SVI processing information.
*/
typedef struct
{
  uint64                     uSviProcessingInfo;
  /**< Reserved for future use. @newpagetable
  */
} QDI_Panel_ALESviProcessingInfoType;

/** Configuration to dynamically modify calibration data of the AL Enhancement
    feature.
*/
typedef struct 
{

    uint32                     uCalibrationA;
    /**< Calibration data A. */

    uint32                     uCalibrationB;
    /**< Calibration data B.

         @note1 Do not change this parameter while the A, C, or D parameters are
         being modified. */

    uint32                     uCalibrationC;
    /**< Calibration data C. */

    uint32                     uCalibrationD;
    /**< Calibration data D. */

    uint32                     uStrengthLimit;
    /**< Vendor-specific calibration parameter. */

    uint32                     uTFilterControl;
    /**< Vendor-specific calibration parameter. */

    uint32                     uMaxIterations;                                   
    /**< Number of iterations needed by the AD Core to converge. */

}
QDI_Panel_ALEnhancementCalibrationType;


/** Properties to set the tone adjustment.
 */
typedef struct
{
  bool32           bEnable;                     
  /**< Enable or disable the tone adjustment. */

  uint32           uHueMax;                     
  /**< Region definition for maximum hue tone adjustment. */

  uint32           uHueMin;                     
  /**< Region definition for minimum hue tone adjustment. */

  uint32           uSaturationMax;              
  /**< Region definition for maximum saturation tone adjustment. */

  uint32           uSaturationMin;              
  /**< Region definition for minimum saturation tone adjustment. */

  uint32           uValueMax;                   
  /**< Region definition for maximum value tone adjustment. */

  uint32           uValueMin;                   
  /**< Region definition for minimum value tone adjustment. */

  int32            iHueGain;                    
  /**< Hue gain for the tone adjustment -- defines the slope of the two sides of 
       a symmetrical trapezoidal curve. */

  uint32           uHueMid;                     
  /**< Offset from the minimum that marks the top two corners of the 
       trapezoid. */

  int32            iValueGain;                  
  /**< Value gain for the tone adjustment -- defines the slope of the two sides
       of a symmetrical trapezoid. */

  int32            iSaturationGain;             
  /**< Saturation gain for the tone adjustment -- defines the slope of the two
       sides of the symmetrical trapezoid. @newpagetable */

} QDI_Panel_PA_MemoryColorConfigType;


/** Properties used to configure the piece-wise linear (PWL) curve for six-zone
    color adjustment.
 */
typedef struct
{
  int32            iHue;
  /**< Adjustment value in hue for a six-zone color LUT. */

  int32            iValue;
  /**< Adjustment value for six-zone color LUT. */

  int32            iSaturation;
  /**< Adjustment value in saturation for the six-zone color LUT. */

} QDI_Panel_PA_SixZonePwlType;


/** Properties used to set six-zone color adjustment and saturation.
 */
typedef struct
{
  bool32                      bHueEnable;                                
  /**< Enable/disable hue for six-zone adjustment. */

  bool32                      bValueEnable;                              
 /**< Enable/disable value for six-zone adjustment. */

  bool32                      bSaturationEnable;                         
  /**< Enable/disable saturation for six-zone adjustment. */

  uint32                      uSaturationMin;
  /**< Minimum saturation threshold to qualify the adjustment. */

  uint32                      uValueMin;
  /**< Minimum value threshold to qualify the adjustment. */

  uint32                      uValueMax;
  /**< Maximum value threshold to qualify the adjustment. */

  QDI_Panel_PA_SixZonePwlType sPwl[QDI_PANEL_SIX_ZONE_LUT_MAX_ENTRIES];
  /**< PWL for six-zone adjustment. */

} QDI_Panel_PA_SixZoneColorConfigType;



/** Properties for backlight control.
 */
typedef struct
{
  bool32                      bEnable;                                
  /**< Enable or disable the backlight module. */

  uint32                      uLevel;
  /**< Set the backlight to a specific level between 0 and 100%. */

  uint32                      uFlags;
  /**< Reserved for future use. @newpagetable */

} QDI_Panel_BacklightControlType;


/** Properties for private driver information.
 */
typedef struct
{
  QDI_Panel_ComponentType   eComponent;
  /**< Display ID for querying private information such as display, memory
       refresh, and engine. */
  
  uint32                    uComponentId;
  /**< Reserved for future use. */

  QDI_HandleType            hPerfTable;
  /**< Perf table handle */

  QDI_Device_VersionType    eDeviceVersion;
  /**< Device version. */

} QDI_Panel_DisplayPrivateInfo;


/** Allows enabling or disabling the EDID override.
*/
typedef struct _QDI_Panel_EDIDOverrideType
{
    bool32   bEnableEdidOverride;
    /**< Indicates if the EDID override is to be enabled or disabled. */

    uint32   uEdidOffset;
    /**< Beginning offset within EDID that this buffer has data for. Value
         must be 128 bytes. The base EDID is in block 0, while CEA extension
         information is in blocks starting from block 1. */

    uint32   uEdidBufferLength;
    /**< The size of the EDID override buffer in bytes. The value must be a
         multiple of 128 bytes. */

    uint8    *pEdidBuffer;
    /**< Pointer to EDID override buffer. @newpagetable */

} QDI_Panel_EDIDOverrideType;


/** Allows enabling or disabling Test Pattern generator usage.
 */
typedef struct _QDI_Panel_TestPatternType
{
    bool32        bEnableTestPattern;
    /**< Enables or disables the test pattern generator. */

    uint32        uTestPatternIndex;
    /**< Index of test pattern to set. Values supported: \n
         1 - Counter pattern \n
         2 - Fixed pattern \n
         3 - Color ramp 64p \n
         4 - Color ramp 256p \n
         5 - Black white vertical lines \n
         6 - Grayscale ramp \n
         7 - Color square \n
         8 - Checkered rectangle \n
         9 - Basic color changing pattern \n
         For any other values the API would return error. */

}QDI_Panel_TestPatternType;


/** Obtains display connection status.
*/
typedef struct _QDI_Panel_ConnectionStatusType
{
    bool32 bConnectionStatus;
    /**< Connection status of the display. \n
         - TRUE -- connected \n 
         - FALSE -- not connected @tablebulletend @newpagetable */
}QDI_Panel_ConnectionStatusType;


/** Panel hot plug event data.
*/
typedef struct _QDI_Panel_HotPlugEventDataType
{
    uint32                            uMaxModeWidth;
    /**< Maximum mode width in pixels.
         Widths above this value are filtered out of the mode list. */

    uint32                            uMaxModeHeight;
    /**< Maximum mode height in pixels.
         Heights above this value are filtered out of the mode list. */

    uint32                            uMaxModeRefreshRate;
    /**< Maximum refresh rate in Hz.
         Refresh rates above this value are filtered out of the mode list. */

    uint32                            uHPDFlags;
    /**< Flags indicating if certain features are supported.
         These flags also allow enabling or disabling certain features. */

    uint64                            uLinkRate;
    /**< Maximum supported link rate in bits per second. */
    
    uint32                            uNumOfLanes;
    /**< Number of data lanes available. */
    
    QDI_DP_ConnectionPolarityType     eConnectionPolarity;
    /**< Connection polarity, applicable for external DP connection. */
    
    QDI_DP_ConnectionPinAssignment    eConnectionPinAssignment;
    /**< Connection pin assignment, applicable for external DP connection. */

    QDI_DP_HPDNotificationType        eHPDType;
    /**< HPD notification type, applicable for external DP connection. */

} QDI_Panel_HotPlugEventDataType;



/** Panel hot plug notifications for virtual hot plug. 
    This enum is used to pass hot plug information from the OEM panel to the driver.
*/
typedef struct _QDI_Panel_HotPlugNotificationType
{    
    bool32                             bConnectionStatus;                                             
    /**< Communicates hot plug status. TRUE if plugged in; FALSE otherwise. */

    QDI_Panel_EDIDOverrideType        *pEdidInfo;                                       
    /**< Enables communicating EDID information from a plugged-in display. */

    QDI_Panel_HotPlugEventDataType    *pHPDEventData;                                      
    /**< Enables sending HPD-related data, such as the support limitations of link bandwidth, features, etc. */

} QDI_Panel_HotPlugNotificationType;


/** Panel HDCP Enable event data (QDI_PANEL_EVENT_HDCP_ENABLE).
*/
typedef struct _QDI_Panel_HDCPEnableEventDataType
{
    bool32                       bEnabled;                                            
    /**< HDCP status. TRUE -- enabled; FALSE otherwise. */

    QDI_Panel_HDCPVersionType    eHDCPVersion;
    /**< HDCP version to be enabled. */
    
    uint32                       uReserved[6];
    /**< Reserved. */

} QDI_Panel_HDCPEnableEventDataType;

/** Panel HDCP status event data (QDI_PANEL_EVENT_HDCP_STATUS).
*/
typedef struct _QDI_Panel_HDCPStatusEventDataType
{
    QDI_Panel_HDCPStatusType          eAuthStatus;                                            
    /**< HDCP authentication status. */
    
    uint32                            uReserved[7];
    /**< Reserved. */

} QDI_Panel_HDCPStatusEventDataType;

/** Ambient light change event data (QDI_PANEL_EVENT_AMBIENT_LEVEL_CHANGE).
*/
typedef struct _QDI_Panel_AmbientLightStatusEventDataType
{
    uint32                            uAmbientLightLevel;                                            
    /**< Ambient light level in milli lux. */
    
    uint32                            uReserved[1];
    /**< Reserved. */

} QDI_Panel_AmbientLevelStatusEventDataType;

/** Properties for setting or querying display features. Used by the
    QDI_DisplayUtil_SetProperty() and QDI_Panel_GetPropertyConfig() functions.
*/
typedef struct 
{
    QDI_Panel_ColorCorrectionMatrixType        *pColorCorrectionMatrix;
    /**< Pointer to the panel color correction matrix. \n Enables programming
         the color correction matrix within the display pipe. \n
         (IRQL = Passive) */

    QDI_Panel_DitheringMatrixType              *pDitheringMatrix;
    /**< Pointer to the panel dithering matrix. \n Enables programming
         the dithering matrix (4x4) within the display pipe. \n
         (IRQL = Passive) */
    
    QDI_Panel_GammaCorrectionTableType         *pGammaCorrectionTbl;
    /**< Pointer to the panel gamma correction table. \n
         Enables programming the gamma correction tables within the display pipe.
         \n
         (IRQL = Passive) */

    QDI_Panel_LinearGammaCorrectionTableType   *pLinearGammaCorrectionTbl;
    /**< Pointer to the panel linear gamma correction table. Enables programming
    the linear gamma correction tables within the display pipe. \n
    (IRQL = Passive) */
    
    QDI_Panel_InverseGammaCorrectionTableType  *pInverseGammaCorrectionTbl;
    /**< Pointer to the panel inverse gamma correction table. \n
         Enables programming the inverse gamma correction table within the display pipe. \n
         (IRQL = Passive) */

    QDI_Panel_GamutMappingTableType            *pGamutMappingTbl;
    /**< Pointer to the panel gamut mapping table. Enables programming the
         the 3D gamut mapping table. \n
         (IRQL = Passive) */

    QDI_HISCSettingsType                       *pHsicSettings;
    /**< Pointer to the panel HSIC settings. \n
         Enables configuring the HSIC control within the display pipe. \n
         (IRQL = Passive) */

    QDI_Panel_HistogramConfigurationType       *pHistogramConfiguration;    
    /**< Pointer to the panel histogram configuration. \n
         Enables configuring and retrieving histogram data generated by the
         histogram block. \n
         (IRQL = Passive) */

    QDI_Panel_PowerMgmtType                    *pPowerMgmt;    
    /**< Pointer to the panel power management configuration.
         \n Allows the OEM to override the panel power behavior of the driver. \n
         (IRQL = Passive) */

    QDI_Panel_CommitConfigType                 *pCommitConfig;    
    /**< Pointer to the panel commit configuration. \n
         Allows the OEM to trigger panel updates. Primarily used for triggering an
         update on a command mode panel. \n
         (IRQL = Passive) */
    
    uint32                                     *pBacklightLevel;
    /**< Pointer to the backlight level sent to the QDI driver for the AL
         Enhancement: 

         This value is represented in integer values in one of two ranges,
         depending on QDI_OEM_ALENHANCEMENT_BACKLIGHT_SCALE_FLAG:
         
         1.  Backlight Level range from 0 to 100 (default scenario). \n
         This range is used when QDI_OEM_ALENHANCEMENT_BACKLIGHT_SCALE_FLAG is
         not passed within the value (default behavior).

         In this scenario the range of this value can is from 0 through 100,
         where 0 is off and 100 is the maximum brightness. When used this way,
         the aPrivateData[11] member from #QDI_Panel_ALEnhancementConfigType
         must be set to the default value (0xFFFF).
         
         2.  Backlight Level range from 0 to BacklightScale. \n
         This range is used when QDI_OEM_ALENHANCEMENT_BACKLIGHT_SCALE_FLAG is
         passed within the value.

         In this scenario, the flag must be passed within each backlight level 
         (such as BacklightLevel | QDI_OEM_ALENHANCEMENT_BACKLIGHT_SCALE_FLAG)
         and the range of the value can go from 0 to the BacklightScale, where
         BacklightScale corresponds to the value configured through the
         aPrivateData[11] member from QDI_Panel_ALEnhancementConfigType. \n
         (IRQL = Passive) */

    QDI_Panel_CABLActivateType                 *pCABLActivate;
    /**< Pointer to activate or deactivate the CABL feature. */

    QDI_Panel_FOSSActivateType                 *pFOSSActivate;
    /**< Pointer to activate or deactivate the FOSS feature. */

    QDI_Panel_FOSSDynamicConfigType            *pFOSSDynamicConfig;
    /**< Pointer to the dynamic configuration structure for FOSS. Use when
         calling the QDI_DisplayUtil_SetProperty() function to update the FOSS
         response. */

    uint32                                     *pAmbientLightLevel;
    /**< Value representing the amount of ambient light, in lux, sent to the QDI
         driver. This value should be the ambient light level from the
         photosensor, normalized to a range from 0 through 0xFFFF. \n
         (IRQL = Passive) */

    QDI_Panel_ALEnhancementEnable              *pALEnhancementEnableType;
    /**< Pointer to enable or disable the AL Enhancement feature. */

    QDI_Panel_ALEnhancementCalibrationType     *pALEnhancementCalibration;
    /**< Pointer to the AL Enhancement calibration configuration. Allows the OEM
         to modify the AL Enhancement calibration. */    

    QDI_Panel_ALESviDynamicConfigType          *pALESviDynamicConfig;
    /**< Pointer to the dynamic configuration structure for SVI. Use when
         calling the QDI_DisplayUtil_SetProperty function to update the SVI response. */

    QDI_Panel_BacklightControlType             *pBacklightControl;
    /**< Pointer to the backlight control structure. Enables direct control of
         the backlight level. */
    
    QDI_Panel_EDIDOverrideType                 *pEdidOverride;
    /**< Pointer to the EDID override structure for the display. Enables or
         disables the EDID override. */

    QDI_Panel_TestPatternType                  *pTestPattern;
    /**< Pointer to the test pattern structure for the display. This enables or
         disables the test pattern generator. */

    QDI_Panel_HotPlugNotificationType          *pHotPlugData;
    /**< Pointer to the hot plug event data. Used to pass HPD polarity and other
         fields used for mode filtering. @newpagetable  */

    uint32                                     *pBrightnessLevel;
    /**< Value representing the current panel brightness level in milli nits (1/1000 of a nit). \n
         (IRQL = Passive) */
         
} QDI_Display_PropertyConfigType;


/** Gamma correction information.
*/
typedef struct
{
    QDI_Panel_GammaCorrectionType           eSupportedGCType;
    /**< Defines the supported gamma correction types. */

    uint32                                  uNumTableElements;
    /**< Number of linear gamma correction table elements. */

    uint32                                  uBitSizePerColor;
    /**<  Number of bits in each element of the gamma correction table.
          Data is aligned to the nearest byte (8 bits). */

}QDI_Panel_GammaCorrectionInfoType;

/** Polynomial color correction information.
*/
typedef struct
{
    QDI_Panel_ColorCorrectionType    eSupportedPccType;
    /**< Defines the supported polynomial color correction types. */

}QDI_Panel_ColorCorrectionInfoType;

/** Gamut mapping information.
*/
typedef struct
{
    QDI_Panel_GamutMappingModeType    eSupportedModeType;
    /**< Defines the supported gamut mapping types. */

    uint32                            uMaxSamplesPerColor;
    /**< Defines the maximum samples per color component. */

    uint32                            uMaxSegmentsPerColor;    
    /**< Defines the maximum segments per color component.
         Only valid for non-uniform mapping. @newpagetable */

}QDI_Panel_GamutMappingInfoType;

/** Dithering information.
*/
typedef struct
{
    QDI_Panel_DitheringModeType    eSupportedDitherType;
    /**< Defines supported dither modes. */

}QDI_Panel_DitheringInfoType;


/** Properties used to query the driver status. 
    Used by QDI_DisplayUtil_GetProperty().
*/
typedef struct 
{
    QDI_Panel_DisplayFrameInfoType       *pFrameInfo;
    /**< Pointer to display the frame information. */

    QDI_Display_CrcPropType              *pCRCInfo;
    /**< Pointer to display the CRC information. */

    QDI_Panel_DisplayPrivateInfo         *pPrivateInfo;
    /**< Pointer to driver private information. */

    QDI_Panel_ALESviProcessingInfoType   *pALESviProcessingInfo;
    /**< Pointer to the SVI's processing information. */

    QDI_Panel_FossProcessingInfoType     *pFOSSProcessingInfo;
    /**< Pointer to the FOSS's processing information. */

    QDI_Panel_CablProcessingInfoType     *pCablProcessingInfo;
    /**< Pointer to the CABL's processing information. */

    QDI_Panel_ConnectionStatusType       *pConnectionStatus;
    /**< Pointer to the connection status information. */

    QDI_Panel_GammaCorrectionInfoType    *pGammaCorrectionInfo;
    /**< Pointer to gamma correction information. */

    QDI_Panel_ColorCorrectionInfoType    *pColorCorrectionInfo;
    /**< Pointer to polynomial color correction information. */

    QDI_Panel_GamutMappingInfoType       *pGamutMappingInfo;
    /**< Pointer to gamut mapping information. */

    QDI_Panel_DitheringInfoType          *pDitheringInfo;
    /**< Pointer to dithering information. */

} QDI_Display_PropertyInfoType;


/** @} */ /* end_addtogroup data_types_qdi */

/** @ingroup panel_func_table
    OEM panel function table to support calls from the QDI driver to the OEM
    panel.
*/
typedef struct 
{
   uint32 uPanelFunctionTableVersion;
   /**< @ingroup panel_func_table_version
        OEM panel driver version. Must be populated by the caller via
        QDI_PANEL_API_VERSION during registration. */
       
   /* FUNCTION: QDI_Panel_Init */
   /** @ingroup panel_func_init
     Initializes the OEM panel driver.
 
     @datatypes
     QDI_HandleType
     QDI_Panel_AttrType

     @param[in]     hOemHandle    OEM-supplied context.

     @param[in,out] pPanelConfig  Pointer to the panel configuration; to be
                                  populated or overridden by the OEM.

     @detdesc
     This function is called from the Graphics KMD during the state transition
     from Loaded to Idle (F1).
     @par
     This is the first call to the OEM panel driver during driver
     initialization. The OEM panel driver must override or populate this
     structure to indicate which display it is to override, the functions
     for which it requires notification, and the panel settings to override.

     @return
     QDI_STATUS_OK -- Successful status must be returned.

     @dependencies
     This function call is made at IRQL = Passive.
   */
   QDI_ErrorType (*QDI_Panel_Init) (
     QDI_HandleType         hOemHandle,
     QDI_Panel_AttrType    *pPanelConfig);


/**  @ingroup panel_func_set_mode
     Calls the OEM panel driver driver when a mode is about to be set (usually
     after suspend or resume transition).
 
     @datatypes
     QDI_HandleType
     QDI_Panel_AttrType

     @param[in]       hOemHandle  OEM-supplied context.

     @param[in,out] pPanelConfig  Pointer to the panel configuration. To be
                                  overridden by the OEM.

     @detdesc
     OEM panel driver can override PanelConfig structure parameters. To do this,
     the OEM must set the parameters with the same values set while running the
     QDI_Panel_Init() function, and modify the values.

     @return
     QDI_STATUS_OK -- Successful status must be returned.

     @dependencies
     This function call is made at IRQL = Passive. @newpage
   */
   QDI_ErrorType (*QDI_Panel_SetMode) (
     QDI_HandleType         hOemHandle,
     QDI_Panel_AttrType    *pPanelConfig);

   /* FUNCTION: QDI_Panel_Term */
/** @ingroup panel_func_term
    Allows the OEM panel driver to clean up necessary context or memory to
    prepare to unload the Graphics KMD.

    @param[in] hOemHandle  OEM-supplied context.

    @detdesc
    This function is called from the Graphics KMD during the state transition
    from Idle (F1) to Loaded.
    @par
    The OEM panel driver must discontinue use of the OEM function table after
    returning from this call.

    @return
    QDI_STATUS_OK -- Successful status must be returned.

    @dependencies
    This function call is made at IRQL = Passive.
   */
   QDI_ErrorType (*QDI_Panel_Term) (
     QDI_HandleType       hOemHandle);

   /* FUNCTION: QDI_Panel_SetPower */
   /** @ingroup panel_func_set_power
     OEM panel driver call prior to powering up and powering down the panel.


     @param[in] hOemHandle  OEM-supplied context
     @param[in] bPowerOn    During post panel power up, this field is set to
                            TRUE. During power down, this field is set to FALSE
                            pre-power down.

     @detdesc
     This function call is made from the Graphics KMD during the power on/off
     state transitions. The OEM panel driver manages resources related to
     providing power to the panel.
     @par
     For power-on scenarios (bPowerOn=TRUE), the OEM panel driver is called
     after the panel's power rails are turned on. For power-off scenarios
     (bPowerOn=FALSE), the OEM panel driver is called before the power to the
     panel is turned off.

     @return
     QDI_STATUS_OK -- Successful status must be returned.

     @dependencies
     This function call is made at IRQL = Passive.
   */
   QDI_ErrorType (*QDI_Panel_SetPower)(
     QDI_HandleType  hOemHandle,  
     bool32          bPowerOn);
    
   /* FUNCTION: QDI_Panel_SetBacklight */
   /** @ingroup panel_func_set_backlight
     OEM panel driver call to adjust the backlight level. 

     @param[in] hOemHandle  OEM-supplied context.

     @param[in] uLevel      Requested backlight level (from 0% to 100%).

     @detdesc
     This function is called from the Graphics KMD for backlight events.
     Backlight event changes are passed from the Microsoft Dxgkrnl to the
     Graphics KMD, and these events are passed to the OEM panel driver.

     @return
     QDI_STATUS_OK -- Successful status must be returned.

     @dependencies
     This function call is made at IRQL = Passive.
   */
   QDI_ErrorType (*QDI_Panel_SetBacklight)(
     QDI_HandleType  hOemHandle,  
     uint32          uLevel);

   /* FUNCTION: QDI_Panel_SetAbaBacklightRatio */
   /** @ingroup panel_func_setAbabacklightRatio
     OEM panel driver call to apply a downscale ratio to the current backlight level. 
     This function is called from the Graphics KMD when ABA (CABL and SVI) processes 
     a frame and outputs a ratio to reduce the backlight. 
     
     @param[in] hOemHandle  OEM-supplied context.
     @param[in] uRatio      Ratio to apply (from 0 (0%) to 1024 (100%)).

     @return
     QDI_STATUS_OK -- Successful status must be returned.

     @dependencies
     This function call is made at IRQL = Passive.
   */
   QDI_ErrorType (*QDI_Panel_SetAbaBacklightRatio)(
     QDI_HandleType  hOemHandle,  
     uint32          uRatio);

   /** @ingroup panel_func_event
       OEM panel driver callback for various panel events.
 
     @param[in] hOemHandle      OEM-supplied context.
 
     @param[in] eEvents         Set of events that have occurred. For the list
                                of events, see #QDI_Panel_EventTypes.

     @param[in] pEventData      Reserved for future use.

     @param[in] uEventDataSize  Reserved for future use.

     @detdesc
     This function is called from the Graphics KMD for the events occurring in
     the Graphics KMD.     
     @par
     This event occurs during the interrupt handler at a high IRQL level (device
     IRQL). The caller must not block or make any OS calls at high IRQL levels.
     @par
     Events occurring at the passive level are allowed to stall the Graphics
     KMD. Stalling the Graphics KMD ensures that the OEM panel has completed
     execution before the Graphics KMD changes state.

     @return
     QDI_STATUS_OK -- Successful status must be returned.

     @dependencies
     This function call is made at IRQL <= DPC.
   */  
   QDI_ErrorType (*QDI_Panel_Event)(
     QDI_HandleType           hOemHandle,
     uint32                   eEvents,
     void                    *pEventData,
     uint32                   uEventDataSize
    );   

   /* FUNCTION: QDI_Panel_Status   */
   /** @ingroup panel_func_status
       Gets the panel status.

       @param[in] hOemHandle      OEM-supplied context.

       @return
       QDI_STATUS_OK      --   Panel is in a good state. \n
       QDI_STATUS_FAILED  --   Panel is in a bad state.

       @dependencies
       This function call is made at IRQL == PASSIVE_LEVEL.
   */
   QDI_ErrorType (*QDI_Panel_Status)(
       QDI_HandleType        hOemHandle);

   /* FUNCTION: QDI_Panel_GetPropertyConfig */
   /** @ingroup panel_func_get_prop_config
     Obtains panel-specific configuration data, that is, color correction
     matrix, gamma correction table, and inverse gamma correction table.
     
     @note1hang A NULL pointer member within the QDI_Display_PropertyConfigType
     data structure causes the corresponding configuration to be ignored.

     @datatypes
     #QDI_Display_PropertyConfigType

     @param[in]     hOemHandle     OEM-supplied context.
     @param[in,out] pPropertyData  Pointer to the property configuration.

     @return
     Reserved.

     @dependencies
     None.
   */  
   QDI_ErrorType (*QDI_Panel_GetPropertyConfig)(
     QDI_HandleType                    hOemHandle,  
     QDI_Display_PropertyConfigType   *pPropertyData);

  /* FUNCTION: QDI_Panel_SetBrightness */
  /** @ingroup panel_func_set_brightness
    OEM panel driver call to adjust the brightness level. 

    @param[in] hOemHandle            OEM-supplied context.

    @param[in] uLevel                Requested brightness level in 
                                     milli nits (1/1000 of a nit).

    @param[in] uTransitionTimeMs     Transition time (in milli sec) the OEM
                                     panel driver should take to set the 
                                     new level.
   @detdesc
    This function is called from the Graphics KMD to the OEM panel driver 
    for brightness events.
    This function will be called if the OEM has specified the brightness
    configuration using QDI_Panel_BrightnessConfigType otherwise legacy
    QDI_Panel_SetBacklight() will be called for backlight events.

    @return
    QDI_STATUS_OK -- Successful status must be returned.

    @dependencies
    This function call is made at IRQL = Passive.
  */
  QDI_ErrorType (*QDI_Panel_SetBrightness)(
    QDI_HandleType  hOemHandle,  
    uint32          uLevel,
    uint32          uTransitionTimeMs);

} QDI_Panel_FunctionTableType;


/** @ingroup display_func_table
    QDI OEM function table for calls from OEM panel to QDI Driver.
*/
typedef struct 
{
   /* FUNCTION: QDI_DisplayUtil_Register */
/** @ingroup display_func_register
    Registers the list of supported OEM panel driver functions with the
    Graphics KMD.

    @datatypes
    #QDI_Panel_FunctionTableType

    @param[in] eDisplayId      ID of the display to which to send the packet.

    @param[in] hOemHandle      OEM-supplied context to be passed to each OEM
                               function.

    @param[in] pFunctionTable  Pointer to the OEM function table list of
                               supported functions.

    @param[in] uFlags          Reserved for future use.

    @return
    QDI_STATUS_OK -- On success.

    @dependencies
    This function call is made at IRQL = Passive.
 */  
   QDI_ErrorType (*QDI_DisplayUtil_Register)(
       QDI_Display_IDType               eDisplayId,
       QDI_HandleType                   hOemHandle,       
       QDI_Panel_FunctionTableType     *pFunctionTable,
       uint32                           uFlags
    );
    
   /* FUNCTION: QDI_DisplayUtil_SendCmdBuf */
   /** @ingroup display_func_send_cmdbuf
     Transmits packet data to the panel.
     


     @param[in] eDisplayId       Display ID to which to send the packet.
     @param[in] uSendPacketType  Type of packet to be sent.
     @param[in] pSendPayload     Pointer to the Send buffer (maximum size of
                                 4K). The contents of this buffer can be copied
                                 to a hardware accessible buffer before 
                                 transmission.
     @param[in] uSendSize        Size in bytes for the data transfer.
     @param[in] uFlags           Available flags:
                                 - QDI_DSI_CMD_FLAG_DMA_FORCE_LP -- Force
                                 packet transfer into Low Power mode.
                                 - QDI_DSI_CMD_FLAG_DMA_FORCE_HS -- Force
                                 packet transfer into High Speed mode.
                                 - QDI_DSI_CMD_FLAG_QUEUE_WRITE_PACKET  --
                                 Queue the write packet. @tablebulletend

     @detdesc
     This function supports synchronous communication.   
     It is a blocking call and returns after the packet has been transmitted or
     if an error has occurred.
     @par
     If QDI_DSI_CMD_FLAG_QUEUE_WRITE_PACKET is used, the write packet queues
     internally until a final write packet without this flag is added to the
     queue. When the unflagged packet is added, the queue flushes and all
     packets (including the final packet) transmit to the panel.

     @return
     QDI_STATUS_OK -- On success.

     @dependencies
     This function call is made at IRQL = Passive.
   */  
   QDI_ErrorType (*QDI_DisplayUtil_SendCmdBuf)(
     QDI_Display_IDType       eDisplayId,
     uint8                    uSendPacketType, 
     void                    *pSendPayload,
     uint32                   uSendSize,
     uint32                   uFlags
    );   
    
   /* FUNCTION: QDI_DisplayUtil_ReadCmdBuf */
/** @ingroup display_func_read_cmdbuf
    Reads a buffer from the panel. 

    @param[in]     eDisplayId              Display ID to which to send the
                                           packet.

    @param[in]     uReadRequestPacketType  Type of read request packet command.

    @param[in]     pReadRequestPayload     Pointer to the read request packet
                                           payload (parameters). \n
                                           For HDMI displays, this parameter is a
                                           pointer to a structure of type
                                           QDI_Panel_HdmiReadPacketInfo.

    @param[in]     uReadRequestSize        Size in bytes of the read request
                                           packet buffer. \n
                                           For HDMI DDC reads, the buffer must
                                           be 128 bytes.

    @param[in]     pReadBuffer             Pointer to the user buffer to hold
                                           the returned data.

    @param[in,out] pReadBufferSize         Pointer to the size in bytes of the
                                           read buffer; also returns the
                                           number of bytes read. \n
                                           For HDMI DDC reads, the buffer must
                                           be 128 bytes.

    @param[in]     uFlags                  Available flags:
                                           - QDI_DSI_CMD_FLAG_READ_RAW_PACKET
                                           -- Return raw read response.
                                           - QDI_DSI_CMD_FLAG_DMA_FORCE_LP --
                                           Force packet transfer in Low Power
                                           mode.
                                           - QDI_DSI_CMD_FLAG_DMA_FORCE_HS --
                                           Force packet transfer in High
                                           Speed mode.
                                           @tablebulletend

    @detdesc
    The caller must specify the requesting
    packet as well as the destination for the data.

    @return
    QDI_STATUS_OK -- On success.

    @dependencies
    This function is a blocking call and returns only after the packet data has
    been returned or there is a timeout. No other send requests can be made
    while this request is being fulfilled.
    @par
    This function call must be made at IRQL = Passive.
   */  
   QDI_ErrorType (*QDI_DisplayUtil_ReadCmdBuf)(
       QDI_Display_IDType       eDisplayId,
       uint8                    uReadRequestPacketType, 
       void                    *pReadRequestPayload,
       uint32                   uReadRequestSize,    
       void                    *pReadBuffer,
       uint32                  *pReadBufferSize,
       uint32                   uFlags
    );   

   /* FUNCTION: QDI_DisplayUtil_GetLastDSIErrors */
   /** @ingroup display_func_get_last_dsi_errors
       @deprecated

     Returns the last DSI errors.

     @param[in]  eDisplayId    Display ID to which to send the packet.
     @param[out] puHostError   Pointer to the DSI host errors that occurred
                               during a DSI write operation.
     @param[out] puPanelError  Pointer to the DSI panel (peripheral) errors
                               that occurred during a DSI read operation.
     @param[in]  uFlags        Reserved; DSI error read flags.

     @return
     QDI_STATUS_OK -- On success.

     @dependencies
     None.
   */  
   QDI_ErrorType (*QDI_DisplayUtil_GetLastDSIErrors)(
     QDI_Display_IDType       eDisplayId,
     uint32                  *puHostErrors,
     uint32                  *puPanelErrors,
     uint32                   uFlags
    );

   /* FUNCTION: QDI_DisplayUtil_LinearFrameBufferInfo */
   /** @ingroup display_func_linear_frame_buffer
     Retrieves details about the linear frame buffer.

     @param[in]  eDisplayId  Display ID to which to send the packet.
     @param[out] pSurfInfo   Pointer to the linear frame buffer surface
                             information.
     @param[in]  uFlags      Reserved.

     @detdesc
     The linear frame buffer is a physically contiguous chunk of memory
     allocated during boot and used by the GOP.

     @return
     QDI_STATUS_OK -- On success.

     @dependencies
     This function is a blocking call; no other send requests can be made
     while this request is being fulfilled.
     @par
     This API call must be made at IRQL = Passive.
   */  
   QDI_ErrorType (*QDI_DisplayUtil_LinearFrameBufferInfo)(
     QDI_Display_IDType       eDisplayId,
     QDI_SurfaceAttrType     *pSurfInfo,
     uint32                   uFlags);

   /* FUNCTION: QDI_DisplayUtil_LinearFrameBufferUpdate */
   /** @ingroup display_func_linear_frame_buf_update
       Sets up or updates the contents of the linear frame buffer.


     @param[in] eDisplayId  Display ID to which to send the packet.

     @param[in] pData       Pointer to the user image data to copy into the
                            frame buffer. This data can be in paged memory
                            because it is copied to the existing linear frame
                            buffer. Pass NULL to terminate Linear Frame Buffer
                            mode. 
     @param[in] uWidth      Source image width. Must be equal to or smaller
                            than the width returned by
                            QDI_DisplayUtil_LinearFrameBufferInfo().
     @param[in] uHeight     Source image height. Must be equal to or smaller
                            than the height returned by
                            QDI_DisplayUtil_LinearFrameBufferInfo().
     @param[in] uStride     Source image stride. The stride must match the
                            format returned by 
                            QDI_DisplayUtil_LinearFrameBufferInfo().
     @param[in] uFlags      Reserved.

     @detdesc
     When the Linear Frame Buffer mode has been activated, the user's content is
     visible on the screen, covering the desktop background.
     @par
     The following is a typical linear frame buffer scenario:
     -# Call QDI_DisplayUtil_LinearFrameBufferUpdate to initiate the frame
        buffer on top of the current desktop background.
     -# Send repeated calls to QDI_DisplayUtil_LinearFrameBufferUpdate to
        update to multiple frames.
     -# Call QDI_DisplayUtil_LinearFrameBufferUpdate with a NULL buffer to
        terminate Linear Frame Buffer mode.

     @return
     QDI_STATUS_OK -- On success. @newpage

     @dependencies
     This function is a blocking call; no other send requests can be
     made while this request is being fulfilled.
     @par
     This function call must be made at IRQL = Passive.
     @par
     No other applications can be running while using this mode; only the
     desktop must be visible. The display cannot be suspended or off.
     @par
     Only the primary panel supports this mode.
     @par
     The caller must terminate Linear Frame Buffer mode before continuing
     with normal operation.
     @par
     The caller must ensure that there is sufficient delay added between
     successive updates to ensure that the previous update has completed.
   */  
   QDI_ErrorType (*QDI_DisplayUtil_LinearFrameBufferUpdate)(
     QDI_Display_IDType       eDisplayId,
     void                    *pData,
     uint32                   uWidth,
     uint32                   uHeight,
     uint32                   uStride,
     uint32                   uFlags);

   /* FUNCTION: QDI_DisplayUtil_SetColorCorrectionInfo */
   /** @ingroup display_func_set_color_correct

     @deprecated See QDI_Panel_FunctionTableType::QDI_Panel_GetPropertyConfig().
     
     Used to dynamically set the panel color correction settings, including
     the color correction matrix, gamma correction table, and inverse gamma
     correction table. A NULL pointer means there are no changes to the
     corresponding settings.

     @datatypes
     #QDI_Panel_ColorCorrectionMatrixType \n
     #QDI_Panel_GammaCorrectionTableType \n
     #QDI_Panel_InverseGammaCorrectionTableType

     @param[in] eDisplayId  Display ID to which to send the packet.
     @param[in] pColorInfo  Pointer to the panel-specific color correction
                            matrix.
     @param[in] pToneInfo   Pointer to the panel-specific gamma correction
                            table.
     @param[in] pIgcInfo    Pointer to the panel-specific inverse gamma
                            correction table.

     @return
     QDI_STATUS_OK -- On success.

     @dependencies
     None.
   */  
   QDI_ErrorType (*QDI_DisplayUtil_SetColorCorrectionInfo)(
     QDI_Display_IDType                          eDisplayId,
     QDI_Panel_ColorCorrectionMatrixType         *pColorInfo,
     QDI_Panel_GammaCorrectionTableType          *pToneInfo,
     QDI_Panel_InverseGammaCorrectionTableType   *pIgcInfo);  

    /* FUNCTION: QDI_DisplayUtil_ResetDisplay     */
    /** @ingroup display_func_reset_display
     Forces a display reset.
 
     @param[in] eDisplayId  ID of the display to be reset.
     @param[in] uFlags      Flags to reset the display. Supported value: \n
                            - QDI_PANEL_DISPLAYRESET_TDR_RESET -- Make a TDR
                              reset. @tablebulletend

     @dependencies
     This function sends a panel initialization event to the OEM panel 
     driver requesting the driver to resend the initialization sequence and
     initialize the panel.
                            
     @return
     QDI_STATUS_OK -- On success.

     @dependencies
     None.
   */  
    QDI_ErrorType (*QDI_DisplayUtil_ResetDisplay)(
      QDI_Display_IDType         eDisplayId,
      uint32                     uFlags);  


   /** @ingroup display_func_set_property
     Used to dynamically set display configurations; that is, color correction
     matrix, gamma correction table, and inverse gamma correction table.
 
     @datatypes
     #QDI_Display_PropertyConfigType

     @param[in] eDisplayId     Display ID to which to apply the properties.
     @param[in] pPropertyData  Pointer to the panel display property.
     @param[in] uFlags         Reserved for future use.

     @detdesc
     A NULL pointer member within the QDI_Display_PropertyConfigType data
     structure causes the corresponding configuration to be ignored.

     @return
     QDI_STATUS_OK -- On success.

     @dependencies
     None.
   */  
    QDI_ErrorType (*QDI_DisplayUtil_SetProperty)(
      QDI_Display_IDType                    eDisplayId,
      QDI_Display_PropertyConfigType       *pPropertyData,
      uint32                                uFlags);
     

   /** @ingroup display_func_get_property
     Used to retrieve current display information.

     @datatypes
     #QDI_Display_PropertyInfoType

     @param[in]  eDisplayId     Display ID from which to get the properties.
     @param[out] pPropertyInfo  Pointer to display property.
     @param[in]  uFlags         Reserved.

     @detdesc
     A NULL pointer member within the QDI_Display_PropertyInfoType data
     structure causes the corresponding member to be ignored.

     @return
     QDI_STATUS_OK -- On success.

     @dependencies
     None.
   */  
    QDI_ErrorType (*QDI_DisplayUtil_GetProperty)(
      QDI_Display_IDType                    eDisplayId,
      QDI_Display_PropertyInfoType         *pPropertyInfo,
      uint32                                uFlags);
     
} QDI_DisplayUtil_FunctionTableType;


#ifdef __cplusplus
}
#endif


#endif /* __QDI_OEM_H__ */
