#include "Driver.h"
#include "Device.h"
#include "DisplayProxy.tmh"

/* ------------------------------------------------------------------------------------------------
** Public functions definitions
** --------------------------------------------------------------------------------------------- */

#define BACKLIGHT_PERCENT_MAX 100

//-------------------------------------------------------------------------------------------------
//  DisplayProxy_PanelInit
//
//  @brief
//      Callback for oem panel driver initialization, called by Graphics KMD during the state
//      transition from Loaded to Idle (F1).
//      This is the first call to the OEM panel driver during driver
//      initialization. The OEM panel driver must override or populate this
//      structure to indicate which display it is to override, the functions
//      for which it requires notification, and the panel settings to override
//
//      OEM can update the static fields in the panel configuration for ex Ambient light
//      or adaptive backlight control configuration for primary display
//
//
//  @params
//      [IN]  hOemHandle
//            An Opqaue OEM handle
//      [OUT] pPanelConfig
//            Pointer to the OEM configuration structure, the panel driver can modify the panel
//            configuration fields prior to returning
//
//  @return
//      QDI_STATUS_OK if successful, else QDI_ErrorType
//-------------------------------------------------------------------------------------------------
//
static QDI_ErrorType DisplayProxy_PanelInit(QDI_HandleType hOemHandle,
											QDI_Panel_AttrType *pPanelConfig)
{
	QDI_ErrorType eStatus = QDI_STATUS_OK;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if (NULL == hOemHandle)
	{
		eStatus = QDI_STATUS_BAD_PARAM;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! invalid params");
	}
	else
	{
		PPANEL_DEVICE_CONTEXT panelDeviceContext = (PPANEL_DEVICE_CONTEXT)hOemHandle;
		PDEVICE_CONTEXT deviceContext = (PDEVICE_CONTEXT)panelDeviceContext->DeviceContext;
		QDI_Display_IDType eDisplayId = panelDeviceContext->eDisplayId;

		UNREFERENCED_PARAMETER(deviceContext);

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! eDisplayId: %d", eDisplayId);

		//
		// OEM should complete this function and can initialize the pPanelConfig as per the
		// display I/F
		//

		if (eDisplayId == QDI_DISPLAY_PRIMARY || eDisplayId == QDI_DISPLAY_SECONDARY)
		{
			// The LG / SiliconWorks panels on Surface Duo (1st Gen) will be already turned
			// on and functional when Windows is booting up. Unfortunately, these displays
			// also render frames before the GPU has finished initializing fully. The GPU
			// Driver only sets the DSC Compression Ratio value internally later in the
			// initialization process Causing wrong encoding to be used for a few seconds,
			// which can be both jarring and also send incorrect data to the display mux ic.
			// The documentation clearly states to not touch below value and that it is
			// read only, however, that's not true, the value is not read only And we force
			// it to be correct before the displays get turned on to ensure a smoother
			// transition in the OS.
			pPanelConfig->sDSCConfig.uDSCCompressionRatio = 3;
		}
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %d", eStatus);

	return eStatus;
}

static QDI_ErrorType SetBacklightLevel(
	PPANEL_DEVICE_CONTEXT panelDeviceContext,
	UINT32 scaledBacklightLevel)
{
	QDI_ErrorType eStatus;
	UINT32 eCounter = 0;
	LARGE_INTEGER delay = { 0 };

	PDEVICE_CONTEXT deviceContext = (PDEVICE_CONTEXT)panelDeviceContext->DeviceContext;

	if (scaledBacklightLevel < 1)
	{
		scaledBacklightLevel = 1;
	}

	if (scaledBacklightLevel > 0x3FF)
	{
		scaledBacklightLevel = 0x3FF;
	}

	BYTE commandPayload[3] = 
	{
		0x51, // MIPI_DCS_SET_DISPLAY_BRIGHTNESS
		0,
		0
	};

	commandPayload[2] = scaledBacklightLevel & 0xFF;
	commandPayload[1] = (scaledBacklightLevel >> 8) & 0x3;

	do
	{
		eStatus = deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_SendCmdBuf(
			panelDeviceContext->eDisplayId,
			0x39,
			commandPayload,
			sizeof(commandPayload),
			0);

		if (eStatus != QDI_STATUS_OK)
		{
			delay.QuadPart = RELATIVE(MICROSECONDS(55));
			KeDelayExecutionThread(KernelMode, TRUE, &delay);
		}
	} while (eStatus == QDI_STATUS_NO_RESOURCES && eCounter++ < 10);

	if (eStatus != QDI_STATUS_OK)
	{
		goto exit;
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %d", eStatus);
	return eStatus;
}

static QDI_ErrorType SetBacklightLevelForAllDisplays(
	PPANEL_DEVICE_CONTEXT panelDeviceContext,
	UINT32 newBacklightLevel)
{
	QDI_ErrorType eStatus;
	PDEVICE_CONTEXT deviceContext = (PDEVICE_CONTEXT)panelDeviceContext->DeviceContext;
	QDI_Display_IDType eDisplayId = panelDeviceContext->eDisplayId;

	eStatus = SetBacklightLevel(panelDeviceContext, newBacklightLevel);
	if (eStatus != QDI_STATUS_OK)
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetBacklightLevel: %d", eStatus);
		goto exit;
	}

	if (eDisplayId == QDI_DISPLAY_PRIMARY)
	{
		eStatus = SetBacklightLevel(&deviceContext->SecondaryPanel, newBacklightLevel);
	}
	else
	{
		eStatus = SetBacklightLevel(&deviceContext->PrimaryPanel, newBacklightLevel);
	}

	if (eStatus != QDI_STATUS_OK)
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetBacklightLevel: %d", eStatus);
		goto exit;
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %d", eStatus);
	return eStatus;
}

static QDI_ErrorType GetBacklightLevel(
	PPANEL_DEVICE_CONTEXT panelDeviceContext,
	UINT32 *scaledBacklightLevel)
{
	QDI_ErrorType eStatus;
	UINT32 eCounter = 0;
	LARGE_INTEGER delay = { 0 };

	PDEVICE_CONTEXT deviceContext = (PDEVICE_CONTEXT)panelDeviceContext->DeviceContext;

	BYTE commandPayload[1] =
	{
		0x52 // MIPI_DCS_GET_DISPLAY_BRIGHTNESS
	};

	BYTE responsePayload[4] =
	{
		0,
		0,
		0,
		0
	};

	uint32 responseSize = sizeof(responsePayload);

	do
	{
		eStatus = deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_ReadCmdBuf(
			panelDeviceContext->eDisplayId,
			0x06,
			commandPayload,
			sizeof(commandPayload),
			responsePayload,
			&responseSize,
			0);

		if (eStatus != QDI_STATUS_OK)
		{
			delay.QuadPart = RELATIVE(MICROSECONDS(55));
			KeDelayExecutionThread(KernelMode, TRUE, &delay);
		}
	} while (eStatus == QDI_STATUS_NO_RESOURCES && eCounter++ < 10);

	if (eStatus != QDI_STATUS_OK)
	{
		goto exit;
	}

	*scaledBacklightLevel = (responsePayload[0] << 8) | responsePayload[1];

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %d", eStatus);
	return eStatus;
}

//-------------------------------------------------------------------------------------------------
//  DisplayProxy_PanelSetBacklight
//
//  @brief
//      Callback for oem panel driver from the Graphics KMD for the backlight events. Backlight event changes are
//      passed from the Microsoft Dxgkrnl to the Graphics KMD, and these events are passed to the OEM
//      panel driver.
//
//  @params
//      [IN] hOemHandle
//           An Opqaue OEM handle
//      [IN] uBacklightLevel
//           Backlight level, Requested backlight level (from 0% to 100%)
//
//  @return
//      QDI_STATUS_OK if successful, else QDI_ErrorType
//-------------------------------------------------------------------------------------------------
//
static QDI_ErrorType DisplayProxy_PanelSetBacklight(QDI_HandleType hOemHandle,
													uint32 uBacklightLevel)
{
	QDI_ErrorType eStatus = QDI_STATUS_OK;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if ((NULL == hOemHandle) ||
		(uBacklightLevel > BACKLIGHT_PERCENT_MAX))
	{
		eStatus = QDI_STATUS_BAD_PARAM;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! invalid params");
	}
	else
	{
		//
		// Get pointer to display proxy context structure using the oem handle
		//
		PPANEL_DEVICE_CONTEXT panelDeviceContext = (PPANEL_DEVICE_CONTEXT)hOemHandle;
		QDI_Display_IDType eDisplayId = panelDeviceContext->eDisplayId;

		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! eDisplayId: %d", eDisplayId);
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! uBacklightLevel: %d", uBacklightLevel);

		//
		// OEM to Complete this function to configure the panel with the requested backlight
		// control
		//

		UINT32 newBacklightLevel = uBacklightLevel * 0x3FF / 0x64;

		eStatus = SetBacklightLevelForAllDisplays(panelDeviceContext, newBacklightLevel);
		if (eStatus != QDI_STATUS_OK)
		{
			TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetBacklightLevelForAllDisplays: %d", eStatus);
			goto exit;
		}
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %d", eStatus);
	return eStatus;
}

NTSTATUS DisplayProxy_SetSecondaryPanelBrightness(PDEVICE_CONTEXT deviceContext, uint32 level)
{
	NTSTATUS status = STATUS_SUCCESS;
	QDI_ErrorType error;

	QDI_Display_PropertyConfigType propertyConfigType;
	QDI_Panel_BacklightControlType backlightControlType;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	RtlZeroMemory(&propertyConfigType, sizeof(QDI_Display_PropertyConfigType));
	RtlZeroMemory(&backlightControlType, sizeof(QDI_Panel_BacklightControlType));

	backlightControlType.bEnable = TRUE;
	backlightControlType.uLevel = level;

	propertyConfigType.pBacklightControl = &backlightControlType;

	error = deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_SetProperty(
		QDI_DISPLAY_SECONDARY,
		&propertyConfigType,
		0
	);

	if (error != QDI_STATUS_OK)
	{
		status = STATUS_UNSUCCESSFUL;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! failed to set panel backlight control property (SECONDARY)");
		goto exit;
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %!STATUS!", status);

	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  DisplayProxy_GetFunctionTable
//
//  @brief
//      This API initializes the OEM supported function table for the current display
//
//  @params
//      [IN]  deviceContext
//            A handle to Display Proxy
//      [OUT] psPanelFunctions
//            Pointer to the panel function table to be modified by OEM depending upon what it
//            the support
//
//  @return
//      STATUS_SUCCESS if successful, else STATUS_UNSUCCESSFUL
//-------------------------------------------------------------------------------------------------
//
NTSTATUS DisplayProxy_GetFunctionTable(
	PDEVICE_CONTEXT deviceContext,
	QDI_Panel_FunctionTableType *psPanelFunctions)

{
	NTSTATUS status = STATUS_SUCCESS;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	if ((NULL == deviceContext) ||
		(NULL == psPanelFunctions))
	{
		status = STATUS_INVALID_PARAMETER;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! invalid params");
	}
	else
	{
		//
		// Populate all of the functions we want to get notifications for
		//
		psPanelFunctions->QDI_Panel_Init = &DisplayProxy_PanelInit;
		psPanelFunctions->QDI_Panel_SetBacklight = &DisplayProxy_PanelSetBacklight;

		//
		// Based on the eDisplayId OEM to set other function table
		//
		// OEM should complete this function by providing the supported
		// function pointers
		psPanelFunctions->QDI_Panel_SetMode = NULL;
		psPanelFunctions->QDI_Panel_SetPower = NULL;
		psPanelFunctions->QDI_Panel_SetAbaBacklightRatio = NULL;
		psPanelFunctions->QDI_Panel_Event = NULL;
		psPanelFunctions->QDI_Panel_SetBrightness = NULL;
		psPanelFunctions->QDI_Panel_Term = NULL;
		psPanelFunctions->QDI_Panel_Status = NULL;
		psPanelFunctions->QDI_Panel_GetPropertyConfig = NULL;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %!STATUS!", status);

	return status;
}

//-------------------------------------------------------------------------------------------------
//  DisplayProxy_InitFunctionTable
//
//  @brief
//      This API initializes the OEM function table which will be used with interacting with
//      Graphics KMD driver
//
//  @params
//      [IN]  deviceContext
//            A handle to DisplayProxy
//      [IN]  FunctionTable
//            Pointer to the panel function table to be modified
//      [IN]  FunctionTableSize
//            Size of the panel function table in bytes
//      [OUT] pBytesReturned
//            Number of bytes returned in the output IOCTL buffer
//
//  @return
//      STATUS_SUCCESS if successful, else STATUS_UNSUCCESSFUL
//-------------------------------------------------------------------------------------------------
//
NTSTATUS DisplayProxy_InitFunctionTable(
	IN PDEVICE_CONTEXT deviceContext,
	IN PVOID FunctionTable,
	IN ULONG FunctionTableSize,
	OUT size_t *BytesReturned)
{
	NTSTATUS status = STATUS_SUCCESS;
	QDI_DisplayUtil_FunctionTableType *psDispUtilsCallbacks;

	// Local list of functions we want notifications for (to be sent to the Graphics KMD driver)
	QDI_Panel_FunctionTableType sPanelFunctions;

	QDI_ErrorType error;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	//
	// Validated the input params
	//
	if ((NULL == FunctionTable) ||
		(sizeof(QDI_DisplayUtil_FunctionTableType) != FunctionTableSize) ||
		(NULL == BytesReturned))
	{
		status = STATUS_INVALID_PARAMETER;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! invalid params %x", status);
		goto exit;
	}

	//
	// IOCTL passes callbacks from OEM Panel Driver to Graphics KMD driver
	//
	psDispUtilsCallbacks = (QDI_DisplayUtil_FunctionTableType *)FunctionTable;

	//
	// Make a local copy of the support Graphics KMD driver callbacks
	//
	memcpy_s(&deviceContext->sDispUtilsFunctionTable, sizeof(QDI_DisplayUtil_FunctionTableType),
			 psDispUtilsCallbacks, sizeof(QDI_DisplayUtil_FunctionTableType));

	//
	// Initialize function table to Null
	//
	RtlZeroMemory(&sPanelFunctions, sizeof(QDI_Panel_FunctionTableType));

	//
	// Setup the version based on the qdi_oem.h header
	//
	sPanelFunctions.uPanelFunctionTableVersion = QDI_PANEL_API_VERSION;

	deviceContext->PrimaryPanel.DeviceContext = deviceContext;
	deviceContext->PrimaryPanel.eDisplayId = QDI_DISPLAY_PRIMARY;

	deviceContext->SecondaryPanel.DeviceContext = deviceContext;
	deviceContext->SecondaryPanel.eDisplayId = QDI_DISPLAY_SECONDARY;

	//
	// Get the populate list of all of the functions from OEM we want to get notifications for
	//
	status = DisplayProxy_GetFunctionTable(deviceContext, &sPanelFunctions);
	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! failed to get OEM function table");
		goto exit;
	}

	//
	// Execute callback to Graphics KMD driver to report supported panel functions
	//
	error = deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_Register(
		QDI_DISPLAY_PRIMARY,
		(QDI_HandleType)&deviceContext->PrimaryPanel,
		&sPanelFunctions,
		0);
	if (error != QDI_STATUS_OK)
	{
		status = STATUS_UNSUCCESSFUL;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! failed to register with Graphics KMD driver (PRIMARY)");
		goto exit;
	}

	error = deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_Register(
		QDI_DISPLAY_SECONDARY,
		(QDI_HandleType)&deviceContext->SecondaryPanel,
		&sPanelFunctions,
		0);
	if (error != QDI_STATUS_OK)
	{
		status = STATUS_UNSUCCESSFUL;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! failed to register with Graphics KMD driver (SECONDARY)");
		goto exit;
	}

	//
	// Set the BytesReturned to zero as nothing is returned from this IOCTL
	//
	*BytesReturned = 0;

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %!STATUS!", status);

	return status;
}

//-------------------------------------------------------------------------------------------------
//  DisplayProxy_DeviceIoControl
//
//  @brief
//      This API translates and handles incoming IOCTL requests coming from Graphics KMD driver
//      OEM driver should call this API from its I/O Event Processing callback registered with
//      WdfIoQueueCreate() create Queue IOCTL call
//
//  @params
//      [IN]  deviceContext
//            A handle to DisplayProxy
//      [IN]  IoControlCode
//            IO Control code for this request
//      [IN]  InputBuffer
//            Pointer to the input IOCTL request buffer
//      [IN]  InputBufferSize
//            Size of the input IOCTL request buffer
//      [IN]  OutputBuffer
//            Pointer to the output IOCTL request buffer
//      [IN]  OutputBufferSize
//            Size of the output IOCTL request buffer
//      [OUT] pBytesReturned
//            Number of bytes returned in the output IOCTL buffer
//
//  @return
//      STATUS_SUCCESS if successful, else STATUS_UNSUCCESSFUL
//-------------------------------------------------------------------------------------------------
//
NTSTATUS DisplayProxy_DeviceIoControl(
	IN PDEVICE_CONTEXT deviceContext,
	IN UINT32 IoControlCode,
	IN PVOID InputBuffer,
	IN size_t InputBufferSize,
	IN PVOID OutputBuffer,
	IN size_t OutputBufferSize,
	OUT size_t *BytesReturned)
{
	NTSTATUS status = STATUS_SUCCESS;

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	//
	// Validate the DisplayProxy handle and input and output params
	//
	if (deviceContext == NULL)
	{
		status = STATUS_INVALID_DEVICE_REQUEST;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! invalid input buffer %!STATUS!", status);
		goto exit;
	}

	switch (IoControlCode)
	{
	case IOCTL_OEMPANEL_QUERY_INTERFACE_TABLE:
	{
		//
		// Override the Panel function table with OEM requested functions
		//
		status = DisplayProxy_InitFunctionTable(deviceContext, InputBuffer, (ULONG)InputBufferSize, BytesReturned);
		break;
	}

	// DcsRead
	case 0x83212014:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! DcsRead");
	
		if (InputBufferSize < InputBufferSize || OutputBuffer == NULL)
		{
			status = 0xC000020;
			goto exit;
		}

		UINT32 Display = *(UINT32*)InputBuffer;
		uint8 uReadRequestPacketType = *((BYTE*)InputBuffer + 16);
		void* pReadRequestPayload = (BYTE*)InputBuffer + 17;
		uint32 ReadBufferSize = (uint32)OutputBufferSize;
		void* pReadBuffer = OutputBuffer;

		if (ReadBufferSize < 4)
		{
			ReadBufferSize = 4;
		}

		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! DcsRead Display=%d, uReadRequestPacketType=%d, ReadBufferSize=%d", Display, uReadRequestPacketType, ReadBufferSize);

		QDI_ErrorType error = deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_ReadCmdBuf(
			Display == 1 ? QDI_DISPLAY_PRIMARY : QDI_DISPLAY_SECONDARY,
			uReadRequestPacketType,
			pReadRequestPayload,
			1,
			pReadBuffer,
			&ReadBufferSize,
			0);

		*BytesReturned = ReadBufferSize;

		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! DcsRead QDI_DisplayUtil_ReadCmdBuf=%d, Display=%d, uReadRequestPacketType=%d, ReadBufferSize=%d", error, Display, uReadRequestPacketType, ReadBufferSize);
		break;
	}

	// DcsWrite
	case 0x83212010:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! DcsWrite");

		UINT32 Display = *(UINT32*)InputBuffer;
		uint8 uReadRequestPacketType = *((BYTE*)InputBuffer + 16);
		void* pReadRequestPayload = (BYTE*)InputBuffer + 17;
		uint32 uSendSize = *((UINT32*)InputBuffer + 1) - 1;

		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! DcsWrite Display=%d, uReadRequestPacketType=%d, uSendSize=%d", Display, uReadRequestPacketType, uSendSize);

		QDI_ErrorType error = deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_SendCmdBuf(
			Display == 1 ? QDI_DISPLAY_PRIMARY : QDI_DISPLAY_SECONDARY,
			uReadRequestPacketType,
			pReadRequestPayload,
			uSendSize,
			0);

		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! DcsWrite QDI_DisplayUtil_SendCmdBuf=%d, Display=%d, uReadRequestPacketType=%d, uSendSize=%d", error, Display, uReadRequestPacketType, uSendSize);
		break;
	}

	// GetBrightness
	case 0x8321201C:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! GetBrightness");

		UINT32 Display = *(UINT32*)InputBuffer;
		UINT32 CurrentBacklight = 0;

		GetBacklightLevel(
			Display == 1 ? (QDI_HandleType)&deviceContext->PrimaryPanel : (QDI_HandleType)&deviceContext->SecondaryPanel,
			&CurrentBacklight
		);

		*(BYTE*)OutputBuffer = (BYTE)((CurrentBacklight * 100 / 0x3FF) & 0xFF);
		*BytesReturned = OutputBufferSize;
		break;
	}

	// GetPanelDriverVersion
	case 0x8321205C:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! Version");

		if (OutputBufferSize != 12)
		{
			status = 0xC000020;
			goto exit;
		}

		PULONG buf = (PULONG)OutputBuffer;
		buf[0] = 2;
		buf[1] = 9;
		buf[2] = 0x2010056;
		*BytesReturned = OutputBufferSize;
		break;
	}

	// GetPanelMfgInfo
	case 0x8321204C:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! GetPanelMfgInfo");
		break;
	}

	// GetPanelRegInfo
	case 0x83212044:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! GetPanelRegInfo");
		break;
	}

	// GetPanelSelfTestInfo
	case 0x83212050:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! GetPanelSelfTestInfo");
		break;
	}

	// GetPanelState
	case 0x83212064:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! GetPanelState");
		break;
	}

	// GetRefreshRate
	case 0x83212034:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! GetRefreshRate");
		break;
	}

	// ResetDisplay
	case 0x83212058:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! ResetDisplay");

		UINT32 Display = *(UINT32*)InputBuffer;

		deviceContext->sDispUtilsFunctionTable.QDI_DisplayUtil_ResetDisplay(
			Display == 1 ? QDI_DISPLAY_PRIMARY : QDI_DISPLAY_SECONDARY,
			0
		);
		break;
	}

	// SetBrightness
	case 0x83212008:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetBrightness");

		UINT32 Display = *(UINT32*)InputBuffer;
		UINT32 uBacklightLevel = *((UINT32*)InputBuffer + 4);

		UINT32 newBacklightLevel = uBacklightLevel * 0x3FF / 100;

		SetBacklightLevel(
			Display == 1 ? (QDI_HandleType)&deviceContext->PrimaryPanel : (QDI_HandleType)&deviceContext->SecondaryPanel,
			newBacklightLevel
		);
		break;
	}

	// SetBrightnessNits
	case 0x83212068:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetBrightnessNits");

		UINT32 Display = *(UINT32*)InputBuffer;
		UINT32 uLevel = *((UINT32*)InputBuffer + 4);

		UINT32 newBacklightLevel = uLevel * 0x3FF / 100 / 6000;

		SetBacklightLevel(
			Display == 1 ? (QDI_HandleType)&deviceContext->PrimaryPanel : (QDI_HandleType)&deviceContext->SecondaryPanel,
			newBacklightLevel
		);
		break;
	}

	// SetCcMatrix
	case 0x83212804:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetCcMatrix");
		break;
	}

	// SetColorProp
	case 0x83212054:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetColorProp");
		break;
	}

	// SetDisplayOff
	case 0x83212040:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetDisplayOff");
		break;
	}

	// SetDisplayOn
	case 0x8321200C:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetDisplayOn");
		break;
	}

	// SetDithering
	case 0x83212070:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetDithering");
		break;
	}

	// SetPanelTemperature
	case 0x83212074:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetPanelTemperature");
		break;
	}

	// SetPixRegValue
	case 0x83212048:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetPixRegValue");
		break;
	}

	// SetTestMode
	case 0x83212078:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! SetTestMode");
		break;
	}

	// ToggleHwReset
	case 0x83212018:
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! ToggleHwReset");
		break;
	}

	default:
	{
		status = STATUS_INVALID_DEVICE_REQUEST;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "%!FUNC! invalid IoControl code %!STATUS!", status);
	}
	}

exit:
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit with status: %!STATUS!", status);
	return status;
}