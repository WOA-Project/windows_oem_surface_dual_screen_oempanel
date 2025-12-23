//-------------------------------------------------------------------------------------------------
// Copyright (C) 2010-2015 Qualcomm Technologies Incorporated.
// All Rights Reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// @file  oempanel.h
// @brief This is the header for the OEM Panel driver IOCTL interface
//-------------------------------------------------------------------------------------------------
#ifndef _OEMPANEL_H_
#define _OEMPANEL_H_

#include <ntddk.h>
#include <initguid.h>

/* -----------------------------------------------------------------------
** Macro definitions
** ----------------------------------------------------------------------- */
//
// OEM Panel driver IOCTL GUID
//
DEFINE_GUID(GUID_DEVINTERFACE_OEM_PANEL_DRIVER, 0xd4a3f63, 0xd08, 0x49a1, 0xb9, 0x1c, 0xc6, 0x5, 0x76, 0x89, 0x41, 0x74);

//
// OEM Bridge driver GUID
//
DEFINE_GUID(GUID_DEVINTERFACE_OEM_BRIDGE_DRIVER, 0xced8991e, 0xad1b, 0x44c7, 0xaf, 0xd8, 0x1d, 0x34, 0x2a, 0x21, 0x58, 0xea);
//
// OEM Panel driver IOCTL Control Codes
//
#define IOCTL_OEMPANEL_DEVICE_ID                      0x0

 //
 // OEM Panel driver IOCTL function code base
 //
#define IOCTL_OEMPANEL_FUNCTION_CODE_BASE             0x1000

//
// OEM Panel driver IOCTL Function Query Interface Table Code
//
#define IOCTL_OEMPANEL_FUNCTION_QUERY_INTF_TABLE      IOCTL_OEMPANEL_FUNCTION_CODE_BASE

//
// Maximum data object count which are defined for
// Type-C power delivery Vender defined messages
//
#define MAX_DATA_OBJECT_PER_VDM_TYPE                  0x8

/* -----------------------------------------------------------------------
** Enumerations
** ----------------------------------------------------------------------- */
//
// Defines Vendor Defined Messages for Alternate Mode of bridge HW
//
typedef enum _DISPLAYPROXY_VDM_TYPE {
  DISPLAYPROXY_VDM_INVALID,                 // Invalid message
  DISPLAYPROXY_VDM_DSCVID,                  // Discover Identity
  DISPLAYPROXY_VDM_DSCVSVID,                // Discover SVIDs
  DISPLAYPROXY_VDM_DSCVMODE,                // Discover Modes
  DISPLAYPROXY_VDM_ENTRMODE,                // Enter Mode
  DISPLAYPROXY_VDM_EXITMODE,                // Exit Mode
  DISPLAYPROXY_VDM_ATTN,                    // Attention
  DISPLAYPROXY_VDM_MAX,
  DISPLAYPROXY_VDM_FORCE_32BIT = 0x7FFFFFFF
} DISPLAYPROXY_VDM_TYPE, *PDISPLAYPROXY_VDM_TYPE;

/* -----------------------------------------------------------------------
** Structures definitions
** ----------------------------------------------------------------------- */

//
// This structure DISPLAYPROXY_VDM_INFO_REQUEST is the input argument of IOCTL_OEMPANEL_QUERY_VDM_INFO
//                                
typedef struct _DISPLAYPROXY_VDM_INFO_REQUEST
{
  DISPLAYPROXY_VDM_TYPE          VdmMessageType;      // VDM message type
} DISPLAYPROXY_VDM_INFO_REQUEST, *PDISPLAYPROXY_VDM_INFO_REQUEST;


//
// This structure DISPLAYPROXY_VDM_INFO_RESPONSE is the output argument of IOCTL_OEMPANEL_QUERY_VDM_INFO
//                                
typedef struct _DISPLAYPROXY_VDM_INFO_RESPONSE
{
  UINT8           NumValidObjects;                                // Valid objects pointed by DataObjectList
  UINT32          DataObjectList[MAX_DATA_OBJECT_PER_VDM_TYPE];   // Data Object List
} DISPLAYPROXY_VDM_INFO_RESPONSE, *PDISPLAYPROXY_VDM_INFO_RESPONSE;

/* -----------------------------------------------------------------------
** IOCTL definitions
** ----------------------------------------------------------------------- */
//
//IOCTL: IOCTL_OEMPANEL_QUERY_INTERFACE_TABLE
//           Input Buffer: QDI_DisplayUtil_FunctionTableType
//                         List of function pointers supported by Graphics KMD driver
//           Output Buffer: None
//
#define IOCTL_OEMPANEL_QUERY_INTERFACE_TABLE    CTL_CODE(IOCTL_OEMPANEL_DEVICE_ID, IOCTL_OEMPANEL_FUNCTION_CODE_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//IOCTL: IOCTL_OEMPANEL_QUERY_VDM_INFO
//           Input Buffer:DISPLAYPROXY_VDM_INFO_REQUEST
//               param DISPLAYPROXY_VDM_TYPE     - For which DISPLAYPROXY_VDM_TYPE, Data Object has to be retrieved
//
//           Output Buffer: DISPLAYPROXY_VDM_INFO_RESPONSE
//               param NumValidObjects (UINT8)   - Number of valid Data Object
//               param DataObjectList (UINT32)   - Data Object list retrieved from Display Proxy
//
#define IOCTL_OEMPANEL_QUERY_VDM_INFO           CTL_CODE(IOCTL_OEMPANEL_DEVICE_ID, IOCTL_OEMPANEL_FUNCTION_CODE_BASE + 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif // _OEMPANEL_H_

