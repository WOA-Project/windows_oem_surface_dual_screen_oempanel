;// SDPDriver event log messages: the message table resource of SDPDriver.sys.
;//
;// RT_MESSAGETABLE, id 1, language 0x409,
;// 312 bytes at 140021060, one block 0xC0010001..0xC0010004, Unicode.
;//
;// Built by SDPDriver.vcxproj: mc.exe -h <IntDir> -r <IntDir> -z SdpMessages SDPDriver.mc
;// generates SdpMessages.h (included by SdpDriver.h), SdpMessages.rc (included
;// by SDPDriver.rc) and MSG00409.bin.

MessageIdTypedef=NTSTATUS

SeverityNames=(
    Success=0x0:STATUS_SEVERITY_SUCCESS
    Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
    Warning=0x2:STATUS_SEVERITY_WARNING
    Error=0x3:STATUS_SEVERITY_ERROR
    )

FacilityNames=(
    System=0x0
    SdpDriver=0x1:FACILITY_SDP_DRIVER
    )

LanguageNames=(English=0x409:MSG00409)

;// Logged by OemPanelEvtDevicePrepareHardware (1400051a0, call at 140005554).
MessageId=0x0001
Severity=Error
Facility=SdpDriver
SymbolicName=SDP_MSG_PREPARE_HARDWARE_FAILED
Language=English
SDPDriver Prepare Hardware fails.
.

;// In the message table but never referenced by any code of the image
;// (no literal 0xC0010002 outside the resource), OemPanelDriverEvtDeviceD0Entry (140005050)
;// only traces its failure.
MessageId=0x0002
Severity=Error
Facility=SdpDriver
SymbolicName=SDP_MSG_D0_ENTRY_FAILED
Language=English
SDPDriver device D0 entry fails.
.

;// Logged by ProcessDisplayAction (140014550, call at 1400161e4); %2 is the action name.
MessageId=0x0003
Severity=Error
Facility=SdpDriver
SymbolicName=SDP_MSG_DISPLAY_ACTION_FAILED
Language=English
SDPDriver DisplayAction Fail  %2.
.

;// Logged by QDI_Panel_Event (14000c600, call at 14000d70c); %2 is the event name.
MessageId=0x0004
Severity=Error
Facility=SdpDriver
SymbolicName=SDP_MSG_QDI_EVENT_FAILED
Language=English
SDPDriver QDIEvent Fail  %2.
.
