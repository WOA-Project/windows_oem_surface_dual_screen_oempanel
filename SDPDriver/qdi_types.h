#ifndef __QDI_TYPES_H__
#define __QDI_TYPES_H__
/*===========================================================================*/
/*!
\file
    qdi_types.h

\b  DESCRIPTION

Common definitions header file. Providing common structures needed for QDI 2.0x Interface.

Copyright (c) 2010-2020 Qualcomm Technologies, Inc.  
All Rights Reserved.
=============================================================================*/

/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/

/* --- DO NOT ADD HEADER FILES HERE! --- */

/*===========================================================================

                        Defines and Structs

===========================================================================*/
/*===========================================================================

                        Defines and Structs

===========================================================================*/

/* -----------------------------------------------------------------------
** Shared Data Types
** ----------------------------------------------------------------------- */
#ifndef _UINT64_DEFINED
typedef  unsigned long long uint64;      /* Unsigned 64 bit value */
#define _UINT64_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  signed long long   int64;       /* Signed 64 bit value */
#define _INT64_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

#ifndef _BOOL32_DEFINED
typedef  unsigned long int  bool32;      /* Boolean value type. */
#define _BOOL32_DEFINED
#endif

#ifndef _UINTPTR_DEFINED                  /* Unsigned integer with an architecture dependent length */
#if ( defined(_WIN64) || (64 == __PTR_BITS__) || (8 == __sizeof_ptr))
typedef unsigned long long uintPtr;
#else
typedef unsigned long int  uintPtr;
#endif
#define _UINTPTR_DEFINED
#endif

#ifndef TRUE
#define TRUE   1   /* Boolean true value. */
#endif

#ifndef FALSE
#define FALSE  0   /* Boolean false value. */
#endif

#ifndef NULL
#define NULL  0
#endif


/*!
* \b QDI_CordType
* 
* Define the top left most corner of a ROI or Rectangle of interest.  The
* display coordinate system is defined with the top left-most corner being
* (0,0) with positive X direction facing right and the positive Y direction
* facing down.  Negative coordinates are valid.
*/
typedef struct
{
  int32              iPosX;
  int32              iPosY;
} QDI_CordType;


/*!
 * \b QDI_AddressType
 * 
 * Data structure holds the 64-bit address information
 */
typedef union 
{
  struct 
  {
      uint32 uLowPart;
      int32  iHighPart;
  } u;
  int64  iQuadPart;
} QDI_AddressType;


/*!
* \b QDI_PixelFormatType       
*
* Supported display/lcd color specific formats.
*
* The supported formats and their associated bit packings
* are listed below.
* Legend: 
*
*
*                    33222222222211111111110000000000  Bit                 \n
*                    10987654321098765432109876543210  Location            \n
*                   |--------------------------------|                     \n
*  RGB_565          |                RRRRRGGGGGGBBBBB|                     \n
*                   |--------------------------------|                     \n
*  RGB_666          |              RRRRRRGGGGGGBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  RGB_888          |        RRRRRRRRGGGGGGGGBBBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  ARGB_1555        |                ARRRRRGGGGGBBBBB|                     \n
*                   |--------------------------------|                     \n
*  XRGB_8888        |xxxxxxxxRRRRRRRRGGGGGGGGBBBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  ARGB_8888        |AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  BGR_565          |                BBBBBGGGGGGRRRRR|                     \n
*                   |--------------------------------|                     \n
*  BGR_8888         |        BBBBBBBBGGGGGGGGRRRRRRRR|                     \n
*                   |--------------------------------|                     \n
*  ABGR_1555        |                ABBBBBGGGGGRRRRR|                     \n
*                   |--------------------------------|                     \n
*  XBGR_8888        |xxxxxxxxBBBBBBBBGGGGGGGGRRRRRRRR|                     \n
*                   |--------------------------------|                     \n
*  ABGR_8888        |AAAAAAAABBBBBBBBGGGGGGGGRRRRRRRR|                     \n
*                   |--------------------------------|                     \n
*  Y_CBCR_H2V2      |                        YYYYYYYY|  Plane0             \n
* (Y_CBCR_H2V2_VC1) |                bbbbbbbbrrrrrrrr|  Plane1             \n
*                   |--------------------------------|                     \n
*  Y_CRCB_H2V2      |                        YYYYYYYY|  Plane0             \n
* (Y_CRCB_H2V2_VC1) |                rrrrrrrrbbbbbbbb|  Plane1             \n
*                   |--------------------------------|                     \n
*  YCRYCB_H2V1      |YYYYYYYYrrrrrrrrYYYYYYYYbbbbbbbb|                     \n
*                   |--------------------------------|                     \n
*  Y_CRCB_H2V1      |                Y0Y0Y0Y0Y1Y1Y1Y1|  Plane0             \n
*                   |                rrrrrrrrbbbbbbbb|  Plane1             \n
*                   |--------------------------------|                     \n
*  Y_CBCR_H2V1      |                Y0Y0Y0Y0Y1Y1Y1Y1|  Plane0             \n
*                   |                bbbbbbbbrrrrrrrr|  Plane1             \n
*                   |--------------------------------|                     \n
*  Y_CR_CB_H2V2     |                Y0Y0Y0Y0Y1Y1Y1Y1|  Plane0             \n
*                   |                        rrrrrrrr|  Plane1             \n
*                   |                        bbbbbbbb|  Plane2             \n
*                   |--------------------------------|                     \n
*  ARGB_4444        |                AAAARRRRGGGGBBBB|                     \n
*                   |--------------------------------|                     \n
*                    33222222222211111111110000000000  Bit                 \n
*                    10987654321098765432109876543210  Location            \n
*
* NOTE: Y_CBCR_H2V2_VC1 and Y_CRCB_H2V2_VC1 formats are identical in
*       packing order to Y_CBCR_H2V2 and Y_CRCB_H2V2 (respectively).  The
*       color range for the VC1 formats are WMV9, the same as main
*       profile (in advanced mode: nRangeY=nRangeC=16).
*
*/
typedef enum
{
  QDI_PIXEL_FORMAT_NONE = 0,
  QDI_PIXEL_FORMAT_RGB_565_16BPP,
  QDI_PIXEL_FORMAT_RGB_666_18BPP,
  QDI_PIXEL_FORMAT_RGB_888_24BPP,
  QDI_PIXEL_FORMAT_ARGB_1555_16BPP,
  QDI_PIXEL_FORMAT_XRGB_8888_32BPP,
  QDI_PIXEL_FORMAT_ARGB_8888_32BPP,
  QDI_PIXEL_FORMAT_BGR_565_16BPP,
  QDI_PIXEL_FORMAT_BGR_888_24BPP,
  QDI_PIXEL_FORMAT_ABGR_1555_16BPP,
  QDI_PIXEL_FORMAT_XBGR_8888_32BPP,
  QDI_PIXEL_FORMAT_ABGR_8888_32BPP,
  QDI_PIXEL_FORMAT_Y_CBCR_H2V2_12BPP, /**< 4:2:0, Pseudo planar, Cb(U) in MSB, viewing in bytes in chroma plane, it is CrCbCrCbCrCb..., same as NV21*/
  QDI_PIXEL_FORMAT_Y_CRCB_H2V2_12BPP, /**< 4:2:0, Pseudo planar, Cr(V) in MSB, viewing in bytes in chroma plane, it is CbCrCbCrCbCr..., same as NV12*/
  QDI_PIXEL_FORMAT_YCRYCB_H2V1_16BPP, /**< 4:2:2, YCrYCb(YVYU) interleave */
  QDI_PIXEL_FORMAT_YCBYCR_H2V1_16BPP, /**< 4:2:2, YCbYCr(YUYV) interleave */
  QDI_PIXEL_FORMAT_CRYCBY_H2V1_16BPP, /**< 4:2:2, CrYCbY(VYUY) interleave */
  QDI_PIXEL_FORMAT_CBYCRY_H2V1_16BPP, /**< 4:2:2, CbYCrY(UYVY) interleave */
  QDI_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP, /**< 4:2:2, Pseudo planar, Cr(U) in MSB */
  QDI_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP, /**< 4:2:2, Pseudo planar, Cb(V) in MSB */
  QDI_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP, /**< Identical packing to Y_CBCR_H2V2_12BPP */
  QDI_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP,  /**< Identical packing to Y_CRCB_H2V2_12BPP */
  QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP,  /**< 4:2:0, planar, Y,CR(V),Cb(U) in separate plane */
  QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_16BPP,  /**< 4:2:2, planar, Y,CR(V),Cb(U) in separate plane */
  QDI_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP = QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_16BPP,
  QDI_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP,  /**< 4:2:0, Pseudo planar, Cr(V), Cb(U) are in 4x2 tile format*/
  QDI_PIXEL_FORMAT_RGBA_5551_16BPP,
  QDI_PIXEL_FORMAT_RGBA_8888_32BPP,
  QDI_PIXEL_FORMAT_AYCBCR_H1V1_32BPP ,    /**< Alpha-YUV, Interleaved packing format, 4 bytes per pixel*/
  QDI_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP,  /**< 4:2:0, planar, Y,Cb(U),Cr(V) in separate plane */
  QDI_PIXEL_FORMAT_Y_CB_CR_H2V2_16BPP,  /**< 4:2:2, planar, Y,Cb(U),Cr(V) in separate plane */
  QDI_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP = QDI_PIXEL_FORMAT_Y_CB_CR_H2V2_16BPP,
  QDI_PIXEL_FORMAT_ARGB_4444_16BPP,
  QDI_PIXEL_FORMAT_XRGB_2101010_32BPP,         /**< 10 bit XRGB format */
  QDI_PIXEL_FORMAT_ARGB_2101010_32BPP,
  QDI_PIXEL_FORMAT_BGRX_1010102_32BPP,
  QDI_PIXEL_FORMAT_BGRA_1010102_32BPP,
  QDI_PIXEL_FORMAT_Y_CRCB_H2V2_P010_24BPP,     /**< 10 bit 4:2:0, Pseudo planar, CB(U) in MSB, viewing in bytes in chroma plane, it is CrCbCrCbCrCb... */
  QDI_PIXEL_FORMAT_Y_CRCB_H2V2_TP10_16BPP,     /**< 10 bit tiled 4:2:0 format, CR(V) in MSB, viewing in bytes in chroma plane, it is CrCbCrCbCrCb...   */
  QDI_PIXEL_FORMAT_RGBA_16161616_64BPP,        /**< 16 bit half-float (normalized between 0.0 and 1.0)    */
  QDI_PIXEL_FORMAT_RGB_101010_30BPP,           /**< 10 bit RGB format  */
  QDI_PIXEL_FORMAT_BGR_101010_30BPP,           /**< 10 bit BGR format  */
  QDI_PIXEL_FORMAT_MAX,  
  QDI_PIXEL_FORMAT_FORCE_32BIT = 0x7FFFFFFF
} QDI_PixelFormatType;

/*!
* \b QDI_3D_FrameFormatType
*
* Define all variations of 3D frame format. 
*/
typedef enum
{
  QDI_3D_FRAME_FORMAT_NONE                                = 0x0,
  QDI_3D_FRAME_FORMAT_FRAME_PACKING,                               /* Top and bottom full resolution frame packing */
  QDI_3D_FRAME_FORMAT_FIELD_ALTERNATIVE,                           /* Alternative Field for left and right frame */
  QDI_3D_FRAME_FORMAT_LINE_ALTERNATIVE,                            /* Line alternative between left and right frame */
  QDI_3D_FRAME_FORMAT_SIDE_BY_SIDE_FULL,                           /* Left and right are side by side full resolution */ 
  QDI_3D_FRAME_FORMAT_L_N_DEPTH,                                   /* Left and depth frame format */
  QDI_3D_FRAME_FORMAT_L_N_DEPTH_N_GFX_N_GFX_MINUS_DEPTH,           /* Left and depth and graphics and graphics - depth format */
  QDI_3D_FRAME_FORMAT_TOP_BOTTOM,                                  /* Top and bottom half resolution */
  QDI_3D_FRAME_FORMAT_RESERVED1,                                   /* Reserved for future use */
  QDI_3D_FRAME_FORMAT_SIDE_BY_SIDE_HALF,                           /* Left and right side by side half resolution */
  QDI_3D_FRAME_FORMAT_FRAME_INTERLEAVE                    = 0x10,  /* Left and right frames are consider separate input surface */
  QDI_3D_FRAME_FORMAT_MAX,
  QDI_3D_FRAME_FORMAT_FORCE_32BIT = 0x7FFFFFFF   
} QDI_3D_FrameFormatType;

/*!
* \b QDI_RotateFlipType
*
* Define all variations of the 90 degree fixed rotation and flip. Some
* rotation cases are redundant.  For example, specifying QDI_ROTATE_90_FLIP_HORZ or
* QDI_ROTATE_270_FLIP_VERT will have the same effect (and are identical to HW).
*/
typedef enum
{
  QDI_ROTATE_NONE,           /**< Do not rotate. */
  QDI_ROTATE_90,             /**< Rotate clockwise 90 degrees. */
  QDI_ROTATE_180,            /**< Rotate clockwise 180 degrees. */
  QDI_ROTATE_270,            /**< Rotate clockwise 270 degrees. */
  QDI_ROTATE_NONE_FLIP_HORZ, /**< Rotate 0 degrees, flip horizontally. */
  QDI_ROTATE_NONE_FLIP_VERT, /**< Rotate 0 degrees, flip vertically. */
  QDI_ROTATE_90_FLIP_HORZ,   /**< Rotate 90 degrees, flip horizontally. */
  QDI_ROTATE_90_FLIP_VERT,   /**< Rotate 90 degrees, flip vertically. */
  QDI_ROTATE_180_FLIP_HORZ,  /**< Rotate 180 degrees, flip horizontally. */
  QDI_ROTATE_180_FLIP_VERT,  /**< Rotate 180 degrees, flip vertically. */
  QDI_ROTATE_270_FLIP_HORZ,  /**< Rotate 270 degrees, flip horizontally. */
  QDI_ROTATE_270_FLIP_VERT,  /**< Rotate 270 degrees, flip vertically. */
  QDI_ROTATE_MAX,            /**< Maximum number of rotating modes. */
  QDI_ROTATE_FORCE_32BIT = 0x7FFFFFFF
} QDI_RotateFlipType;


/*!
 * \b QDI_FlipType
 * 
 * Defines enum for mirroring/flipping
 *
 */

typedef enum
{
  QDI_FLIP_NONE            = 0x00,  /**< No flip*/
  QDI_FLIP_HORIZONTAL_ONLY,         /**< Horizontal (left/right) flip*/
  QDI_FLIP_VERTICAL_ONLY,           /**< Vertical (up/down) flip*/
  QDI_FLIP_HORZ_AND_VERT,           /**< Both Horizontal and Vertical flipping*/
  QDI_FLIP_MAX,
  QDI_FLIP_FORCE_32BIT     = 0x7FFFFFFF
}QDI_FlipType;

/*!
 * \b QDI_VC1RangeLevelType
 *
 * Defines mapping levels for Luma and Chroma components during VC1 Range Mapping
 * If QDI_VC1_RANGE_LEVELx is specified, then the Y/UV components are scaled using following equation:
 *      Y[n] = CLIP(((((Y[n] - 128) * (x + 9)) + 4) >> 3) + 128)
 * 
 *
 */
typedef enum
{
  QDI_VC1_RANGE_LEVEL_NONE = 0x0,   /**< Range Mapping is disabled*/
  QDI_VC1_RANGE_LEVEL0,             /**< Range mapping is performed using the above equation, where x = 0*/
  QDI_VC1_RANGE_LEVEL1,             /**< x = 1 */   
  QDI_VC1_RANGE_LEVEL2,             /**< x = 2 */
  QDI_VC1_RANGE_LEVEL3,             /**< x = 3 */    
  QDI_VC1_RANGE_LEVEL4,             /**< x = 4 */      
  QDI_VC1_RANGE_LEVEL5,             /**< x = 5 */
  QDI_VC1_RANGE_LEVEL6,             /**< x = 6 */       
  QDI_VC1_RANGE_LEVEL7,             /**< x = 7 */   
  QDI_VC1_RANGE_LEVEL_MAX,
  QDI_VC1_RANGE_LEVEL_FORCE_32BIT = 0x7FFFFFFF
} QDI_VC1RangeLevelType;


/*!
 * \b QDI_VC1RangeMapType
 *
 * The Luma and Chroma Range Mapping levels are specified in this structure.
 * The Luma and Chroma components are rescaled using the following equations:
 *      Y[n]  = CLIP(((((Y[n]  - 128) * (eRangeMapY  + 9)) + 4) >> 3) + 128)
 *      Cb[n] = CLIP(((((Cb[n] - 128) * (eRangeMapUV + 9)) + 4) >> 3) + 128)
 *      Cr[n] = CLIP(((((Cr[n] - 128) * (eRangeMapUV + 9)) + 4) >> 3) + 128)
 * If both eRangeMapY and eRangeMapUV are set to QDI_VC1_RANGE_LEVEL_NONE, then VC1 range mapping is disabled
 *
 */
typedef struct
{
  QDI_VC1RangeLevelType    eRangeMapY;     /**< Range mapping level for Luma*/
  QDI_VC1RangeLevelType    eRangeMapUV;    /**< Range mapping level for Chroma*/
} QDI_VC1RangeMapType;


/*!
* \b QDI_DisplayConnectType
*
* Defines the MDP hw connectivity for each HW channel.
*/
typedef enum
{
  QDI_DISPLAY_CONNECT_NONE = 0,
  QDI_DISPLAY_CONNECT_EBI2,                              /**< 1  - Display is connected to EBI2 interface - DEPRECATED*/
  QDI_DISPLAY_CONNECT_LCDC,                              /**< 2  - Display is connected to LCDC interface - DEPRECATED */
  QDI_DISPLAY_CONNECT_MDDI_PRIMARY,                      /**< 3  - Display is connected to primary MDDI interface - DEPRECATED */
  QDI_DISPLAY_CONNECT_MDDI_EXTERNAL,                     /**< 4  - Display is connected to external MDDI interface - DEPRECATED */
  QDI_DISPLAY_CONNECT_TV,                                /**< 5  - It is deprecated. Please use QDI_DISPLAY_CONNECT_ANALOG_TV instead. */
  QDI_DISPLAY_CONNECT_ANALOG_TV = QDI_DISPLAY_CONNECT_TV,/**< 5  - Display is connected to TV interface - DEPRECATED */
  QDI_DISPLAY_CONNECT_MDDI_LCDC,                         /**< 6  - Display is connected to MDDI LCDC mode interface - DEPRECATED */
  QDI_DISPLAY_CONNECT_DTV,                               /**< 7  - It is deprecated. Please use QDI_DISPLAY_CONNECT_HDMI instead. */
  QDI_DISPLAY_CONNECT_HDMI = QDI_DISPLAY_CONNECT_DTV,    /**< 7  - Display is connected to HDMI interfaces */
  QDI_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO,                 /**< 8  - Display is connected to Primary DSI interface using Video Mode     */
  QDI_DISPLAY_CONNECT_PRIMARY_DSI_CMD,                   /**< 9  - Display is connected to Primary DSI interface using Command Mode   */
  QDI_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO,               /**< 10 - Display is connected to Secondary DSI interface using Video Mode   */
  QDI_DISPLAY_CONNECT_SECONDARY_DSI_CMD,                 /**< 11 - Display is connected to Secondary DSI interface using Command Mode */
  QDI_DISPLAY_CONNECT_LVDS,                              /**< 12 - Display is connected to LVDS interface */
  QDI_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK,             /**< 13 - QDI output is written back to Frame Buffer (Memory) via the writeback feature. *
                                                          **<      i.e. Output is not connected to physical display but to memory */
  QDI_DISPLAY_CONNECT_DP,                                /**< 14 - Display is connected to Display Port interface */
  QDI_DISPLAY_CONNECT_DBI,                               /**< 15 - Display is connected to DBI interface */  
  QDI_DISPLAY_CONNECT_MHL,                               /**< 16 - Display is connected to MHL interface */
  QDI_DISPLAY_CONNECT_DP_ALT_MODE,                       /**< 17 - Display is connected to Display Port over Type-C USB interface */
  QDI_DISPLAY_CONNECT_EDP,                               /**< 18 - Display is connected to Embedded Display Port interface */
  QDI_DISPLAY_CONNECT_MAX,
  QDI_DISPLAY_CONNECT_FORCE_32BIT = 0x7FFFFFFF
} QDI_DisplayConnectType;

/*!
 * \b QDI_DeinterlaceType
 *
 * Defines the type of deinterlacing.
 */
typedef enum
{
  QDI_DEINTERLACE_NONE = 0,
  QDI_DEINTERLACE_BOB_LINEAR_REPLICATE,
  QDI_DEINTERLACE_BOB_VERTICAL_STRETCH,
  QDI_DEINTERLACE_BOB_VERTICAL_STRETCH_4TAP,
  QDI_DEINTERLACE_PIXEL_ADAPTIVE,
  QDI_DEINTERLACE_MAX,
  QDI_DEINTERLACE_FORCE_32BIT = 0x7FFFFFFF
}QDI_DeinterlaceType;


/*!
* \b QDI_Color_EncodingType
*
*  Defines the standards for gamma correction, inverse gamma correction and Color Conversion Matrix.
*/
typedef enum
{
  QDI_COLOR_ENCODING_DEFAULT = 0, /** Default color encoding type, REC601        */
  QDI_COLOR_ENCODING_REC601,      /** ITU-R BT.601 (CCIR601)                     */
  QDI_COLOR_ENCODING_REC709,      /** ITU-R BT.709 (CCIR709)                     */
  QDI_COLOR_ENCODING_sRGB,        /** Standard RGB                               */
  QDI_COLOR_ENCODING_REC2020,     /** ITU-R BT.2020 (CCIR2020)                   */
  QDI_COLOR_ENCODING_scRGB,       /** Standard scRGB                             */
  QDI_COLOR_ENCODING_HDR10,       /** ITU-R BT.2020 (CCIR2020)+SMPTE ST2084 (PQ) */
  QDI_COLOR_ENCODING_MAX,
  QDI_COLOR_ENCODING_FORCE_32BIT = 0x7FFFFFFF
} QDI_Color_EncodingType;

/*!
 * \b DisplayColorComponentsPackingType
 *
 * Indicates how used bits of color components are packed.
 */
typedef enum
{
  QDI_COLOR_COMPONENTS_PACKING_TIGHT = 0, /**< No unused bits present between color components */    
  QDI_COLOR_COMPONENTS_PACKING_LOOSE,     /**< 1 color component per byte */
  QDI_COLOR_COMPONENTS_PACKING_MAX,
  QDI_COLOR_COMPONENTS_PACKING_FORCE_32BIT = 0x7FFFFFFF
} QDI_ColorComponentsPackingType;

/*!
 * \b DisplayColorComponentAlignmentType
 *
 * For loose packing, indicates how used bits are aligned within color component's byte.
 */
typedef enum
{
  QDI_COLOR_COMPONENT_ALIGNMENT_LSB = 0, /**< Color component's used bits are LSB-aligned */
  QDI_COLOR_COMPONENT_ALIGNMENT_MSB,     /**< Color component's used bits are MSB-aligned */
  QDI_COLOR_COMPONENT_ALIGNMENT_MAX,
  QDI_COLOR_COMPONENT_ALIGNMENT_FORCE_32BIT = 0x7FFFFFFF
} QDI_ColorComponentAlignmentType;


/*!
 * \b DisplayColorOrderingType
 *
 * Determines component ordering of interface (MSB to LSB)
 */
typedef enum
{
  QDI_COLOR_ORDERING_RGB = 0, /**< RGB */
  QDI_COLOR_ORDERING_RBG,     /**< RBG */
  QDI_COLOR_ORDERING_GRB,     /**< GRB */
  QDI_COLOR_ORDERING_GBR,     /**< GBR */
  QDI_COLOR_ORDERING_BRG,     /**< BRG */
  QDI_COLOR_ORDERING_BGR,     /**< BGR */
  QDI_COLOR_ORDERING_MAX,
  QDI_COLOR_ORDERING_FORCE_32BIT = 0x7FFFFFFF
} QDI_ColorOrderingType;


/*!
 * \b QDI_Display_IDType
 *  
 *  Defines displays ID
 */
typedef enum
{
  QDI_DISPLAY_NONE = 0,
  QDI_DISPLAY_PRIMARY,       /**< for primary display */
  QDI_DISPLAY_SECONDARY,     /**< for secondary display */
  QDI_DISPLAY_THIRD,         /**< for third display */
  QDI_DISPLAY_EXTERNAL,      /**< for 1st external */
  QDI_DISPLAY_EXTERNAL2,     /**< for 2nd external */
  QDI_DISPLAY_EXTERNAL3,     /**< for 3rd external */
  QDI_DISPLAY_FRAMEBUFFER,   /**< for framebuffer writeback */
  QDI_DISPLAY_EXTERNAL4,     /**< for 4th external */
  QDI_DISPLAY_EXTERNAL5,     /**< for 5th external */
  QDI_DISPLAY_EXTERNAL6,     /**< for 6th external */
  QDI_DISPLAY_ID_MAX,
  QDI_DISPLAY_ID_FORCE_32BIT =0x7FFFFFFF
} QDI_Display_IDType;


/*!
 * \b QDI_Layer_IDType
 *
 * Defines Layer ID's
 */
typedef enum
{
  QDI_LAYER_ID_GRAPHICS1 = 0,  /**< RGB1 */
  QDI_LAYER_ID_GRAPHICS2,      /**< RGB2 */
  QDI_LAYER_ID_GRAPHICS3,      /**< RGB3 */
  QDI_LAYER_ID_GRAPHICS4,      /**< RGB4 */
  QDI_LAYER_ID_GRAPHICS_ANY,   /**< RGB1-RGB4 */
  QDI_LAYER_ID_OVERLAY1,       /**< ViG1 */
  QDI_LAYER_ID_OVERLAY2,       /**< ViG2 */
  QDI_LAYER_ID_OVERLAY3,       /**< ViG3 */
  QDI_LAYER_ID_OVERLAY4,       /**< ViG4 */
  QDI_LAYER_ID_OVERLAY_ANY,    /**< ViG1-ViG4 */
  QDI_LAYER_ID_DMA1,           /**< DMA1 */
  QDI_LAYER_ID_DMA2,           /**< DMA2 */
  QDI_LAYER_ID_DMA3,           /**< DMA3 */
  QDI_LAYER_ID_DMA4,           /**< DMA4 */
  QDI_LAYER_ID_DMA5,           /**< DMA5 */
  QDI_LAYER_ID_DMA6,           /**< DMA6 */
  QDI_LAYER_ID_DMA7,           /**< DMA7 */
  QDI_LAYER_ID_DMA8,           /**< DMA8 */
  QDI_LAYER_ID_DMA_ANY,        /**< DMA1-DMA8 */
  QDI_LAYER_ID_CURSOR1,        /**< Cursor1 */
  QDI_LAYER_ID_CURSOR2,        /**< Cursor2 */
  QDI_LAYER_ID_CURSOR3,        /**< Cursor3 */
  QDI_LAYER_ID_CURSOR4,        /**< Cursor4 */
  QDI_LAYER_ID_MAX,
  QDI_LAYER_ID_FORCE_32BIT = 0x7FFFFFFF
} QDI_Layer_IDType;


/*!
 * \b QDI_AccessLevelType
 *
 *  Defines the list of available access levels for QDI resources.
 *
 *  Access modes are defined as follows
 *
 *  QDI_ACCESS_SHARED      - Client is the primary user of this resource but allows other clients to
 *                            have shared access to a subset of functionality. 
 *  QDI_ACCESS_EXCLUSIVE   - Client has sole access to the resource.  No other clients can initialize
 *                            and use this resource.
 *  QDI_ACCESS_NONE        - Access level is unspecified or not in use.
 *                                    
 */
typedef enum
{
  QDI_ACCESS_SHARED         = 0,
  QDI_ACCESS_EXCLUSIVE,
  QDI_ACCESS_NONE,
  QDI_ACCESS_MAX,
  QDI_ACCESS_FORCE_32BIT    = 0x7FFFFFFF
} QDI_AccessLevelType, QDI_Layer_AccessLevelType;

/*!
 * \b QDI_PixelFormatCapType
 *
 * Flags to indicate the pixel format capabilities like macro tile format.
 */
typedef enum
{
  QDI_PIXEL_FORMAT_CAP_NONE                         = 0x00000000,
  QDI_PIXEL_FORMAT_CAP_FORMAT_SUPPORTED             = 0x00000001,    /**< Flag that is used to inform QDI client whether the pixel format is supported */
  QDI_PIXEL_FORMAT_CAP_A4X_MACRO_TILE               = 0x00000002,    /**< Flag that is used to inform QDI client whether the A4X macro tile pixel format is supported*/
  QDI_PIXEL_FORMAT_CAP_A5X_MACRO_TILE               = 0x00000004,    /**< Flag that is used to inform QDI client whether the A5X macro tile pixel format is supported*/
  QDI_PIXEL_FORMAT_CAP_BWC                          = 0x00000100,    /**< Flag that is used to inform QDI client whether the BWC (bandwidth compression) is supported for pixel format*/
  QDI_PIXEL_FORMAT_CAP_UBWC                         = 0x00000200,    /**< Flag that is used to inform QDI client whether the UBWC (universal bandwidth compression) is supported for pixel format*/
  QDI_PIXEL_FORMAT_CAP_HSYNC_LINEAR                 = 0x00000400,    /**< Flag that is used to inform QDI client whether the hsync flip on Linear surface is supported for the pixel format */
  QDI_PIXEL_FORMAT_CAP_HSYNC_UBWC                   = 0x00000800,    /**< Flag that is used to inform QDI client whether the hsync flip on UBWC surface is supported for the pixel format */
  QDI_PIXEL_FORMAT_CAP_FORCE_32BIT                  = 0x7FFFFFFF
}QDI_PixelFormatCapType;

/*!
 * \b QDI_PartialUpdateCapType
 * 
 * Flags to indicate the partial update support type
 *
 */

typedef enum
{
  QDI_PARTIAL_UPDATE_NOT_SUPPORTED              = 0x00000000,  /**< Flag that is used to inform QDI client that the partial update is not supported*/
  QDI_PARTIAL_UPDATE_WITH_SRC_RECT              = 0x00000001,  /**< Flag that is used to inform QDI client that partial update is supported with source partial update rectangle*/
  QDI_PARTIAL_UPDATE_WITH_DEST_RECT             = 0x00000002,  /**< Flag that is used to inform QDI client that partial update is supported with destination partial update rectangle*/
  QDI_PARTIAL_UPDATE_MAX,
  QDI_PARTIAL_UPDATE_FORCE_32BIT                = 0x7FFFFFFF
}QDI_PartialUpdateCapType;


/*!
 * \b QDI_WireFormatType
 * 
 * Bitmask to indicate what formats are supported across the wire
 *
 */

typedef enum
{
  QDI_WIRE_FORMAT_UNKNOWN                       = 0x00000000,  /**< Unknown or undefined wire format */
  QDI_WIRE_FORMAT_RGB444                        = 0x00000001,  /**< Fully sampled RGB */
  QDI_WIRE_FORMAT_YUV444                        = 0x00000002,  /**< Fully sampled YUV */
  QDI_WIRE_FORMAT_YUV422                        = 0x00000004,  /**< Sub-sampled YUV 422 */
  QDI_WIRE_FORMAT_YUV420                        = 0x00000008,  /**< Sub-Sampled YUV 420 */  
  QDI_WIRE_FORMAT_MAX,
  QDI_WIRE_FORMAT_FORCE_32BIT                   = 0x7FFFFFFF
}QDI_WireFormatType;


/*!
 * \b QDI_BltFlags
 * 
 * Flags to indicate hint for blt engines
 *
 */
typedef enum
{
  QDI_BLT_FIRST_FRAME_HINT                      = 0x00000001,  /**< Set only for first frame of the video stream */
}QDI_BltFlags;

/*!
 * Flag that can be used for QDI_Device_Commit. The function is a non-blocking call with this flag.
 */
#define  QDI_FLAG_DEVICE_COMMIT_ASYNC                 0x00000001

/*!
* Flag that can be used for QDI_Device_Close. It will force to shutdown all the instances of QDI
*/
#define QDI_FLAG_DEVICE_FORCE_CLOSE                   0x00000100

/*!
* NOTE: This FLAG should NOT set for normal QDI usage and it should be ONLY used for diagnostics 
* purposes.
*/
#define  QDI_FLAG_DEVICE_DIAG_MODE_DIRECT_OUTPUT      0x00010000

/*! 
* Flag that can be used for QDI_Gfx_Blt. The function is a non-blocking call with this flag. 
*/ 
#define  QDI_FLAG_DEVICE_BLT_ASYNC                    0x00002000

/*! 
* Flag that can be used for QDI_Util_AllocSurface. This flag determines which memory bank to allocate from
* for better memory latency requirements 
*/ 
#define  QDI_FLAG_UTIL_ALLOC_FAST_MEM                 0x00000001

/*! 
* Flag that can be used for QDI_Display_HDMI_CEC_PropType. This flag determines if the CEC message is 
* intended to be a ping/polling message
*/ 
#define  QDI_FLAG_DISPLAY_HDMI_CEC_PING_MESSAGE       0x00000001

/*!
* Flags that can be used by the input of QDI_MultiPipe_InfoType to reserve layers even they are not
* physically used in multi-pipe mode when layer requirements are being calculated. Set this flag if QDI client
* does not intend to call QDI_Util_GetInfo for every commit
*/
#define  QDI_FLAG_UTIL_MULTIPIPE_RESERVE_LAYER        0x00000001

/*!
 * \b QDI_SurfaceAttrType Flags
 */

/*! 
* Flag that can be used to inform QDI driver to use QDI_PixelPlaneTypeEx, QDI_FormatYuvPackedPlaneTypeEx,
* QDI_FormatYuvPsuedoPlanarPlaneTypeEx, QDI_FormatYuvPlanarPlaneTypeEx instead of QDI_PixelPlaneType,
* QDI_FormatYuvPackedPlaneType, QDI_FormatYuvPsuedoPlanarPlaneType, QDI_FormatYuvPlanarPlaneType in 
* QDI_SurfaceAttrType data structure
*/ 
#define  QDI_FLAG_SURFACE_ATTR_EX_TYPE                0x00000001

/*!
* Flag that can be used to inform QDI driver about the YUV Range for the surface in the
* QDI_SurfaceAttrType data structure
*     If flag is set - Full Range YUV  (0-255)
*     Default value  - Studio Range YUV (16-235)
*/
#define  QDI_FLAG_SURFACE_ATTR_FULL_RANGE_YUV         0x00000002

/*!
* Flag that can be used to inform QDI driver whether the surface is compressed 
*/
#define  QDI_FLAG_SURFACE_ATTR_COMPRESSED             0x00000004

/*!
 * Flag that can be used to inform QDI driver whether the format is macro tile
 */
#define QDI_FLAG_SURFACE_ATTR_MACRO_TILE              0x00000008

/*!
* Flag that can be used to inform the QDI driver whether the surface is allocated in a secure context bank
*/
#define QDI_FLAG_SURFACE_ATTR_SECURE_ALLOCATION       0x00000010

/*!
* Define a common "handle type" to be used by all QDI components.
*/
typedef void   *QDI_HandleType;

/*!
* \b QDI_FlagsType
*
* Flags that could be applied as an option to an existing feature.
*
*/
typedef uint32   QDI_FlagsType;

/*!
* \b QDI_ErrorType
* Define a common "handle type" to be used by all QDI components.
*/
typedef uint32   QDI_ErrorType;

/*!
* \b QDI_RectType
*
* Define the rectangle (rect) or region of interest (roi).  Define a 
* coordinate and expand with the pixel width and pixel height.  Specifying
* zero for either width or height will result in a zero area rectangle
*/
typedef struct
{
  QDI_CordType           sCordXY; /**< Define the top left most corner of rectangle */
  uint32                 uWidthX; /**< Define the width of rectangle */
  uint32                 uHeightY;/**< Define the height of rectangle */
} QDI_RectType;

/*!
* \b QDI_PixelPlaneType
*
* Define plane offset and stride.
*
*/
typedef struct 
{
  uint8                   *pOffset;       /**< Data offset */
  int32                    iStrideInByte; /**< Stride of a scan line in bytes */
} QDI_PixelPlaneType;

/*!
* \b QDI_PixelPlaneTypeEx
*
* Define plane offset and stride.
*
*/
typedef struct 
{
  QDI_AddressType          sPlaneAddress; /**< Data offset  */
  uint32                   uStrideInByte; /**< Stride of a scan line in bytes */
} QDI_PixelPlaneTypeEx;


/*!
* \b QDI_FormatYuvPackedPlaneType
*
* Define the planes for YUV packed formats.
*
* This applies to the following color format:
*
* QDI_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
* QDI_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
* QDI_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
* QDI_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
*
*/
typedef struct 
{
  uint8                    *pOffset;                  /**< Data offset */
  int32                     iStrideInByte;            /**< Stride of a scan line in bytes */
  bool32                    bChromaOffSite;           /**< Source chroma siting, True denotes the chroma are not subsampled at the same time as luma. Applied for YUV format*/
  bool32                    bChromaVerticalPacked;    /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/
} QDI_FormatYuvPackedPlaneType;

/*!
* \b QDI_FormatYuvPackedPlaneTypeEx
*
* Define the planes for YUV packed formats.
*
* This applies to the following color format:
*
* QDI_PIXEL_FORMAT_YCRYCB_H2V1_16BPP
* QDI_PIXEL_FORMAT_YCBYCR_H2V1_16BPP
* QDI_PIXEL_FORMAT_CRYCBY_H2V1_16BPP
* QDI_PIXEL_FORMAT_CBYCRY_H2V1_16BPP
*
*/
typedef struct 
{
  QDI_AddressType           sPlaneAddress;            /**< Data offset  */
  uint32                    uStrideInByte;            /**< Stride of a scan line in bytes */
  bool32                    bChromaOffSite;           /**< Source chroma siting, True denotes the chroma are not subsampled at the same time as luma. Applied for YUV format*/
  bool32                    bChromaVerticalPacked;    /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/
} QDI_FormatYuvPackedPlaneTypeEx;


/*!
* \b QDI_FormatYuvPsuedoPlanarType
*
* Define the YUV planes for pseudo planar color formats.
*
* This applies to the following color format:
*
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP (4:2:0, Pseudo planar, Cb in MSB)
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP (4:2:0, Pseudo planar, Cr in MSB)
* QDI_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP (4:2:2, Pseudo planar, Cr in MSB)
* QDI_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP (4:2:2, Pseudo planar, Cb in MSB)
* QDI_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
* QDI_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP
* QDI_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
*/
typedef struct 
{
  uint8                     *pLumaOffset;              /**< Luma data offset */
  int32                      iLumaStrideInByte;        /**< Stride of a scan line in bytes for luma*/
  uint8                     *pChromaOffset;            /**< Chroma data offset */
  int32                      iChromaStrideInByte;      /**< Stride of a scan line in bytes for chroma*/
  bool32                     bChromaOffSite;           /**< Source chroma siting, True denotes the chroma are not subsampled at the same time as luma. Applied for YUV format*/
  bool32                     bChromaVerticalPacked;    /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/
} QDI_FormatYuvPsuedoPlanarPlaneType;

/*!
* \b QDI_FormatYuvPsuedoPlanarTypeEx
*
* Define the YUV planes for pseudo planar color formats.
*
* This applies to the following color format:
*
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP (4:2:0, Pseudo planar, Cb in MSB)
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP (4:2:0, Pseudo planar, Cr in MSB)
* QDI_PIXEL_FORMAT_Y_CRCB_H2V1_16BPP (4:2:2, Pseudo planar, Cr in MSB)
* QDI_PIXEL_FORMAT_Y_CBCR_H2V1_16BPP (4:2:2, Pseudo planar, Cb in MSB)
* QDI_PIXEL_FORMAT_Y_CBCR_H2V2_VC1_12BPP
* QDI_PIXEL_FORMAT_Y_CRCB_H2V2_VC1_12BPP
* QDI_PIXEL_FORMAT_Y_CBCR_SUPERTILE_4x2_12BPP
*/
typedef struct 
{
  QDI_AddressType             sLumaPlaneAddress;        /**< Luma data offset */
  uint32                      uLumaStrideInByte;        /**< Stride of a scan line in bytes for luma*/
  QDI_AddressType             sChromaPlaneAddress;      /**< Chroma data offset  */
  uint32                      uChromaStrideInByte;      /**< Stride of a scan line in bytes for chroma*/
  bool32                      bChromaOffSite;           /**< Source chroma siting, True denotes the chroma are not subsampled at the same time as luma. Applied for YUV format*/
  bool32                      bChromaVerticalPacked;    /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/
} QDI_FormatYuvPsuedoPlanarPlaneTypeEx;

/*!
* \b QDI_FormatYuvPlanarPlaneType
*
* Define the YUV planes for planar color formats.
*
* This applies to the following color format:
*
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP
* QDI_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP
* QDI_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP
*/
typedef struct 
{
  uint8                      *pLumaOffset;                /**< Luma data offset */
  int32                       iLumaStrideInByte;          /**< Stride of a scan line in bytes for luma*/
  uint8                      *pChromaBlueOffset;          /**< Chroma data offset */
  int32                       iChromaBlueStrideInByte;    /**< Stride of a scan line in bytes for chroma blue*/
  uint8                      *pChromaRedOffset;           /**< Chroma data offset */
  int32                       iChromaRedStrideInByte;     /**< Stride of a scan line in bytes for chroma red*/
  bool32                      bChromaOffSite;             /**< Source chroma siting, True denotes the chroma are not subsampled at the same time as luma. Applied for YUV format*/
  bool32                      bChromaVerticalPacked;      /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/
} QDI_FormatYuvPlanarPlaneType;

/*!
* \b QDI_FormatYuvPlanarPlaneTypeEx
*
* Define the YUV planes for planar color formats.
*
* This applies to the following color format:
*
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V2_12BPP
* QDI_PIXEL_FORMAT_Y_CR_CB_H2V1_16BPP
* QDI_PIXEL_FORMAT_Y_CB_CR_H2V2_12BPP
* QDI_PIXEL_FORMAT_Y_CB_CR_H2V1_16BPP
*/
typedef struct 
{
  QDI_AddressType             sLumaPlaneAddress;          /**< Luma data offset  */
  uint32                      uLumaStrideInByte;          /**< Stride of a scan line in bytes for luma*/
  QDI_AddressType             sChromaBluePlaneAddress;    /**< Chroma data offset  */
  uint32                      uChromaBlueStrideInByte;    /**< Stride of a scan line in bytes for chroma blue*/
  QDI_AddressType             sChromaRedPlaneAddress;     /**< Chroma data offset  */
  uint32                      uChromaRedStrideInByte;     /**< Stride of a scan line in bytes for chroma red*/
  bool32                      bChromaOffSite;             /**< Source chroma siting, True denotes the chroma are not subsampled at the same time as luma. Applied for YUV format*/
  bool32                      bChromaVerticalPacked;      /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/
} QDI_FormatYuvPlanarPlaneTypeEx;


/*!
* \b QDI_FormatCompressedPlaneType
*
* Define the planes for compressed surface.
*
* BWC (Bandwidth compression) Note:
* BWC compressed surface contains 2 planes regardless of pixel format:
* - Compressed pixel data plane (pCompressedPlane0Offset/uCompressedPlane0StrideInByte)
* - Meta data plane (pMetadataPlane0Offset/uMetadataPlane0StrideInByte)
*
* UBWC (Universal bandwidth compression) Note: 
* UBWC compression is applied for each plane.
* UBWC compressed pseudo planar surface contains:
* - Y-meta data plane (pMetadataPlane0Offset/uMetadataPlane0StrideInByte)
* - Y-compressed pixel data plane (pCompressedPlane0Offset/uCompressedPlane0StrideInByte)
* - UV-meta data plane (pMetadataPlane1Offset,uMetadataPlane1StrideInByte)
* - UV-compressed pixel data plane (pCompressedPlane1Offset/uCompressedPlane1StrideInByte)
* UBWC compressed interleaved surface contains:
* - Meta data plane (pMetadataPlane0Offset/uMetadataPlane0StrideInByte)
* - Compressed pixel data plane (pCompressedPlane0Offset/uCompressedPlane0StrideInByte)
*
* UBWC compression memory layout requirements:
* - Meta data/compressed pixel data planes should be allocated in virtually contiguous memory,  compressed pixel data plane adjacent to meta data plane.
* - Meta data/compressed pixel data plane starting address has to be 4K aligned
* - Meta data/compressed pixel data plane size has to be 4K aligned.
*/
typedef struct
{
  uint8                    *pCompressedPlane0Offset;       /**< Compressed pixel data plane starting address, Y-compressed pixel data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uCompressedPlane0StrideInByte; /**< Stride in bytes of compressed pixel data plane, Y-compressed pixel data plane stride for UBWC compressed YUV pseudo planar format */
  uint8                    *pMetadataPlane0Offset;         /**< Meta data plane starting address, Y-meta data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uMetadataPlane0StrideInByte;   /**< Stride in bytes of meta data plane, Y-meta data plane stride for UBWC compressed YUV pseudo planar format */
  uint8                    *pCompressedPlane1Offset;       /**< UV-compressed pixel data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uCompressedPlane1StrideInByte; /**< UV-compressed pixel data plane stride for UBWC compressed YUV pseudo planar format */
  uint8                    *pMetadataPlane1Offset;         /**< UV-meta data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uMetadataPlane1StrideInByte;   /**< UV-meta data plane stride for UBWC compressed YUV pseudo planar format */
  bool32                    bChromaVerticalPacked;         /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/
} QDI_FormatCompressedPlaneType;


/*!
* \b QDI_FormatCompressedPlaneTypeEx
*
* Define the planes for compressed surface.
*
* BWC (Bandwidth compression) Note:
* BWC compressed surface contains 2 planes regardless of pixel format:
* - Compressed pixel data plane (sCompressedPlane0Address/uCompressedPlane0StrideInByte)
* - Meta data plane (sMetadataPlane0Address/uMetadataPlane0StrideInByte)
*
* UBWC (Universal bandwidth compression) Note:
* UBWC compression is applied for each plane.
* UBWC compressed pseudo planar surface contains:
* - Y-meta data plane (sMetadataPlane0Address/uMetadataPlane0StrideInByte)
* - Y-compressed pixel data plane (sCompressedPlane0Address/uCompressedPlane0StrideInByte)
* - UV-meta data plane (sMetadataPlane1Address,uMetadataPlane1StrideInByte)
* - UV-compressed pixel data plane (sCompressedPlane1Address/uCompressedPlane1StrideInByte)
* UBWC compressed interleaved surface contains:
* - Meta data plane (sMetadataPlane0Address/uMetadataPlane0StrideInByte)
* - Compressed pixel data plane (sCompressedPlane0Address/uCompressedPlane0StrideInByte)
* UBWC compressed planar surface is not supported.
*
* UBWC compression memory layout requirements:
* - Meta data/compressed pixel data planes should be allocated in virtually contiguous memory, compressed pixel data plane adjacent to meta data plane.
* - Meta data/compressed pixel data plane starting addresses has to be 4K aligned
* - Meta data/compressed pixel data plane size has to be 4K aligned.
*/
typedef struct 
{ 
  QDI_AddressType           sCompressedPlane0Address;      /**< Compressed pixel data plane starting address, Y-compressed pixel data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uCompressedPlane0StrideInByte; /**< Stride in bytes of compressed pixel data plane, Y-compressed pixel data plane stride for UBWC compressed YUV pseudo planar format */
  QDI_AddressType           sMetadataPlane0Address;        /**< Meta data plane starting address, Y-meta data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uMetadataPlane0StrideInByte;   /**< Stride in bytes of meta data plane, Y-meta data plane stride for UBWC compressed YUV pseudo planar format */
  QDI_AddressType           sCompressedPlane1Address;      /**< UV-compressed pixel data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uCompressedPlane1StrideInByte; /**< UV-compressed pixel data plane stride for UBWC compressed YUV pseudo planar format */
  QDI_AddressType           sMetadataPlane1Address;        /**< UV-meta data plane address for UBWC compressed YUV pseudo planar format */
  uint32                    uMetadataPlane1StrideInByte;   /**< UV-meta data plane stride for UBWC compressed YUV pseudo planar format */
  bool32                    bChromaVerticalPacked;         /**< CbCr packed direction, when source is rotated 90, this should be TRUE, applied for YUV format*/

} QDI_FormatCompressedPlaneTypeEx;

/*!
 * \b QDI_ QDI_HISCSettingsType
 *
 *  Defines QDI HSIC control info 
 */

typedef struct
{
  bool32                    bEnabled;            /** indicates if HSIC is enabled or not */
  int32                     iHueLevel;           /** Hue level (absolute range defined by device caps) */
  int32                     iSaturationLevel;    /** Saturation level (absolute range defined by device caps) */
  int32                     iIntensityLevel;     /** Intensity level (absolute range defined by device caps) */
  int32                     iContrastLevel;      /** Contrast level from (absolute range defined by device caps) */
} QDI_HISCSettingsType;


/*!
 * \b QDI_SurfaceAttrType       
 *
 * Defines the attributes for memory which will be used as a display
 * frame buffer.  
 */

typedef struct
{
  QDI_PixelFormatType           ePixelFormat;         /**< Pixel format.    */
  uint32                        uWidthInPixel;        /**< Width in pixel   */
  uint32                        uHeightInPixel;       /**< Height in pixel  */
  union                                        
  {
    QDI_PixelPlaneType                    sRgb;                /**< RGB plane                               */
    QDI_FormatYuvPackedPlaneType          sYuvPacked;          /**< YUV packed plane                        */
    QDI_FormatYuvPsuedoPlanarPlaneType    sYuvPsuedoPlanar;    /**< YUV pseudo planar planes                */
    QDI_FormatYuvPlanarPlaneType          sYuvPlanar;          /**< YUV planar planes                       */
    QDI_PixelPlaneTypeEx                  sRgbEx;              /**< RGB plane                               */
    QDI_FormatYuvPackedPlaneTypeEx        sYuvPackedEx;        /**< YUV packed plane                        */
    QDI_FormatYuvPsuedoPlanarPlaneTypeEx  sYuvPsuedoPlanarEx;  /**< YUV pseudo planar planes                */
    QDI_FormatYuvPlanarPlaneTypeEx        sYuvPlanarEx;        /**< YUV planar planes                       */
    QDI_FormatCompressedPlaneType         sCompressedPlane;    /**< Compressed plane                        */
    QDI_FormatCompressedPlaneTypeEx       sCompressedPlaneEx;  /**< Compressed plane                        */
  }uPlanes;  
  uint32                                  uFlags;              /**< Flag to inform the special functionality*/
}QDI_SurfaceAttrType;

/*!
* \b QDI_Status
*
* Enumeration giving return values from QDI function calls.
*
*/
typedef enum
{
  QDI_STATUS_OK = 0,                   /* Command successful */
  QDI_STATUS_SUCCESS = QDI_STATUS_OK,
  QDI_STATUS_BAD_HANDLE,               /* Bad Destination Handle */
  QDI_STATUS_BAD_PARAM,                /* Bad Parameter */
  QDI_STATUS_NOT_SUPPORTED,            /* Operation not supported */
  QDI_STATUS_NO_RESOURCES,             /* Out of resources */
  QDI_STATUS_FAILED,                   /* Command Failed */
  QDI_STATUS_TIMEOUT,                  /* Command not completed within expected time */
  QDI_STATUS_HW_ERROR,                 /* Hardware isn't responding as expected */
  QDI_STATUS_INVALID_STATE,            /* Not in correct state to perform task */
  QDI_STATUS_BUFFER_TOO_SMALL,         /* A function needs a larger buffer for the amount of data*/
  QDI_STATUS_MAX,
  QDI_STATUS_FORCE_32BIT = 0x7FFFFFFF
} QDI_Status;


/* -----------------------------------------------------------------------
** 1. Gfx Blt operations
** ----------------------------------------------------------------------- */

/*!
* \b QDI_GfxBlt_EngineId
*
*  Blt engine enumerations, identifies a unique hardware resource for memory to memory operations.
*
*/
typedef enum 
{
  QDI_GFXBLT_ENGINE_NONE         = 0,     /**< Reserved ID */
  QDI_GFXBLT_ENGINE_ROTATOR,              /**< Rotator engine ID, dedicated engine for rotation operations */
  QDI_GFXBLT_ENGINE_MDP_BLT,              /**< Blt engine ID, dedicated engine used for color conversion/blend operations */
  QDI_GFXBLT_ENGINE_MAX,
  QDI_GFXBLT_ENGINE_FORCE_32BIT  = 0x7FFFFFFF
} QDI_GfxBlt_EngineId;

/*!
* \b QDI_Layer_SharpeningModeType
*
*  Defines the smoothing and sharpening features
*/
typedef enum
{
  QDI_LAYER_SHARPENINGMODE_NONE = 0,      /**< No effects on Images*/
  QDI_LAYER_SHARPENINGMODE_SHARPEN,       /**< Sharpening effects on Images */
  QDI_LAYER_SHARPENINGMODE_SMOOTH,        /**< Smoothing effects on Images  */
  QDI_LAYER_SHARPENINGMODE_MAX,
  QDI_LAYER_SHARPENINGMODE_FORCE_32BIT = 0x7FFFFFFF 
} QDI_Layer_SharpeningModeType;

/*!
 * \b QDI_BlendOpType
 *
 *  Defines information about blending parameters for layer
 *
 */
typedef struct
{
  bool32             bAlphaPerPixel;      /**< Use per pixel alpha */
  uint32             uConstAlphaValue;    /**< Constant alpha, range [0, 0xFF], valid when bAlphaPerPixel is False */
  bool32             bUseNonNativeAlpha;  /**< Select alpha from opposite layer */
  bool32             bModulateAlpha;      /**< Constant alpha modulates the alpha selected */
  bool32             bInverseAlpha;       /**< Inverse alpha is used */
} QDI_BlendOpType;

/*!
* \b QDI_GfxBltBlendType
*
* Use this structure to set Gfx Blt blend specific operations.
*/
typedef struct
{
  bool32                      bEnableAlphaBlend;   /**< Alpha blend */
  QDI_BlendOpType             sFgSurfBlendOp;      /**< Alpha blend setting applied to foreground surface */
  QDI_BlendOpType             sBgSurfBlendOp;      /**< Alpha blend setting applied to background surface */
}QDI_GfxBltBlendType; 

/*!
* \b QDI_GfxBltColorKeyType
*
* Use this structure to set Gfx Blt blend specific operations.
*/
typedef struct
{
  bool32                      bEnableFgColorKey;   /**< Enable foreground color key */
  uint32                      uFgColorKey;         /**< Foreground color key, same format as foreground surface. */
  bool32                      bEnableBgColorKey;   /**< Enable background color key */
  uint32                      uBgColorKey;         /**< Background color key, same format as foreground surface. */
}QDI_GfxBltColorKeyType; 

/*!
* \b QDI_GfxBltOpType
*
* Use this structure to set Gfx blt specific operations.
*/
typedef struct
{
  QDI_RotateFlipType          eRotateFlip;         /**< Fixed rotation (90 degrees) */
  QDI_GfxBltBlendType         sGfxBltBlend;        /**< Blend setting */
  QDI_GfxBltColorKeyType      sGfxBltColorKey;     /**< Color key setting */
  bool32                      bEnableSharpening;   /**< Enable sharpening */
  uint32                      uSharpenPercent;     /**< 16.16 fixed format. Range [0..0x10000] */
}QDI_GfxBltOpType; 

/*!
 * \b QDI_GfxBltSurf
 *
 * Use this structure to set Gfx blt specific operations.
 */
typedef struct
{
  QDI_RectType                 sSrcRect;               /**< Source cropping rectangle */
  QDI_RectType                 sDestRect;              /**< Destination output rectangle. */
  QDI_SurfaceAttrType          sSurface;               /**< Surface */
  QDI_Color_EncodingType       eVideoTransferMatrix;   /**< Either REC709 or REC601 can be specified for YUV to RGB conversion, default is REC601. */
  QDI_DeinterlaceType          eDeinterlaceMethod;     /**< Specifies the Type of Deinterlacing for the surface*/
  bool32                       bDeinterlaceRefOddField;/**< 1== reference odd fields for deinterlacing. Default is even  */

  QDI_FlipType                 *pFlipType;             /**< Defines horizontal (left/right) mirroring, or vertical (up/down) mirroring, or both*/
  QDI_GfxBltBlendType          *pGfxBltBlend;          /**< Blend setting */
  QDI_GfxBltColorKeyType       *pGfxBltColorKey;       /**< Color key setting */
  uint32                       *pSharpenPercent;       /**< Percentage of smoothing/sharpening. 0 to 100 %. */   
  QDI_HISCSettingsType         *pHSICSettings;         /**< HSIC configuration */
  QDI_VC1RangeMapType          *pVc1RangeMap;          /**< Specifies VC1 Range Mapping levels for Luma and Chroma*/
  QDI_Layer_SharpeningModeType eSharpMode;             /**< Sharpening mode */
} QDI_GfxBltSurf; 


/*!
 * \b QDI_GfxBltOpenParamsType
 *
 *  Gfx Blt open structure
 */

typedef struct 
{
  QDI_GfxBlt_EngineId eBltEngine;                       /** Blt engine ID to open a handle to      */
  uint32              uInstanceId;                      /** Hardware instance ID of the eBltEngine */
  QDI_FlagsType       flags;                            /** Reserved for future usage              */
} QDI_GfxBltOpenParamsType;


/* -----------------------------------------------------------------------
** 1. Device
** ----------------------------------------------------------------------- */

/** Defines maximum number of scale factor values */
#define                QDI_MAX_DISCRETE_SCALE_FACTORS           8

/*  */
/*!
* \b QDI_ScaleStepMethodType
*
*  Structure defines the sequence type for parameter set.
*
*/
typedef enum
{
  /**< Continuous means that parameter can take every values from min to max  */
  QDI_STEP_CONTINUOUS,

  /**< Parameter can take every integer values from min to max with stepSize(power(stepNumber)),
  if stepSize =2 and min value = 2, then parameter can be minValue*step_size(power(set_Number)):
  firstElement=2*2(power 0) = 2,second = 2*2(power 1) = 4,third = 2*2(power 2) =8 ..(step_Number starts from 0).   */
  QDI_STEP_LOGARITHMIC,

  /**< This step method is used for undefined sequence (ex: 1.5, 2, 4, 8, 16, 32, 64).
  There values are described in Q16.16 format in QDI_ScalePropertyType.uArray[].
  QDI_ScalePropertyType.uMinValue and QDI_ScalePropertyType.uMaxValue represents valid range of QDI_ScalePropertyType.uArray.
  For example, if uArray[] = {0x18000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000}, uMinValue = 0, uMaxValue = 7
  Scale factor of 1.5 can be achieved by: ((uArray[0] >> 16) + ((uArray[0] & 0xFFFF)/0x10000)) = (1+(0x8000/0x10000)) = 1.5,
  Scale factor of 2 can be achieved by: ((uArray[1] >> 16) + ((uArray[1] & 0xFFFF)/0x10000)) = (2+0) = 2 and so on*/
  QDI_STEP_ARRAY,

  QDI_STEP_MAX,
  QDI_STEP_FORCE_32BIT = 0x7FFFFFFF
} QDI_ScaleStepMethodType;

/*  */
/*!
* \b QDI_ScalePropertyType
*
*  Defines data to calculate parameters set.
*  This structure specifies basic rules of some sequence of elements.
*  Use this structure to calculate every element in this sequence.
*
*/
typedef struct
{
  QDI_ScaleStepMethodType       eStepType;                                 /**< Sequence type for parameter set */
  uint32                        uStepSize;                                 /**< Step size for set of parameters  */
  uint32                        uMinValue;                                 /**< Minimum value in the set */
  uint32                        uMaxValue;                                 /**< Maximum value in the set */
  int32                         uArray[QDI_MAX_DISCRETE_SCALE_FACTORS];    /**< Array containing scale factor values sorted from lowest to highest in Q16.16 format */ 
} QDI_ScalePropertyType;


/*  */
/*!
* \b QDI_ScalingCapsType
*
*  Describes scalar capabilities.
*
*/
typedef struct
{
  bool32                        bDownscaleSupported;    /**< TRUE if downscaling is supported  */
  bool32                        bUpscaleSupported;      /**< TRUE if upscaling is supported  */
  QDI_ScalePropertyType         sDownscaleRatio;        /**< Data to calculate downscale ratio */
  QDI_ScalePropertyType         sUpscaleRatio;          /**< Data to calculate upscale ratio */
}QDI_ScalingCapsType;


/*!
* \b QDI_Device_DisplayCapsType
*
* Structure containing information pertaining to a particular
* device.
*
*/
typedef struct                 
{
  uint32    uMaxDisplays;                   /**< max number of displays in same time.*/    
  uint32    uMaxPlugableDisplay;            /**< max number of pluggable displays in same time.*/
  uint16    uMaxSurfaceWidth;               /**< max supported input surface width in pixels. */
  uint16    uMaxSurfaceHeight;              /**< max supported input surface height in pixels. */
  uint32    uMaxLineInterrupts;             /**< max supported line interrupts */
  uint32    uReserved[4];                   /**< Reserved for future usage.*/
}QDI_Device_DisplayCapsType;

/*!
* \b QDI_Device_LayerCapsType                
*
* Structure containing layer caps based on device version.
* device.
*
*/
typedef struct               
{
  uint32  uMaxRGBLayers;      /**< Max RGB layers in physical device.*/ 
  uint32  uMaxVideoLayers;    /**< Max video layers in physical device.*/
  uint32  uMaxDMALayers;      /**< Max DMA layers in physical device.*/
  uint32  uMaxCursorLayers;   /**< Max cursor layers */
  uint32  uReserved[6];       /**< Reserved for future use.*/
}QDI_Device_LayerCapsType;

/*!
* \b QDI_Device_BltCapsType;
*
* Structure containing information pertaining to blt capabilities.
*
*/
typedef struct
{
  bool32              bHWAcceleratedBlt;                                   /**< This device have support for blt acceleration */
  bool32              bHWAccleleratedRotation;                             /**< This device have support for rotation acceleration */
  bool32              bBltBlendngSupported;                                /**< Blending capability of the blt operation */
  uint32              uBltMaxNumberStreams;                                /**< Maximum number of streams supported for blending */
  bool32              bBltOutputFormatsSupported[QDI_PIXEL_FORMAT_MAX];    /**< List of all of the supported blt output formats */
  uint32              uBltOutputPixelFormatCaps[QDI_PIXEL_FORMAT_MAX];     /**< List of all of the supported blt output formats. Please refer to QDI_PixelFormatCapType*/
  uint32              uRotatorOutputPixelFormatCaps[QDI_PIXEL_FORMAT_MAX]; /**< List of all of the supported rotator output formats. Please refer to QDI_PixelFormatCapType*/
  uint32              uMaxUpscaleRatio;                                    /**< The maximum up-scale ratio of the blt engine per input stream*/
  uint32              uMaxDownscaleRatio;                                  /**< The maximum down-scale ratio of the blt engine per input stream*/
  uint32              uMaxRotatorEngines;                                  /**< The number of rotator HW engines available for use */
  uint32              uMaxBltEngines;                                      /**< The number of blt HW engines available for use */
  QDI_ScalingCapsType sRotatorScalarCaps;                                  /**< Rotator scalar caps */
} QDI_Device_BltCapsType;  



/*!
* \b QDI_Device_CapsType
*
* Structure containing information describing device capabilities
*
*/
typedef struct
{
  bool32 bPictureAdjustment;                                  /**< This device supports picture adjustment */
  uint32 uHueRange;                                           /**< The valid absolute range of hue adjustment */
  uint32 uIntensityRange;                                     /**< The valid absolute range of intensity adjustment */
  uint32 uSaturationRange;                                    /**< The valid absolute range of saturation adjustment */
  uint32 uContrastRange;                                      /**< The valid absolute range of contrast adjustment */  
} QDI_Device_CapsType;

/*!
* \b QDI_Device_Version
*
* Describes HW version that QDI supports.
* 
*/
typedef enum
{
  QDI_DEVICE_VERSION_04_00 = 0,   /**< Device 4.0 */
  QDI_DEVICE_VERSION_04_01,       /**< Device 4.1 */
  QDI_DEVICE_VERSION_04_02,       /**< Device 4.2 */
  QDI_DEVICE_VERSION_04_03,       /**< Device 4.3 */
  QDI_DEVICE_VERSION_04_04,       /**< Device 4.4 */
  QDI_DEVICE_VERSION_05_00,       /**< Device 5.0 */
  QDI_DEVICE_VERSION_05_01,       /**< Device 5.1 */
  QDI_DEVICE_VERSION_05_02,       /**< Device 5.2 */
  QDI_DEVICE_VERSION_05_03,       /**< Device 5.3 */
  QDI_DEVICE_VERSION_05_04,       /**< Device 5.4 */
  QDI_DEVICE_VERSION_05_05,       /**< Device 5.5 */  
  QDI_DEVICE_VERSION_05_06,       /**< Device 5.6 */
  QDI_DEVICE_VERSION_05_07,       /**< Device 5.7 */
  QDI_DEVICE_VERSION_05_08,       /**< Device 5.8 */
  QDI_DEVICE_VERSION_05_09,       /**< Device 5.9 */
  QDI_DEVICE_VERSION_05_10,       /**< Device 5.10 */
  QDI_DEVICE_VERSION_05_11,       /**< Device 5.11 */
  QDI_DEVICE_VERSION_05_12,       /**< Device 5.12 */
  QDI_DEVICE_VERSION_05_13,       /**< Device 5.13 */
  QDI_DEVICE_VERSION_05_14,       /**< Device 5.14 */
  QDI_DEVICE_VERSION_05_15,       /**< Device 5.15 */
  QDI_DEVICE_VERSION_05_16,       /**< Device 5.16 */
  QDI_DEVICE_VERSION_05_17,       /**< Device 5.17 */
  QDI_DEVICE_VERSION_05_18,       /**< Device 5.18 */
  QDI_DEVICE_VERSION_05_19,       /**< Device 5.19 */
  QDI_DEVICE_VERSION_05_20,       /**< Device 5.20 */
  QDI_DEVICE_VERSION_03_00,       /**< Device 3.00 */
  QDI_DEVICE_VERSION_03_01,       /**< Device 3.01 */
  QDI_DEVICE_VERSION_03_02,       /**< Device 3.02 */
  QDI_DEVICE_VERSION_03_03,       /**< Device 3.03 */
  QDI_DEVICE_VERSION_03_04,       /**< Device 3.04 */
  QDI_DEVICE_VERSION_03_05,       /**< Device 3.05 */
  QDI_DEVICE_VERSION_06_00,       /**< Device 6.00 */
  QDI_DEVICE_VERSION_06_01,       /**< Device 6.01 */
  QDI_DEVICE_VERSION_06_02,       /**< Device 6.02 */
  QDI_DEVICE_VERSION_06_03,       /**< Device 6.03 */
  QDI_DEVICE_VERSION_06_04,       /**< Device 6.04 */
  QDI_DEVICE_VERSION_06_05,       /**< Device 6.05 */
  QDI_DEVICE_VERSION_07_00,       /**< Device 7.00 */
  QDI_DEVICE_VERSION_07_01,       /**< Device 7.01 */
  QDI_DEVICE_VERSION_07_02,       /**< Device 7.02 */
  QDI_DEVICE_VERSION_07_03,       /**< Device 7.03 */
  QDI_DEVICE_VERSION_07_04,       /**< Device 7.04 */
  QDI_DEVICE_VERSION_07_05,       /**< Device 7.05 */
  QDI_DEVICE_VERSION_08_00,       /**< Device 8.00 */
  QDI_DEVICE_VERSION_08_01,       /**< Device 8.01 */
  QDI_DEVICE_VERSION_08_02,       /**< Device 8.02 */
  QDI_DEVICE_VERSION_08_03,       /**< Device 8.03 */
  QDI_DEVICE_VERSION_08_04,       /**< Device 8.04 */
  QDI_DEVICE_VERSION_08_05,       /**< Device 8.05 */
  QDI_DEVICE_VERSION_09_00,       /**< Device 9.00 */
  QDI_DEVICE_VERSION_09_01,       /**< Device 9.01 */
  QDI_DEVICE_VERSION_09_02,       /**< Device 9.02 */
  QDI_DEVICE_VERSION_09_03,       /**< Device 9.03 */
  QDI_DEVICE_VERSION_09_04,       /**< Device 9.04 */
  QDI_DEVICE_VERSION_09_05,       /**< Device 9.05 */
  QDI_DEVICE_VERSION_10_75,       /**< Device 10.75 */
  QDI_DEVICE_VERSION_11_99,       /**< Device 11.99 */
  QDI_DEVICE_VERSION_MAX,
  QDI_DEVICE_VERSION_FORCE_32BIT = 0x7FFFFFFF
}QDI_Device_VersionType;


/*!
* \b QDI_Device_InfoType
*
* Structure containing information pertaining to a particular
* device.
*
*/
typedef struct
{
  QDI_Device_VersionType            eDeviceVersion;     /**< HW version that QDI support*/
  QDI_Device_DisplayCapsType        sDisplayCaps;       /**< Display caps*/ 
  QDI_Device_LayerCapsType          sLayerCaps;         /**< Layer caps*/
  QDI_Device_BltCapsType            sBltCaps;           /**< Blt caps*/
  QDI_Device_CapsType               sDeviceCaps;        /**< Device caps*/    
} QDI_Device_InfoType;

/*!
* \b QDI_Device_IDType
*  
*  Defines devices ID
*/
typedef enum
{
  QDI_DEVICE_ID1 = 0,
  QDI_DEVICE_ID_MAX,
  QDI_DEVICE_FORCE_32BIT =0x7FFFFFFF
} QDI_Device_IDType;

/*!
* \b QDI_DeviceOpenParamsType
*
* Parameters to initialize the device
*/
typedef struct 
{
  QDI_Device_IDType   eDevice_ID;        /**< ID of the device to be initialized */
  uint32              uVersion_ID;       /**< version ID of the QDI driver */
} QDI_Device_OpenParamsType;

typedef enum
{
  QDI_DEVICE_PROPERTY_IMEM_OWNER = 0,   /**< IMEM owner */
  QDI_DEVICE_PROPERTY_COMMIT_DONE,      /**< Commit finished  */
  QDI_DEVICE_PROPERTY_BLT_DONE,         /**< Rotator finished  */
  QDI_DEVICE_PROPERTY_DEVICE_STATUS,    /**< Dynamic information corresponding to the device, including display and layers */
  QDI_DEVICE_PROPERTY_MAX,
  QDI_DEVICE_PROPERTY_FORCE_32BIT = 0x7FFFFFFF
} QDI_Device_PropertyType;

typedef enum
{
  QDI_DEVICE_IMEM_OWNER_NONE = 0, /**< IMEM is not used.*/
  QDI_DEVICE_IMEM_OWNER_ROTATOR,  /**< Rotator owns IMEM */
  QDI_DEVICE_IMEM_OWNER_CAMERA,   /**< Camera owns IMEM */
  QDI_DEVICE_IMEM_OWNER_MAX,
  QDI_DEVICE_IMEM_OWNER_FORCE_32BIT = 0x7FFFFFFF
} QDI_Device_ImemOwnerType;


/*!
* \b QDI_Gfx_Blt_InfoType
*
*  Contains GfxBlt information about modes supported and feature limitations.
*  
*/
typedef struct
{
  bool32                          bRotation;                                        /**< TRUE if rotation supported. Refer to QDI_RotateFlipType for all supported rotation mode.  */
  bool32                          bHorizontalFlip;                                  /**< TRUE if Horizontal flip supported.  */
  bool32                          bVerticalFlip;                                    /**< TRUE if Vertical flip supported.  */
  QDI_ScalingCapsType             sScaleCaps;                                       /**< Contains scale capabilities  */
  bool32                          bInputPixelFormatSupported[QDI_PIXEL_FORMAT_MAX]; /**< Specifies input pixel formats supported  */
  bool32                          bOutputPixelFormatSupported[QDI_PIXEL_FORMAT_MAX];/**< Specifies output pixel formats supported  */
  uint32                          uReserved[16];
}QDI_Gfx_Blt_InfoType;


/*!
* \b QDI_Gfx_BltParamsType
*
*  Defines the parameters for Blt.
* Input/output surfaces, source and destination rectangle, Blt operation parameters.
*
*/
/*!
* \b QDI_Gfx_BltParamsType
*
*  Parameters for a rotation blt.
* 
*  Parameters:
*     sFgBltSurface     - Not supported
*     sBgBltSurface     - Rotator input surface
*     sOutputBltSurface - Rotator output surface
*     sFgBltRect        - Not supported.    
*     sBgBltRect        - Input ROI for source surface
*     sGfxBltOp         - Rotator operation
*     sOutputBltCord    - Starting offset in destination surface for rotator output
*
*  Supported Input/Output Formats:
*     RGB Input - RGB output (Same pixel depth is required)
*     YUV422 Input - Only linear YUV422 pseudo-planar output is supported. 
*     YUV420 (including tiled) Input - Only linear YUV420 pseudo-planar output is supported. 
*
*  Input Alignments:
*     RGB Formats - No alignment restrictions.
*     YUV Formats - Width and height must be a multiple of 2
*                   For both input surface and input ROI.
*    Tiled Format - Width must be aligned to 64 pixels, height must be aligned to 8 pixels.
*                   For both input surface and input ROI.
*
*  Output Size:
*     All Formats - Output surface must be equal to or larger than input ROI
*
*/
typedef struct
{
  QDI_SurfaceAttrType           sFgBltSurface;               /**< Foreground input surface, not supported for rotation. */
  QDI_SurfaceAttrType           sBgBltSurface;               /**< Background surface, used as input to rotator */   
  QDI_SurfaceAttrType           sOutputBltSurface;           /**< Rotator destination surface */   
  QDI_RectType                  sFgBltRect;                  /**< Foreground input surface ROI, not supported for rotation */    
  QDI_RectType                  sBgBltRect;                  /**< Background surface ROI */   
  QDI_GfxBltOpType              sGfxBltOp;                   /**< Rotation operation */     
  QDI_CordType                  sOutputBltCord;              /**< Destination output offset */   
  uint32                        uOutputWidth;                /**< Destination ROI Width  */
  uint32                        uOutputHeight;               /**< Destination ROI Height  */
  QDI_Color_EncodingType        eFgVideoTransferMatrix;      /**< Either REC709 or REC601 can be specified for YUV to RGB conversion, default is REC601. */
  QDI_Color_EncodingType        eBgVideoTransferMatrix;      /**< Either REC709 or REC601 can be specified for YUV to RGB conversion, default is REC601. */
  QDI_Color_EncodingType        eOutputVideoTransferMatrix;  /**< Either REC709 or REC601 can be specified for YUV to RGB conversion, default is REC601. */
  uint32                        uNumPasses;                  /**< Hint from caller that more than one pass is required */
  uint32                        uFrameRate;                  /**< Frame rate hint */
  uint32                        uFlags;                      /**< Uses flags defined in QDI_BltFlags */  
} QDI_Gfx_RotationParamsType, QDI_Gfx_BltParamsType;


/*!
 * \b QDI_Gfx_BlendParamsType
 *
 * Blt parameters for a MDP blt engine based operations
 */
typedef struct
{
  uint32                        uNumInputSurfaces;     /**< Number of source surface in pInputSurfList */
  QDI_GfxBltSurf                *pInputSurfList;       /**< Array of input surfaces including per surface operation */
  QDI_SurfaceAttrType           sOutputSurface;        /**< Output surface pointer */
  QDI_RectType                  sOutputROI;            /**< Output surface ROI */
  uint32                        uBackgroundColor;      /**< Background color (in ARGB8888 format) */
  QDI_Color_EncodingType        eVideoTransferMatrix;  /**< Either REC709 or REC601 can be specified for YUV to RGB conversion, default is REC601. */
  uint32                        uNumPasses;            /**< Hint from caller that more than one pass is required */
  uint32                        uFrameRate;            /**< Frame rate hint */
  uint32                        uFlags;                /**< Uses flags defined in QDI_BltFlags */
} QDI_Gfx_BlendParamsType;


/*!
 * \b QDI_Gfx_Blt2ParamsType
 *
 * Parameter for blt engine operation.
 */
typedef union
{
  QDI_Gfx_RotationParamsType  sRotationParams;  /**< Rotation parameters */
  QDI_Gfx_BlendParamsType     sBltParams;       /**< Blend parameters */
} QDI_Gfx_Blt2ParamsType;


/*!
 * \b QDI_Device_DeviceStatusType
 *
 *  Dynamic device information about the state of the driver.
 */
typedef struct
{
  QDI_AccessLevelType   eDisplaysAccessLevel[QDI_DISPLAY_ID_MAX];    /**< Array of the current access type for all displays */
  QDI_AccessLevelType   eLayersAccessLevel[QDI_LAYER_ID_MAX];        /**< Array of the current access type for all layers  */
} QDI_Device_DeviceStatusType;




typedef union
{
  QDI_Device_ImemOwnerType         eImemOwner;    /**< QDI_DEVICE_PROPERTY_IMEM_OWNER */
  bool32                           bCommitDone;   /**< QDI_DEVICE_PROPERTY_COMMIT_DONE */
  QDI_Device_DeviceStatusType      eDeviceStatus; /**< QDI_DEVICE_PROPERTY_DEVICE_STATUS */
} QDI_Device_PropertyParamsType;

/*!
* \b QDI_Device_CbEventType
*
* Describe the device callback events supported.  In general, one should
* avoid doing much work in a Callback routine - and best to just set an
* event/signal to other worker thread to handle the real work.  Callbacks
* will be called from the same thread/task in the order specified below
* starting with QDI_CB_ERROR.
* 
*/
typedef enum
{
  QDI_DEVICE_EVENT_NONE = 0,                             /**< none event  */
  QDI_DEVICE_EVENT_DEVICE_COMMIT_DONE,                   /**< Event ID for notifying client when all the pending request under a device handle is committed */
  QDI_DEVICE_EVENT_DISPLAY_COMMIT_DONE,                  /**< Event ID for notifying client when all the pending request under a display handle is committed */
  QDI_DEVICE_EVENT_LAYER_COMMIT_DONE,                    /**< Event ID for notifying client when all the pending request under a layer handle is committed */
  QDI_DEVICE_EVENT_DISPLAY_VSYNC_START,                  /**< Event ID to notify the client at the beginning of VSYNC */
  QDI_DEVICE_EVENT_ROTATION_DONE,                        /**< Event ID to notify the client when rotation is done*/
  QDI_DEVICE_EVENT_HDMI_PLUGGED,                         /**< Event ID to notify the client when detect HDMI is plugged-in. */
  QDI_DEVICE_EVENT_HDMI_UNPLUGGED,                       /**< Event ID to notify the client when detect HDMI is unplugged */
  QDI_DEVICE_EVENT_TVOUT_PLUGGED,                        /**< Event ID to notify the client when detect analog TV is plugged-in. */
  QDI_DEVICE_EVENT_TVOUT_UNPLUGGED,                      /**< Event ID to notify the client when detect analog TV is unplugged */
  QDI_DEVICE_EVENT_HDMI_CEC_MSG_RECEIVED,                /**< Event ID to notify the client upon receiving a Consumer Electronic Control (CEC) message from HDMI sink device */
  QDI_DEVICE_EVENT_HDMI_HDCP_AUTHENTICATE_SUCCEED,       /**< Event ID to notify the client when HDCP authentication succeed */
  QDI_DEVICE_EVENT_HDMI_HDCP_AUTHENTICATE_FAILED,        /**< Event ID to notify the client when HDCP authentication failed */
  QDI_DEVICE_EVENT_HDMI_CEC_MSG_SEND_COMPLETED,          /**< Event ID to notify the client when sending CEC message to HDMI sink device is completed*/
  QDI_DEVICE_EVENT_DISPLAY_ERROR,                        /**< Event ID to notify the client when display errors happen */
  QDI_DEVICE_EVENT_DISPLAY_FRAMEBUFFER_WRITEBACK_DONE,   /**< Event ID to notify the client when the pending writeback to framebuffer is completed by HW */
  QDI_DEVICE_EVENT_OEM_UPDATE,                           /**< Event ID to notify the client when the OEM event happens */
  QDI_DEVICE_EVENT_MAX,
  QDI_DEVICE_EVENT_FORCE_32BIT = 0x7FFFFFFF
}QDI_Device_CbEventType;

/*!
* \b QDI_Device_RegisterCallbackInfoType
*
* Define event parameters used to execute the event callback.
*/
typedef struct
{
  QDI_Device_CbEventType        eEventId;        /**< Event ID.*/
  union
  {
    QDI_HandleType              hDevice;        /**< Device handle for device related event */
    QDI_HandleType              hDisplay;       /**< Display handle for display related event */
    QDI_HandleType              hLayer;         /**< Layer handle for layer related event */
  }hHandle;
  QDI_FlagsType                 flags;          /**< flag */
  void                         *pData;          /**< data pointer passed in by the module registering this event/callback */
  void                         *pConfigData;    /**< a transparent pointer which the other module can dereference to use/modify the data */   
}QDI_Device_RegisterCallbackInfoType;

/*!
* \b QDI_Device_ReturnCallbackInfoType
*
* Package the function pointer and arguments to be used to execute the
* event callback.
*/
typedef struct
{
  QDI_Device_CbEventType        eEventId;       /**< Event ID.*/
  union
  {
    QDI_HandleType              hDevice;        /**< Device handle for device related event */
    QDI_HandleType              hDisplay;       /**< Display handle for display related event */
    QDI_HandleType              hLayer;         /**< Layer handle for layer related event */
  }hHandle;
  QDI_FlagsType                 flags;
  QDI_Status                    eStatus;         /**< event result */
  void                         *pData;           /**< data pointer which was passed in by the client module(which registered this event/callback) */
  void                         *pConfigData;     /**< a transparent pointer which the client module can dereference to use the data */   
}QDI_Device_ReturnCallbackInfoType;

/*!
* \b QDI_Device_CbFxnType
*
* Callback function type supported.  Caller must create a function with
* these parameters and cast.
*/

typedef void (* QDI_Device_CbFxnType)(QDI_Device_ReturnCallbackInfoType *pReturnCbInfo);

/* -----------------------------------------------------------------------
** 2. Display
** -----------------------------------------------------------------------*/

/*!
* \b QDI_Display_CapsType
*
* Bit fields store flag information indicating the display block capabilities.
*
*/
typedef struct
{
  bool32                        bPostGammaCorrection;       /** Indicates if the display can support gamma correction */
  bool32                        bPostColorSpaceConversion;  /** Indicates if the display supports non-RGB output */
  bool32                        bAdaptativeBacklight;       /**< Indicates if content adaptive backlight is supported */
  bool32                        bDither;                    /**< Indicates if output dithering is supported for low pixel depth panels */
  bool32                        bDeflickerDeinterlace;      /**< Indicates of the display can support deflickering when interlacing */
  bool32                        bScanlinesCount;            /**< Indicates if the display's scanline counter can be queried */
  bool32                        bVblankState;               /**< Indicates if the displays vblank state can be queried */
  bool32                        bRotation;                  /**< Indicates if rotation (in multiples of 90 degrees) can be supported */
  bool32                        bMirroring;                 /**< Indicates if mirroring (horizontal and/or vertical) can be supported */
  bool32                        bVsyncAutoUpdate;           /**< Indicates if hardware can generate a periodic vsync */
  uint32                        uMaxOfLayersSupport;        /**< Indicates the maximum number of layers that can be attached to this display */
  bool32                        bSynchronizedRefresh;       /**< Indicates that this display refresh can be synchronized with other displays */
  bool32                        bLetterboxing;              /**< Indicates if letterboxing is possible on this display, letterboxing means
                                                                 having base layer smaller than the display size */
  uint32                        uBacklightLevels;           /**< Indicates the number of discrete backlight levels supported */
  bool32                        bSmoothBacklight;           /**< Indicates if smooth backlight control is supported */  
  bool32                        bFrameBufferWriteBack;      /**< Indicates if the display can support writeback to framebuffer or not */
  bool32                        bBorderColor;               /**< Indicates if the display can support border color or not */
  QDI_PartialUpdateCapType      ePartialUpdateSupport;      /**< Indicates if the display can support partial update feature */
  uint32                        uMaxDestUpScaleRatio;       /**< Indicate maximum upscale supported on the destination scaler */
  uint32                        uMaxDestDownScaleRatio;     /**< Indicate maximum downscale supported on the destination scaler */
  uint32                        uMaxNumMetadata;            /**< Indicates maximum number of metadata formats simultaneously supported on this display */
  uint32                        uMaxMetadataSize;           /**< Indicates maximum size of metadata supported per format on this display in bytes */
  uint32                        uPCCCoefficients;           /**< Indicates the maximum number of polynomial color correction coefficients supported on this display */
} QDI_Display_CapsType;

/*!
 * \b QDI_BrightnessRangeFlagType
 *
 * Flag to indicate the property of a brightness range
 */

typedef enum
{
  QDI_BRIGHTNESS_RANGE_FLAG_BOOST       = 0x00000001,       /**< Flag to specify boost brightness range                                   */
  QDI_BRIGHTNESS_RANGE_FLAG_CALIBRATED  = 0x00000002,       /**< Brightness range is calibrated, panel brightness calibrated to each step */
  QDI_BRIGHTNESS_RANGE_FLAG_FORCE_32BIT = 0x7FFFFFFF
} QDI_BrightnessRangeFlagType; 

/*!
* Defines maximum number of brightness ranges (in QDI_Display_BrightnessCapsType) supported by QDI driver
*/
#define QDI_MAXIMUM_BRIGHTNESS_RANGE_LEVEL       10

/*!
* \b QDI_Display_BrightnessRangeParams
*
*  Defines brightness range level parameters
*  
*  This structure contains minimum and maximum brightness levels and a step size
*/
typedef struct 
{
  uint32                              uMinimumLevel;                           /**< Minimum brightness level for this range defined in millinits (1/1000 of a nit). */
  uint32                              uMaximumLevel;                           /**< Maximum brightness level for this range defined in millinits (1/1000 of a nit). */
  uint32                              uStepSize;                               /**< The step size for brightness levels in this range. */
  QDI_BrightnessRangeFlagType         uFlags;                                  /**< Flag to indicate the property of the brightness range. */
}QDI_Display_BrightnessRangeParams;

/*!
* \b QDI_Display_BrightnessCapsType
*
*  Defines brightness capability supported by the driver exposed through display caps QDI_Display_InfoType
*  
*  This structure contains the number of ranges defined by the driver and parameters for each range
*  If the number of ranges are zero then legacy backlight API must be used to retrieve the display brightness capability and to set/get
*  brightness property.
*/
typedef struct {
  uint32                              uNumRanges;                                        /**< Number of valid brightness range levels in sRangeParams */
  QDI_Display_BrightnessRangeParams   sRangeParams[QDI_MAXIMUM_BRIGHTNESS_RANGE_LEVEL];  /**< Configuration of each brightness range level */
}QDI_Display_BrightnessCapsType;

/*!
 * \b QDI_PanelMountType
 *
 * Indicates the physical mounting of the panel, can be used as a clue for the application
 * on the type of content depending on the mounting.
 */

typedef enum
{
  QDI_PANEL_MOUNT_UNKNOWN, /**< Unknown orientation */
  QDI_PANEL_MOUNT_TOP,     /**< Panel is mounted on above other panels */
  QDI_PANEL_MOUNT_LEFT,    /**< Panel is mounted on below other panels */
  QDI_PANEL_MOUNT_RIGHT,   /**< Panel is mounted on the right other panels */
  QDI_PANEL_MOUNT_BOTTOM,  /**< Panel is mounted on the left other panels */
  QDI_PANEL_MOUNT_CENTER,  /**< Panel is mounted on the center relative to other panels */
  QDI_PANEL_MOUNT_MAX,     /**< Maximum number mount positions. */
  QDI_PANEL_MOUNT_FORCE_32BIT = 0x7FFFFFFF
} QDI_PanelMountType; 

/*!
* \b QDI_Display_ChromaticityPointType
*
*  Defines the display chromaticity values in the X, Y and Z dimension. 
*  
*  Each dimension is represent by a binary fraction in 10 bit length (bit9 × 2^-1 + ... + bit0 × 2^-10), 
*  as defined in the EDID specification
*/
typedef struct 
{
  uint32                              uX;     /* CIE X color space */
  uint32                              uY;     /* CIE Y color space */
  uint32                              uZ;     /* CIE Z color space, optional, if CIE 1931 XY color spaces is used */
}QDI_Display_ChromaticityPointType;

/*!
* \b QDI_Display_ChromaticityInfoType
*
*  Defines the display chromaticity red, green, blue and white point data in CIE XYZ color space. 
*/
typedef struct 
{
  QDI_Display_ChromaticityPointType sRed;      /**< Red chromaticity data, each dimension, in color space xyz, is
                                                    represent by a binary fraction in 10 bit length,
                                                    stored in the least significant bits */
  QDI_Display_ChromaticityPointType sGreen;    /**< Green chromaticity data, each dimension, in color space xyz, is
                                                    represent by a binary fraction in 10 bit length,
                                                    stored in the least significant bits */
  QDI_Display_ChromaticityPointType sBlue;     /**< Blue chromaticity data, each dimension, in color space xyz, is
                                                    represent by a binary fraction in 10 bit length,
                                                    stored in the least significant bits */
  QDI_Display_ChromaticityPointType sWhite;    /**< Whitepoint chromaticity data, each dimension, in color space xyz, is
                                                    represent by a binary fraction in 10 bit length,
                                                    stored in the least significant bits */
}QDI_Display_ChromaticityInfoType;

/*!
* \b QDI_Display_InfoType
*
* Structure containing information pertaining to a particular
* display.   This is a stripped down version of DS Layer SurfaceAttrType
* and contains only the items we want to expose to the user.
*
* The values in iVisWidthPx and iVisHeightPx are the dimensions to use for 
* memory surface allocation.  The sActiveRoi specifies the visible portion
* of the memory surface.
*/
typedef struct
{
  QDI_PixelFormatType               eColorFormat;                                       /**< Color format of the display */
  QDI_DisplayConnectType            ePhysConnect;                                       /**< Physical connection of display.*/
  uint32                            uVisWidthPx;                                        /**< Physical display width in pixels */
  uint32                            uVisHeightPx;                                       /**< Physical display height in pixels */
  uint32                            uRefreshRate;                                       /**< Refresh Rate of Display, in 16.16 fixed format*/
  QDI_RectType                      sActiveRoi;                                         /**< Visible portion of Display Surface. */
  QDI_Display_CapsType              sDisplayFeatures;                                   /**< features available to this display */
  bool32                            bInUse;                                             /**< returns if the display is in use or not*/
  QDI_RotateFlipType                ePanelOrientation;                                  /**< Indicates how is the mounting of the panel */
  uint32                            uNumberOfModes;                                     /**< Number of operations */
  uint32                            uNumberOfAudioModes;                                /**< Number of audio modes for HDMI displays */
  QDI_PanelMountType                ePanelMounting;                                     /**< Indicates the physical mounting of the panel  */
  uint32                            uPhysicalPanelWidth;                                /**< Physical panel width in millimeters, in 16.16 fixed format */
  uint32                            uPhysicalPanelHeight;                               /**< Physical panel height in millimeters,in 16.16 fixed format */
  uint32                            uNumOfOutputPixelFormatSupported;                   /**< Number of supported output pixel format of display HW*/
  QDI_PixelFormatType               eOutputPixelFormatSupported[QDI_PIXEL_FORMAT_MAX];  /**< Supported output pixel format of display HW */
  QDI_WireFormatType                eWireFormat;                                        /**< List of supported wire formats */
  uint32                            uMaxLuminance;                                      /**< Maximum luminance of the display in millinits (1/1000 of a nit) */
  uint32                            uMinLuminance;                                      /**< Minimum luminance of the display in millinits (1/1000 of a nit) */  
  uint32                            uMaxFullFrameAvgLuminance;                          /**< Maximum full frame average luminance of the display in millinits (1/1000 of a nit) */  
  QDI_Display_BrightnessCapsType    sBrightnessCaps;                                    /**< Advanced brightness configuration of the display */
  uint32                            uPhysicalPortID;                                    /**< Physical port index for external display connect */
  uint32                            uStreamID;                                          /**< Stream index for the physical port */
  QDI_Display_ChromaticityInfoType  sChromaticityInfo;                                  /**< Chromaticity coordinates of the display */
  uint32                            uNumberOfInterfaces;                                /**< Number of interfaces associated with the display */
  QDI_Color_EncodingType            eColorBlendSpace;                                   /**< Supported color space by the display other than sRGB (sRGB is implicitly supported by the display) */
} QDI_Display_InfoType;

/*!
 * \b QDI_Display_SynchronizedRefreshType
 *
 *  Structure for the synchronized refresh to enable or disable synchronization
 */
typedef struct
{
  bool32         bEnableSynchronization;            /**< Configure this display to run off a synchronized timer */
  QDI_FlagsType  uFlags;                            /**< Reserved flags */
} QDI_Display_SynchronizedRefreshType;

/*!
* \b QDI_Display_RawEDIDInfoPropType 
*
*  Structure to obtain the raw display EDID (Extended display identification data) information.
*/
typedef struct
{
  uint8                                  *pData;                        /* Buffer pointer to the display EDID info */
  uint32                                  uDataSize;                    /* Size of buffer as input and size of data copied as output */
  uint32                                  uFlags;                       /* Reserved */
} QDI_Display_RawEDIDInfoPropType ;

/*!
* \b QDI_Display_RawEDIDInfoSizePropType
*
*  Structure to obtain the size of the raw display EDID (Extended display identification data) information
*/
typedef struct
{
  uint32                                  uDataSize;                   /* Size of the display EDID info */
  uint32                                  uFlags;                      /* Reserved */
} QDI_Display_RawEDIDInfoSizePropType;

/*!
* \b QDI_Display_OpenParamsType
*
* Parameters to initialize the display - for future expansion
*/
typedef struct 
{
  QDI_Display_IDType   eDisplayId; /**< Id of the display to be initialized */
}QDI_Display_OpenParamsType;

/*!
* \b QDI_Display_ModeAspectRatioType
*
*  Indicates the various aspect ratio that the sink can support
*/
typedef enum
{
  QDI_DISPLAY_MODE_ASPECT_RATIO_NONE     = 0x0,
  QDI_DISPLAY_MODE_ASPECT_RATIO_4_3,                   /* Aspect ratio of 4:3 */
  QDI_DISPLAY_MODE_ASPECT_RATIO_16_9,                  /* Aspect ratio of 16:9 */
  QDI_DISPLAY_MODE_ASPECT_RATIO_16_10,                 /* Aspect ratio of 16:10 */
  QDI_DISPLAY_MODE_ASPECT_RATIO_5_4,                   /* Aspect ratio of 5:4 */
  QDI_DISPLAY_MODE_ASPECT_RATIO_15_9,                  /* Aspect ratio of 15:9 */
  QDI_DISPLAY_MODE_ASPECT_RATIO_5_3,                   /* Aspect ratio of 5:3 */ 
  QDI_DISPLAY_MODE_ASPECT_RATIO_MAX,
  QDI_DISPLAY_MODE_ASPECT_RATIO_32BIT    = 0x7FFFFFFF
}QDI_Display_ModeAspectRatioType;



/*!
* \b QDI_Display_ModeInfoType
*
* Structure containing information pertaining to a particular mode of operation
*  of the display. A same display can have different mode of operation  
*/
typedef struct
{
  uint32                           uModeIndex;       /**< mode Index start from 0 */
  uint32                           uWidthPx;         /**< visible display width (pixels).*/
  uint32                           uWidthTotalPx;    /**< total display width including blanking (pixels) */
  uint32                           uHeightPx;        /**< visible display height (pixels).*/
  uint32                           uHeightTotalPx;   /**< total display height including blanking (pixels) */
  uint32                           uRefreshRate;     /**< 16.16 fixed format. Refresh Rate of Display */
  uint32                           uPixelRate;       /**< Pixel frequency in Hz */
  QDI_PixelFormatType              eColorFormat;     /**< Color format of the display */
  QDI_RotateFlipType               eRotation;        /**< Indicates the orientation of the screen */
  bool32                           bInterlaced;      /**< Indicates interlace output or progressive output */
  QDI_3D_FrameFormatType           e3DFrameFormat;   /**< Indicates the type of 3D format this mode is associated with */
  QDI_Display_ModeAspectRatioType  eAspectRatio;     /**< Indicates the aspect ratio of the current mode */
  uint32                           uPixelClk;        /**< Indicates the pixel clock for this mode. */
  bool32                           bAudioSupported;  /**< Indicates whether current HDMI mode supports Audio*/
} QDI_Display_ModeInfoType;

/*!
* \b QDI_ZorderType
*
*  Defines the valid Z order position for attached layers.  
*  ZORDER_4 and ZORDER_5 are reserved for future use.  
*  ZORDER_TOP and ZORDER_BOTTOM are used for positioning layers.
*
*/
typedef enum
{
  QDI_ZORDER_BOTTOM = 0,
  QDI_ZORDER_0 = 0, 
  QDI_ZORDER_1,
  QDI_ZORDER_2,
  QDI_ZORDER_3,
  QDI_ZORDER_4,
  QDI_ZORDER_5,
  QDI_ZORDER_6,
  QDI_ZORDER_TOP,
  QDI_ZORDER_MAX,
  QDI_ZORDER_FORCE_32BIT = 0x7FFFFFFF
} QDI_ZorderType;

/*!
* \b QDI_Display_PropertyType
*
*  Defines list of display properties that can be modified 
*/
typedef enum
{
  QDI_DISPLAY_PROPERTY_VISIBLESTATE = 0,             /**< Enable or display any transfer to the physical panel */
  QDI_DISPLAY_PROPERTY_POWER,                        /**< Power on or off the physical panel */
  QDI_DISPLAY_PROPERTY_GAMMA_CORRECTION,             /**< Set or get gamma correction table, enable or disable gamma correction.*/
  QDI_DISPLAY_PROPERTY_AMBIENT_ADAPTIVE_BACKLIGHT,   /**< Set or get content ambient backlight property.*/
  QDI_DISPLAY_PROPERTY_BACKLIGHT,                    /**< Get or set backlight level*/
  QDI_DISPLAY_PROPERTY_DITHER,                       /**< Enable or disable display dither. */
  QDI_DISPLAY_PROPERTY_DEFLICKER,                    /**< Enable or disable deflickering. */
  QDI_DISPLAY_PROPERTY_ROTATION_FLIP,                /**< Set mirror and rotation. */
  QDI_DISPLAY_PROPERTY_MODE_INDEX,                   /**< Set or get current mode index. */
  QDI_DISPLAY_PROPERTY_MODE_INFO,                    /**< Use it to get mode information.*/
  QDI_DISPLAY_PROPERTY_VSYNC,                        /**< Wait for VBLANK or get VSYNC status.*/
  QDI_DISPLAY_PROPERTY_SCREEN_CAPTURE,               /**< Return frame buffer data being shown on the screen. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_CRC,                          /**< Return CRC of one frame. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_COMMIT_DONE,                  /**< Return status if commit is finished. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_CONTENT_ADAPTIVE_BACKLIGHT,   /**< Set or get content adaptive backlight property.*/
  QDI_DISPLAY_PROPERTY_PRIMARY_SURFACE,              /**< Return primary surface's device,display,layer handle and primary surface attribute. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_HDMI_AUDIO,                   /**< Set HDMI audio configurations.*/
  QDI_DISPLAY_PROPERTY_HDMI_AUDIO_MODE_INFO,         /**< Return HDMI audio capabilities. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_HDMI_VENDOR_INFO,             /**< Return HDMI Vendor data block info. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_HDMI_AVI_PACKET,              /**< Set HDMI Auxiliary Video Info (AVI) packet info.*/
  QDI_DISPLAY_PROPERTY_HDMI_ISRC_PACKET,             /**< Set HDMI International standard recording code (ISRC) packet info.*/
  QDI_DISPLAY_PROPERTY_HDMI_ACP_PACKET,              /**< Set HDMI Audio Content Protection (ACP) packet info.*/
  QDI_DISPLAY_PROPERTY_HDMI_CEC_MSG,                 /**< Send HDMI CEC message to HDMI sink device */
  QDI_DISPLAY_PROPERTY_3D_FRAME_FORMAT,              /**< Set 3D frame format */
  QDI_DISPLAY_PROPERTY_HSIC,                         /**< Set/Get panel hue, saturation, intensity and contrast control.*/
  QDI_DISPLAY_PROPERTY_GETSCANLINE,                  /**< Return the current active display scanline. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_TEARCHECK,                    /**< Enable or disable tear check. */
  QDI_DISPLAY_PROPERTY_SYNCHRONIZED_REFRESH,         /**< Enable/disable display refresh synchronization */
  QDI_DISPLAY_PROPERTY_CONTENT_PROTECTION,           /**< Enable/disable display content protection */
  QDI_DISPLAY_PROPERTY_RAW_EDID_INFO,                /**< Return Display EDID information. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_RAW_EDID_INFO_LENGTH,         /**< Return Display EDID information length. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_PANEL_CONNECTION_STATUS,      /**< Return external panel connection status. Supported only for QDI_Display_GetProperty */ 
  QDI_DISPLAY_PROPERTY_HDCP_INFO,                    /**< Return HDCP information. Supported only for QDI_Display_GetProperty */ 
  QDI_DISPLAY_PROPERTY_HDCP_STATUS,                  /**< Return HDCP status. Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_CABL_LUT_LENGTH,              /**< Return the Gamma LUT length (number of entries). Supported only for QDI_Display_GetProperty */
  QDI_DISPLAY_PROPERTY_SMOOTH_BACKLIGHT,             /**< Set/Get Smooth Backlight enable status */
  QDI_DISPLAY_PROPERTY_FRAMEBUFFER_WRITEBACK_CONFIG, /**< Set or Return writeback configuration. */
  QDI_DISPLAY_PROPERTY_FRAMEBUFFER_WRITEBACK_INFO,   /**< Set or Return framebuffer writeback Info. */
  QDI_DISPLAY_PROPERTY_HDCP_DOWNSTREAM_KSV_LIST,     /**< Return the KSV list of all downstream devices connected to the HDCP repeater */
  QDI_DISPLAY_PROPERTY_DYNAMIC_REFRESH_RATE,         /**< Set or Return dynamic refresh rates supported by the display */
  QDI_DISPLAY_PROPERTY_PICTURE_MODE,                 /**< Set display picture mode.Supported only for QDI_Display_SetProperty*/
  QDI_DISPLAY_PROPERTY_BORDER_COLOR,                 /**< Set/Get display border color */
  QDI_DISPLAY_PROPERTY_DESTINATION_SCALE,            /**< Scale destination surface to requested dimension */
  QDI_DISPLAY_PROPERTY_HDCP_LEVEL,                   /**< Set or return the HDCP protection level */   
  QDI_DISPLAY_PROPERTY_BRIGHTNESS,                   /**< Set/Get the brightness configuration using QDI_Display_BrightnessPropType 
                                                          Usage of the QDI_DISPLAY_PROPERTY_BRIGHTNESS and QDI_DISPLAY_PROPERTY_BACKLIGHT are mutually exclusive.*/   
  QDI_DISPLAY_PROPERTY_PROGRAMMABLE_LINE_INTERRUPT,  /**< Configure programmable line interrupts */
  QDI_DISPLAY_PROPERTY_ROUTING_INFO,                 /**< Return routing info i.e. controller id, stream id */
  QDI_DISPLAY_PROPERTY_METADATA,                     /**< Configure metadata sent on a display. */
  QDI_DISPLAY_PROPERTY_BLEND_SPACE,                  /**< Set/Get display blend space. */
  QDI_DISPLAY_PROPERTY_PCC,                          /**< Set polynomial color correction matrix. */
  QDI_DISPLAY_PROPERTY_MAX,
  QDI_DISPLAY_PROPERTY_FORCE_32BIT = 0x7FFFFFFF
} QDI_Display_PropertyType;

/*!
* \b QDI_Display_VisibleStateType
*
*  Defines list of possible visible state of a display 
*/
typedef enum
{
  QDI_DISPLAY_STATE_OFF = 0,
  QDI_DISPLAY_STATE_ON ,
  QDI_DISPLAY_STATE_MAX ,
  QDI_DISPLAY_STATE_FORCE32BIT = 0x7FFFFFFF
}QDI_Display_VisibleStateType;

/*!
* \b QDI_Display_GammaControlPropType
*
*  Defines if the Gamma Control is enabled.
*  It will also provide the tables that are used for the gamma control 
*  Tables could be read or set ( if GetProperty or SetProperty is called )
*/
typedef struct 
{ 
  bool32 bEnabled;    /**< Enable gamma mapping */
  uint32 uEntrySize;  /**< Size of each of the Gamma table entries in bytes */
  uint8  uInputWidth; /**< Width in bits of each input of the entries in Gamma table. Inputs are padded to next byte */
  uint8* pRedCr;      /**< Pointer to the gamma table red entry. All inputs are byte aligned. */
  uint8* pGreenY;     /**< Pointer to the gamma table green entry. All inputs are byte aligned. */ 
  uint8* pBlueCb;     /**< Pointer to the gamma table blue entry. All inputs are byte aligned. */ 
} QDI_Display_GammaControlPropType;

/*!
* \b QDI_Display_PCCPropType
*
*  Controls the PCC block configuration.
*  It will also provide the polynomial color correction(PCC) matrix
*  which will be used for panel calibration.
*/
typedef struct
{
  bool32 bEnabled;          /**< Enable PCC */
  float  fCCMatrix[3][11];  /**< Color correction matrix. */
  /**<
  Color correction matrix -- Passes the color correction coefficients
         in the panel color correction buffer in order.

         @note1 Any changes to the panel color correction order yields unpredictable results.

         Color coefficient indexes are defined as follows:
         - Index 0 -- Red color coefficients
         - Index 1 -- Green color coefficients
         - Index 2 -- Blue color coefficients
            - R' = a00 + a01 R + a02 G + a03 B + a04 R*G +
                                   a05 G*B + a06 B*R + a07 R*R + a08 G*G + a09 B*B + a010 R*G*B
            - G' = a10 + a11 R + a12 G + a13 B + a14 R*G +
                                   a15 G*B + a16 B*R + a17 R*R + a18 G*G + a19 B*B + a110 R*G*B
            - B' = a20 + a21 R + a22 G + a23 B + a24 R*G +
                                   a25 G*B + a26 B*R + a27 R*R + a28 G*G + a29 B*B + a210 R*G*B

              where a          = pcc matrix, 
                    R, G, B    = input pixels
                    R', G', B' = output pixels after applying PCC.
  */

} QDI_Display_PCCPropType;

/*!
* \b QDI_Display_VSyncPropType
*
*  Defines if driver is in VBlank or driver will wait until VBlank.
*/
typedef struct
{
  bool32                           bWaitForVsync;    /**< Indicate if QDI need wait for VSYNC. */
  bool32                           bIsInVBlank;      /**< Indicate if display is in VBlank. */
} QDI_Display_VSyncPropType;

/*!
* \b QDI_Display_CrcPropType
*
*  Defines if the CRC is enabled 
*  crcValue is a read-only if crc is enabled 
*/
typedef struct
{
  bool32                bEnabled;             /**< Indicate if CRC collection is enabled */
  uint32                uCrcValue;            /**< CRC value */
  bool32                bDumpedFrameEnabled;  /**< Indicate if dumping frame is enabled*/
  QDI_SurfaceAttrType   sDumpedSurface;       /**< The surface attribute for dumped frame */
} QDI_Display_CrcPropType;

/*!
* \b QDI_Display_ScreenCapturePropType
*
*  Retrieve frame buffer data being shown on the display screen 
*
*/

typedef struct
{
  QDI_SurfaceAttrType   sCaptureSurface;       /**< The surface for saving frame buffer data */
  QDI_RectType          sRegionOfInterest;     /**< Region of Interest */
  QDI_FlagsType         flags;                 /**< Reserved */
} QDI_Display_ScreenCapturePropType;

/*!
* \b QDI_Display_PrimarySurfacePropType
*
*  Defines if the primary surface is attached to the display 
*/
typedef struct
{
  QDI_HandleType        hDevice;
  QDI_HandleType        hDisplay;
  QDI_HandleType        hLayer;
  QDI_SurfaceAttrType   primarySurface;
} QDI_Display_PrimarySurfacePropType;

/*!
* \b QDI_PowerSavingModeType
*
*  Defines display power saving level 
*/
typedef enum
{
  QDI_POWERSAVE_HIGH = 0, /**< Most power saving*/
  QDI_POWERSAVE_MEDIUM,  /**< Normal power saving*/
  QDI_POWERSAVE_LOW,     /**< Low power saving*/
  QDI_POWERSAVE_MAX,
  QDI_POWERSAVE_FORCE_32BIT = 0x7FFFFFFF
} QDI_PowerSavingModeType;

/*!
* \b QDI_ContentType
*
*  Defines display content type
*/
typedef enum
{
  QDI_CONTENT_GRAPHIC = 0, /**< Graphic content, ie, menu UI */
  QDI_CONTENT_STILL_IMAGE, /**< Still image content, ie, JPEG */
  QDI_CONTENT_VIDEO,       /**< Video content, ie, MP4, H264 video */
  QDI_CONTENT_MAX,
  QDI_CONTENT_FORCE_32BIT = 0x7FFFFFFF
} QDI_ContentType;

/*!
* \b QDI_ABLProviderType
*
*  Defines ABL function provider 
*/
typedef enum 
{
  QDI_ABL_PROVIDER_QUALCOMM = 0, /**< Qualcomm MSM performs ABL algorithm */
  QDI_ABL_PROVIDER_PANEL_VENDOR, /**< Panel vendor performs ABL algorithm */
  QDI_ABL_PROVIDER_MAX,
  QDI_ABL_PROVIDER_FORCE_32BIT = 0x7FFFFFFF
}QDI_ABLProviderType;

/*!
 * \b QDI_Display_ContentABLInfoType
 *
 *  Defines QDI content based ABL info. This structure is used to configure CABL properties
 */
typedef struct 
{
  bool32                   bEnable;               /**< FALSE - disable, TRUE - enable content ABL */
  QDI_ABLProviderType      eProvider;             /**< Qualcomm or Panel Vendor (Currently not used)*/
  QDI_PowerSavingModeType  ePowerSavingMode;      /**< Power-save high, medium, low correspond to Image-Quality low, normal, high*/
  QDI_ContentType          eContent;              /**< graphic, Still image or Video (Currently not used)*/
} QDI_Display_ContentABLInfoType;

/*!
 * \b QDI_Display_ContentABLStatusType
 *
 *  Defines QDI content based ABL info. This structure is used retrieve CABL info 
 *  on QDI_Display_GetProperty()
 *  If pGammaLutBuffer is non-null, then the CABL gamma LUT is returned
 *  
 *  To retrieve pGammaLutBuffer, caller must first call QDI_Display_GetProperty()
 *  with QDI_DISPLAY_PROPERTY_CABL_LUT_SIZE_INFO as eProperty and determine 
 *  the memory to be allocated for pGammaLutBuffer
 *
 *  Note iActualBacklight is the backlight level calculated by CABL but it's not
 *  the level that will be programmed to the hardware. Other processes like mapping using
 *  a backlight response table could take place to calculate the final backlight level.
 */
typedef struct 
{
  QDI_Display_ContentABLInfoType    sCABLConfiguration;    /**< Returns the current configuration of CABL */ 
  uint32                            uOriginalBacklight;    /**< The backlight level that was originally programmed through QDI_DISPLAY_PROPERTY_BACKLIGHT*/
  int32                             iActualBacklight;      /**< Backlight level computed by CABL algorithm (Q16.16 format) */
  uint16                           *pGammaLutBuffer;       /**< Array containing the LUT computed by CABL algorithm, the same is applied to R, G, and B channels*/
  uint32                            uBufferLength;         /**< Length of the pGammaLutBuffer array in units. Set by the caller. */
  uint32                            uFlags;                /**< Reserved*/
}QDI_Display_ContentABLStatusType;

/*!
 * \b QDI_ Display_AmbientABLInfoType
 *
 *  Defines QDI ambient based ABL info 
 */
typedef struct
{
  bool32                    bEnable;            /**< 0 - disable, 1 - enable content ABL */
  QDI_ABLProviderType       eProvider;          /**< QDI_ABLProviderType */
  QDI_PowerSavingModeType   ePowerSavingMode;   /**< QDI_PowerSavingModeType */
  uint32                    uMaxIntensity;      /**< maximum light intensity with unit in mlux */ 
  uint32                    uMinIntensity;      /**< minimum light intensity with unit in mlux */
  uint32                    uMeasuredIntensity; /**< light intensity measured by the sensor with unit in mlux */
} QDI_Display_AmbientABLInfoType;

/*!
 * \b QDI_Display_SmoothBacklightPropType
 *
 *  Defines QDI Smooth Backlight Control (SBC) info. For a set property only bEnable is used to enable or 
 *  disable SBC. For a get property uSBCBacklightLevel indicates the last calculated backlight level. 
 *  Note this backlight level is not the level that will make it to the hardware. After SBC other processes
 *  like CABL and more could take place to calculate the final backlight level.
 *  
 */
typedef struct
{
  bool32                  bEnable;               /**< FALSE - disable, TRUE - enable Smooth Backlight Control */
  int32                   iSBCBacklightLevel;    /**< Current Backlight Level as calculated by SBC (Q16.16 format) */
  uint32                  uRampTime;             /**< Reserved */
} QDI_Display_SmoothBacklightPropType;



/*!
* \b QDI_Display_RefreshRateType
*
* Defines dynamic refresh rates supported for current mode. For GetProperty, this property provides the number of dynamic refresh rates
* supported and the list of these refresh rates in Q16.16 format. For SetProperty the user specifies and the index of the refresh rate 
* in the refresh rate list supplied in the GetProperty. 
*
* When dynamic refresh rate is disabled QDI will switch back to default refresh rate for the current mode.
*/
typedef struct
{
    bool32                  bEnableDynamicRefreshRate;                      /**< Used in only SetProperty to enable/disable use of dynamic refresh rate */
    uint32                  uRefreshRateIndex;                              /**< Used in only SetProperty to specify which dynamic refresh rate to use */
    uint32                  uNumberOfRefreshRates;                          /**< Used in only GetProperty to specify how many dynamic refresh rates 
                                                                                 supported for this mode */ 
    uint32                 *pRefreshRates;                                  /**< Used in only GetProperty to specify the list of refresh rates (in Hz) supported (Q16.16 format) 
                                                                                 Can be NULL, in which case GetProperty will only return the number of refresh rates supported. */ 
} QDI_Display_RefreshRateType;

/*!
* \b QDI_Display_DestScalerConfigType
*
*  Defines HW destination scaling parameters.
*/
typedef struct
{
  bool32                   bScale;                /**< FALSE/TRUE - disable/enable scaling of destination scaler */
  QDI_RectType             sSrcRect;              /**< Dimensions of input to destination scaler */
  QDI_RectType             sDstRect;              /**< Dimensions of scaled output (output of destination scaler) */
} QDI_Display_DestScalerConfigType;


/*!
* \b QDI_HDMI_AudioSampleRateType
*
* The various audio sample rate for HDMI Display
*/
typedef enum
{
  QDI_HDMI_AUDIO_SAMPLE_RATE_NONE              = 0x0,  /* No Audio Sample rate */
  QDI_HDMI_AUDIO_SAMPLE_RATE_32KHZ             = 0x1,  /* Audio Sample rate 32KHZ */
  QDI_HDMI_AUDIO_SAMPLE_RATE_44_1KHZ           = 0x2,  /* Audio Sample rate 44.1KHZ */
  QDI_HDMI_AUDIO_SAMPLE_RATE_48KHZ             = 0x4,  /* Audio Sample rate 48KHZ */
  QDI_HDMI_AUDIO_SAMPLE_RATE_88_2KHZ           = 0x8,  /* Audio Sample rate 88.2KHZ */
  QDI_HDMI_AUDIO_SAMPLE_RATE_96KHZ             = 0x10, /* Audio Sample rate 96KHZ */
  QDI_HDMI_AUDIO_SAMPLE_RATE_176_4KHZ          = 0x20, /* Audio Sample rate 176.4KHZ */
  QDI_HDMI_AUDIO_SAMPLE_RATE_192KHZ            = 0x40, /* Audio Sample rate 192KHZ */
  QDI_HDMI_AUDIO_SAMPLE_RATE_MAX,                
  QDI_HDMI_AUDIO_SAMPLE_RATE_FORCE_32BIT = 0x7FFFFFFF
}QDI_HDMI_AudioSampleRateType;

/*!
* \b QDI_HDMI_AudioSampleBitDepthType
*
* The various audio sample bit depth for HDMI Display
*/
typedef enum
{
  QDI_HDMI_AUDIO_BIT_DEPTH_NONE   = 0x0,              /* No Audio Bit Depth */
  QDI_HDMI_AUDIO_BIT_DEPTH_16_BIT = 0x1,              /* Audio Bit Depth 16bit per sample*/
  QDI_HDMI_AUDIO_BIT_DEPTH_20_BIT = 0x2,              /* Audio Bit Depth 20bit per sample*/
  QDI_HDMI_AUDIO_BIT_DEPTH_24_BIT = 0x4,              /* Audio Bit Depth 24bit per sample*/
  QDI_HDMI_AUDIO_BIT_DEPTH_MAX,
  QDI_HDMI_AUDIO_BIT_DEPTH_FORCE_32BIT = 0x7FFFFFFF
}QDI_HDMI_AudioSampleBitDepthType;

/*!
* \b QDI_HDMI_AudioChannelNumType
*
* The various audio channel number for HDMI Display
*/
typedef enum
{
  QDI_HDMI_AUDIO_CHANNEL_NONE = 0x0,                   /* Audio channel 2 */
  QDI_HDMI_AUDIO_CHANNEL_2    = 0x1,                   /* Audio channel 2 */
  QDI_HDMI_AUDIO_CHANNEL_4    = 0x2,                   /* Audio channel 4 */ 
  QDI_HDMI_AUDIO_CHANNEL_6    = 0x4,                   /* Audio channel 6 */ 
  QDI_HDMI_AUDIO_CHANNEL_8    = 0x8,                   /* Audio channel 8 */ 
  QDI_HDMI_AUDIO_CHANNEL_MAX,
  QDI_HDMI_AUDIO_CHANNEL_FORCE_32BIT = 0x7FFFFFFF
}QDI_HDMI_AudioChannelNumType;

/*!
* \b QDI_HDMI_AudioFormatType
*
*  Defines the audio formats supported by the HDMI display
*/
typedef enum
{
  QDI_HDMI_AUDIO_FORMAT_RESERVED = 0,                  /* Reserved for future */
  QDI_HDMI_AUDIO_FORMAT_LPCM,                          /* Audio codec type Linear PCM */
  QDI_HDMI_AUDIO_FORMAT_AC3,                           /* Audio codec type AC3 */
  QDI_HDMI_AUDIO_FORMAT_MPEG1_LAYER1_AND_2,            /* Audio codec type MPEG1 Layer1 and 2 */
  QDI_HDMI_AUDIO_FORMAT_MP3,                           /* Audio codec type MP3*/
  QDI_HDMI_AUDIO_FORMAT_MPEG2,                         /* Audio codec type MPEG2 */
  QDI_HDMI_AUDIO_FORMAT_AAC,                           /* Audio codec type AAC */
  QDI_HDMI_AUDIO_FORMAT_DTS,                           /* Audio codec type DTS */
  QDI_HDMI_AUDIO_FORMAT_ATRAC,                         /* Audio codec type ATRAC */
  QDI_HDMI_AUDIO_FORMAT_ONE_BIT_AUDIO,                 /* Audio codec type 1 bit audio */
  QDI_HDMI_AUDIO_FORMAT_DOLBY_DIGITAL_PLUS,            /* Audio codec type Dolby Digital+ */
  QDI_HDMI_AUDIO_FORMAT_DTS_HD,                        /* Audio codec type DTS HD*/
  QDI_HDMI_AUDIO_FORMAT_MAT,                           /* Audio codec type MAT */
  QDI_HDMI_AUDIO_FORMAT_DST,                           /* Audio codec type DST */
  QDI_HDMI_AUDIO_FORMAT_WMA_PRO,                       /* Audio codec type WMA PRO */
  QDI_HDMI_AUDIO_FORMAT_RESERVED1,                     /* Reserved for future */
  QDI_HDMI_AUDIO_FORMAT_MAX,
  QDI_HDMI_AUDIO_FORMAT_FORCE_32BIT = 0x7FFFFFFF
} QDI_HDMI_AudioFormatType;

/*!
* \b QDI_HDMI_DeepColorModeType
*
*  Defines the deep color bit depth used by the HDMI display
*/
typedef enum
{
  QDI_HDMI_DEEP_COLOR_FORMAT_NONE   = 0x0, /* Not Deep color format */
  QDI_HDMI_DEEP_COLOR_FORMAT_Y_444  = 0x1, /* Deep color bit depth Y444*/
  QDI_HDMI_DEEP_COLOR_FORMAT_30_BIT = 0x2, /* Deep color bit depth 30 bit */
  QDI_HDMI_DEEP_COLOR_FORMAT_36_BIT = 0x4, /* Deep color bit depth 36 bit */
  QDI_HDMI_DEEP_COLOR_FORMAT_48_BIT = 0x8, /* Deep color bit depth 48 bit */
  QDI_HDMI_DEEP_COLOR_FORMAT_MAX,
  QDI_HDMI_DEEP_COLOR_FORMAT_FORCE_32BIT = 0x7FFFFFFF
}QDI_HDMI_DeepColorModeType;

/*!
* \b QDI_HDMI_AudioChannelAllocType
*
*  Defines the mapping between the audio channel and sink device speakers
*
*  FL  - Front Left
*  FC  - Front Center
*  FR  - Front Right
*  FLC - Front Left Center
*  FRC - Front Right Center
*  RL  - Rear Left
*  RC  - Rear Center
*  RR  - Rear Right
*  RLC - Rear Left Center
*  RRC - Rear Right Center
*  LFE - Low Frequency Effect
*/
typedef enum
{
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_0  = 0x0, /* Allocate to FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_1,        /* Allocate to LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_2,        /* Allocate to FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_3,        /* Allocate to FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_4,        /* Allocate to RC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_5,        /* Allocate to RC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_6,        /* Allocate to RC, FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_7,        /* Allocate to RC, FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_8,        /* Allocate to RR, RL, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_9,        /* Allocate to RR, RL, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_10,       /* Allocate to RR, RL, FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_11,       /* Allocate to RR, RL, FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_12,       /* Allocate to RC, RR, RL, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_13,       /* Allocate to RC, RR, RL, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_14,       /* Allocate to RC, RR, RL, FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_15,       /* Allocate to RC, RR, RL, FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_16,       /* Allocate to RRC, RLC, RR, RL, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_17,       /* Allocate to RRC, RLC, RR, RL, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_18,       /* Allocate to RRC, RLC, RR, RL, FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_19,       /* Allocate to RRC, RLC, RR, RL, FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_20,       /* Allocate to FRC, FLC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_21,       /* Allocate to FRC, FLC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_22,       /* Allocate to FRC, FLC, FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_23,       /* Allocate to FRC, FLC, FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_24,       /* Allocate to FRC, FLC, RC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_25,       /* Allocate to FRC, FLC, RC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_26,       /* Allocate to FRC, FLC, RC, FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_27,       /* Allocate to FRC, FLC, RC, FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_28,       /* Allocate to FRC, FLC, RR, RL, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_29,       /* Allocate to FRC, FLC, RR, RL, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_30,       /* Allocate to FRC, FLC, RR, RL, FC, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_31,       /* Allocate to FRC, FLC, RR, RL, FC, LFE, FR, FL speakers */
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_MAX,      
  QDI_HDMI_AUDIO_CHANNEL_ALLOC_CODE_FORCE_32BIT = 0x7FFFFFFF
}QDI_HDMI_AudioChannelAllocType;

/*!
* \b QDI_HDMI_AudioLevelShiftType
*
*  Defines the audio attenuation level to be shifted by the sink device 
*/
typedef enum
{
  QDI_HDMI_AUDIO_LEVEL_SHIFT_0DB   = 0x0,  /* 0 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_1DB,          /* 1 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_2DB,          /* 2 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_3DB,          /* 3 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_4DB,          /* 4 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_5DB,          /* 5 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_6DB,          /* 6 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_7DB,          /* 7 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_8DB,          /* 8 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_9DB,          /* 9 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_10DB,         /* 10 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_11DB,         /* 11 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_12DB,         /* 12 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_13DB,         /* 13 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_14DB,         /* 14 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_15DB,         /* 15 Decibel */
  QDI_HDMI_AUDIO_LEVEL_SHIFT_MAX,
  QDI_HDMI_AUDIO_LEVEL_SHIFT_FORCE_32BIT = 0x7FFFFFFF
}QDI_HDMI_AudioLevelShiftType;

/*!
* \b QDI_HDMI_AudioSpeakerLocationType
*
*  Defines various audio speaker locations that the sink device supports
*/
typedef enum
{
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_NONE   = 0x0,  /* No Speakers */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_FL     = 0x1,  /* FL  - Front Left */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_FC     = 0x2,  /* FC  - Front Center */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_FR     = 0x4,  /* FR  - Front Right */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_FLC    = 0x8,  /* FLC - Front Left Center */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_FRC    = 0x10, /* FRC - Front Right Center */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_RL     = 0x20, /* RL  - Rear Left */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_RC     = 0x40, /* RC  - Rear Center */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_RR     = 0x80, /* RR  - Rear Right */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_RLC    = 0x100,/* RLC - Rear Left Center */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_RRC    = 0x200,/* RRC - Rear Right Center */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_LFE    = 0x400,/* LFE - Low Frequency Effect */
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_MAX,
  QDI_HDMI_AUDIO_SPEAKER_LOCATION_FORCE_32BIT = 0x7FFFFFFF
}QDI_HDMI_AudioSpeakerLocationType;

/*!
* \b QDI_HDMI_ContentFilterType
*
*  Indicates if the sink can support various filtering pertaining to various content types.
*/
typedef enum
{
  QDI_HDMI_CONTENT_TYPE_NONE     = 0x0,
  QDI_HDMI_CONTENT_TYPE_GRAPHICS = 0x1,        /* Sink device applies no filtering to the pixel data */
  QDI_HDMI_CONTENT_TYPE_PHOTO    = 0x2,        /* Sink device supports filtering for still pictures */
  QDI_HDMI_CONTENT_TYPE_CINEMA   = 0x4,        /* Sink device supports filtering for cinema contents */
  QDI_HDMI_CONTENT_TYPE_GAME     = 0x8,        /* Sink device supports processing with low audio and video latency */
  QDI_HDMI_CONTENT_TYPE_MAX,
  QDI_HDMI_CONTENT_TYPE_32BIT    = 0x7FFFFFFF
}QDI_HDMI_ContentFilterType;



/*!
* \b QDI_Display_HDMI_AudioPropType
*
*  Defines the audio parameters required for HDMI display
*/
typedef struct
{
  bool32                           bAudioInfoPacket;      /* Enable/Disable Audio InfoFrame Packet */
  bool32                           bInhibitDownMix;       /* Enable/Disable Audio output down mixing */
  bool32                           bReserved[4];          /* Reserved for future use */
  QDI_HDMI_AudioSampleRateType     eAudioSampleRate;      /* Audio sample rate */
  QDI_HDMI_AudioChannelNumType     eAudioChannelNum;      /* Audio channel number */
  QDI_HDMI_AudioFormatType         eAudioFormat;          /* Audio format */
  QDI_HDMI_AudioSampleBitDepthType eAudioSampleBitDepth;  /* Audio bit depth */
  QDI_HDMI_AudioChannelAllocType   eChannelAllocCode;     /* Audio channel to speaker mapping */
  QDI_HDMI_AudioLevelShiftType     eLevelShiftValue;      /* Audio attenuation level shift */
  uint32                           uReserved[6];          /* Reserved for future */
} QDI_Display_HDMI_AudioPropType;

/*!
* \b QDI_Display_HDMI_AudioModeInfoPropType
*
*  Defines the audio capabilities supported by the HDMI display
*/
typedef struct
{
  uint32                                  uAudioModeIndex;                                      /* Index to query the various audio capabilities associated with a particular audio format */
  QDI_HDMI_AudioFormatType                eAudioFormat;                                         /* Type of audio format this mode is associated with */
  uint16                                  uSupportedAudioSampleRateMask;                        /* Various sample rates supported with this audio format type (Use QDI_HDMI_AudioSampleRateType) */
  uint16                                  uSupportedAudioChannelMask;                           /* Various audio channels supported with this audio format type (Use QDI_HDMI_AudioChannelNumType) */
  uint32                                  uSupportedSpeakerLocationMask;                        /* Various speaker locations supported by this sink device (Use QDI_HDMI_AudioSpeakerLocationType) */
  union 
  {
    uint32                                uSupportedAudioSampleBitDepthMask;                    /* For LPCM audio format, this field indicates the various sample bit depths supported (Use QDI_HDMI_AudioSampleBitDepthType) */ 
    uint32                                uAudioMaxBitRate;                                     /* For non LPCM audio formats, this field indicates maximum bit rate supported */
  }audioStreamInfo;
  uint32                                  uReserved[4];                                         /* Reserved for future */
} QDI_Display_HDMI_AudioModeInfoPropType;

/*!
* \b  QDI_HDMI_LipSyncInfoType
*
*  Defines the audio/video latency HDMI display
*/
typedef struct
{
  uint8            uVideoLatency;              /* Video latency in milliseconds for progressive video format */
  uint8            uInterlaceVideoLatency;     /* Video latency in milliseconds for interlaced video format */
  uint8            uAudioLatency;              /* Audio latency in milliseconds for progressive video format */
  uint8            uInterlaceAudioLatency;     /* Audio latency in milliseconds for interlaced video format */
} QDI_HDMI_LipSyncInfoType;


/*!
* \b QDI_Display_HDMI_VendorInfoPropType
*
*  Defines the vendor specific data block info supported by the HDMI display
*/
typedef struct
{
  bool32                                  bVideoOnly;                                           /*  HDMI display support DVI mode (Video only) or HDMI mode (Audio and Video) */
  QDI_HDMI_LipSyncInfoType                sLipSyncInfo;                                         /*  Video and audio latency info */
  uint8                                   uSupportedDeepColorModeMask;                          /*  Pixel depth for each deep color mode HDMI display can support (Use QDI_HDMI_DeepColorModeType) */
  uint8                                   uSupportedContentFilterMask;                          /*  Sink supports various content filters (Use QDI_HDMI_ContentFilterType) */
  uint16                                  uPhysicalAddr;                                        /*  16 bit physical address of the source device, upper 16 bit is ignored.*/
  bool32                                  bAISupport;                                           /*  Sink supports ACP, ISRC1 and ISRC2 packets */
  bool32                                  bDVIDualSupport;                                      /*  Sink supports DVI Dual mode */
  QDI_Display_ModeAspectRatioType         ePreferredAspectRatio;                                /*  Preferred aspect ratio reported by the Sink */
  uint32                                  uMaxScreenWidthInMillimeter;                          /*  Maximum screen width reported by the Sink in millimeters */
  uint32                                  uMaxScreenHeightInMillimeter;                         /*  Maximum screen height reported by the Sink in millimeters */
  uint8                                   uManufacturerId[2];                                   /*  Manufacturer ID : using EISA ID,  uManufacturerId[0] is low byte */
  uint16                                  uProductId;                                           /*  Product code,  used to differentiate between different models from the manufacturer */
  uint8                                   uNameDescriptionLength;                               /*  Sink name description length */                                       
  uint8                                   uNameDescription[13];                                 /*  Sink name description*/
  uint32                                  uReserved[2];                                         /*  Reserved for future */
} QDI_Display_HDMI_VendorInfoPropType;

/*!
* \b QDI_Display_HDMI_CEC_PropType
*
*  Defines the CEC message that sends/receives to/from HDMI sink device (Supplement 1 - HDMI Spec 1.3a)
*/
typedef struct
{
  uint32                                  uSrcAddr;                              /* Logical Address of the source device */
  uint32                                  uDestAddr;                             /* Logical Address of the destination device */
  uint32                                  uMsgID;                                /* Opcode ID for the CEC message */
  uint8                                  *pData;                                 /* Contains opcode parameters if any */
  uint32                                  uDataSize;                             /* Payload size */
  uint32                                  uFlags;                                /* QDI_FLAG_DISPLAY_HDMI_CEC_PING_MESSAGE can be set as part of this field to indicate if the message is indicated as a Ping/Polling Message */
} QDI_Display_HDMI_CEC_PropType;



/*!
* \b QDI_Display_3D_FrameFormatType
*
*  Defines the 3D frame format for the incoming surfaces 
*/
typedef struct
{
  QDI_3D_FrameFormatType e3DFrameFormat;    /* Various 3D packing frame format */
  uint32                 uReserved[10];     /* Reserved for future */
} QDI_Display_3D_FrameFormatType;


/*!
* \b QDI_HDMI_AVI_ScanType
*
*  Defines HDMI display scan type within the Auxiliary Video Info packet (AVI)
*/
typedef enum
{
  QDI_HDMI_AVI_SCAN_NONE = 0,  /* Scan type will be up to the default of the HDMI display*/
  QDI_HDMI_AVI_SCAN_OVERSCAN,  /* Select overscan option for the HDMI display */
  QDI_HDMI_AVI_SCAN_UNDERSCAN, /* Select underscan option for the HDMI display */
  QDI_HDMI_AVI_SCAN_MAX,
  QDI_HDMI_AVI_SCAN_FORCE_32BIT = 0x7FFFFFFF
} QDI_HDMI_AVI_ScanType;

/*!
* \b QDI_HDMI_AVI_LetterBoxInfoType
*
*  Defines letter box width and height
*/
typedef struct
{

  uint32 uEndOfTopBarLine;      /* The last line number of the top letter box bar */
  uint32 uStartOfBottomBarLine; /* The starting line number of the bottom letter box bar */
  uint32 uEndOfLeftBarLine;     /* The last column of the left letter box bar */
  uint32 uStartOfRightBarLine;  /* The starting column of the right letter box bar */
} QDI_HDMI_AVI_LetterBoxInfoType;

/*!
* \b QDI_HDMI_AVI_ActiveFormatType
*
*  Defines the display aspect ratio of the active pixels
*/
typedef enum
{
  QDI_HDMI_AVI_ACTIVE_FORMAT_NONE = 0,
  QDI_HDMI_AVI_ACTIVE_FORMAT_RESERVED,                 /* Reserved for future use */
  QDI_HDMI_AVI_ACTIVE_FORMAT_BOX_16_9_TOP,             /* Active format is 16:9 and shift top */
  QDI_HDMI_AVI_ACTIVE_FORMAT_BOX_14_9_TOP,             /* Active format is 14:9 and shift top */
  QDI_HDMI_AVI_ACTIVE_FORMAT_BOX_16_9_CENTER,          /* Active format is 16:9 and shift center */
  QDI_HDMI_AVI_ACTIVE_FORMAT_RESERVED1,                /* Reserved for future use */
  QDI_HDMI_AVI_ACTIVE_FORMAT_RESERVED2,                /* Reserved for future use */
  QDI_HDMI_AVI_ACTIVE_FORMAT_RESERVED3,                /* Reserved for future use */
  QDI_HDMI_AVI_ACTIVE_FORMAT_CODED_FRAME,              /* Active format is same as coded frame */
  QDI_HDMI_AVI_ACTIVE_FORMAT_4_3_CENTER,               /* Active format is 4:3 shifted center */
  QDI_HDMI_AVI_ACTIVE_FORMAT_16_9_CENTER,              /* Active format is 16:9 shifted center */
  QDI_HDMI_AVI_ACTIVE_FORMAT_14_9_CENTER,              /* Active format is 14:9 shifted center */
  QDI_HDMI_AVI_ACTIVE_FORMAT_RESERVED4,                /* Reserved for future use */
  QDI_HDMI_AVI_ACTIVE_FORMAT_4_3_PROTECT_14_9_CENTER,  /* Active format is 4:3 with shoot and protect and 14:9 center*/
  QDI_HDMI_AVI_ACTIVE_FORMAT_16_9_PROTECT_14_9_CENTER, /* Active format is 16:9 with shoot and protect and 14:9 center*/
  QDI_HDMI_AVI_ACTIVE_FORMAT_16_9_PROTECT_4_3_CENTER,  /* Active format is 16:9 with shoot and protect and 4:3 center*/
  QDI_HDMI_AVI_ACTIVE_FORMAT_MAX,
  QDI_HDMI_AVI_ACTIVE_FORMAT_FORCE_32BIT = 0x7FFFFFFF,
} QDI_HDMI_AVI_ActiveFormatType;


/*!
* \b QDI_Display_HDMI_AVIPacketPropType
*
*  Defines the HDMI Auxiliary Video Info Packet 
*/
typedef struct
{
  bool32                          bEnableAVIPacket;               /* Enable/Disable Auxilary Video Info (AVI) Packet */
  QDI_HDMI_AVI_ScanType           eScanInfo;                      /* Configure scan option of the HDMI display */
  QDI_HDMI_AVI_LetterBoxInfoType  sLetterBoxInfo;                 /* Configure the Letter box width/height */
  QDI_HDMI_AVI_ActiveFormatType   eActiveFormat;                  /* Describes content presentation with various aspect ratio */
  QDI_HDMI_ContentFilterType      eContentFilter;                 /* Configure the type of content filter supported by the sink */
  uint32                          uReserved[7];                   /* Reserved for future use */
} QDI_Display_HDMI_AVIPacketPropType;


/*!
* \b QDI_HDMI_ACP_StreamType
*
*  Defines audio content protection type
*/
typedef enum
{
  QDI_HDMI_ACP_STREAM_TYPE_GENERIC_AUDIO = 0,            /* Generic audio stream type */
  QDI_HDMI_ACP_STREAM_TYPE_IEC_60958_AUDIO,              /* IEC 60958 audio stream type */
  QDI_HDMI_ACP_STREAM_TYPE_DVD_AUDIO,                    /* DVD audio stream type */
  QDI_HDMI_ACP_STREAM_TYPE_SUPER_AUDIO_CD,               /* Super audio stream type */
  QDI_HDMI_ACP_STREAM_TYPE_MAX,
  QDI_HDMI_ACP_STREAM_TYPE_FORCE_32BIT = 0x7FFFFFFF,
} QDI_HDMI_ACP_StreamType;

/*!
* \b QDI_Display_HDMI_ACP_DVDAudioType
*
*  Defines the ACP payload for DVD Audio content
*/
typedef struct
{
  uint8   uCopyPermission;       /* Audio copy permission bit (6-8) of payload byte 0 */
  uint8   uCopyNumber;           /* Audio copy number bit (3-5) of payload byte 0*/
  uint8   uQuality;              /* Audio quality bit (1-2) of payload byte 0 */
  uint8   uTransaction;          /* Audio transaction bit(0) of payload byte 0 */
  uint8   uReserved[27];         /* DVD stream type has 27 bytes reserved */
}QDI_Display_HDMI_ACP_DVDAudioType;


/*!
* \b QDI_Display_HDMI_SuperAudioCDType
*
*  Defines the ACP payload for DVD Audio content
*/
typedef struct
{
  uint8   uContentControlInfo[16];    /* Additional content control info (CCI). See Super Audio CD specification */
  uint8   uReserved[12];              /* Super Audio CD stream type has 12 bytes reserved */
}QDI_Display_HDMI_SuperAudioCDType;

/*!
* \b QDI_Display_HDMI_ACPPacketPropType
*
*  Defines the properties of an Audio Content Protection packet
*/
typedef struct
{
  bool32                                   bEnableACPPacket;               /* Enable/Disable Audio Content Protection (ACP) Packet */
  QDI_HDMI_ACP_StreamType                  eACPStreamType;                 /* Indicate the type of audio content protection */
  union
  {
    QDI_Display_HDMI_ACP_DVDAudioType      sDVDStream;                     /* ACP Payload for DVD stream type */
    QDI_Display_HDMI_SuperAudioCDType      sSuperAudioStream;              /* ACP Payload for Super Audio CD stream type */
  }payload;
  uint32                                   uReserved[10];                  /* Reserved for future use */
} QDI_Display_HDMI_ACPPacketPropType;


/*!
* \b QDI_Display_HDMI_ISRCPacketPropType
*
*  Defines the properties of an International Standard Recording Code packet
*/
typedef struct
{
  bool32                          bEnableISRCPacket;              /* Enable/Disable International standard recording (ISRC) Packet */
  uint32                          uISRCStatusCode;                /* Indicate the ISRC status code See DVD specification for Read only disc section Part 4 */
  uint8                           uISRCCode1[16];                 /* Indicate the ISRC digits / characters in each byte for first ISRC code */
  uint8                           uISRCCode2[16];                 /* Indicate the ISRC digits / characters in each byte for second ISRC code (Optional) */
  uint32                          uReserved[10];                  /* Reserved for future use */
} QDI_Display_HDMI_ISRCPacketPropType;

/*!
* \b QDI_HDCP_STATUS
*
*  Defines HDCP status
*/
typedef enum
{
  QDI_DISPLAY_HDCP_STATUS_IDLE = 0,            /* HDCP is not enabled */
  QDI_DISPLAY_HDCP_STATUS_NORMAL,              /* HDCP link passed authentication */
  QDI_DISPLAY_HDCP_STATUS_LINK_LOST,           /* HDCP link failed authentication */
  QDI_DISPLAY_HDCP_STATUS_AUTHENTICATION,      /* HDCP link authentication/reauthentication in process */
  QDI_DISPLAY_HDCP_STATUS_MAX,
  QDI_DISPLAY_HDCP_STATUS_FORCE_32BIT = 0x7FFFFFFF
} QDI_Display_HDCP_Status;

/*!
* \b QDI_Display_HDCPLevelType
*
*  Defines the HDCP level enforced on HDCP capable end points.
*/
typedef enum
{
  QDI_DISPLAY_HDCP_LEVEL_1p4 = 0,              /**< HDCP 1.4 */
  QDI_DISPLAY_HDCP_LEVEL_2p2,                  /**< HDCP 2.2 no type enforcement */
  QDI_DISPLAY_HDCP_LEVEL_2p2_Type1,            /**< HDCP 2.2 enforcing Type 1 rules */
  QDI_DISPLAY_HDCP_LEVEL_MAX,
  QDI_DISPLAY_HDCP_LEVEL_FORCE_32BITS = 0x7FFFFFFF
} QDI_Display_HDCPLevelType;

/*!
* \b QDI_Display_MetadataActionType
*
*  Defines an action to take on a metadata format on a display.
*/
typedef enum
{
  QDI_DISPLAY_METADATA_ACTION_SET         = 0x0,   /**< Set new metadata for a format */
  QDI_DISPLAY_METADATA_ACTION_CLEAR       = 0x1,   /**< Send no metadata for a format */
  QDI_DISPLAY_METADATA_ACTION_MAX         = 0x2,
  QDI_DISPLAY_METADATA_ACTION_FORCE_32BIT = 0x7FFFFFFF,
} QDI_Display_MetadataActionType;

/*!
* \b QDI_Display_HDR10Metadata_InfoType
*
*  Defines HDR10 Metadata definition.
*/
typedef struct
{
  QDI_Display_ChromaticityInfoType       sChromaticityInfo;            /**< Red, Green, Blue and White point chromaticity coordinates */
  uint32                                 uMaxMasteringLuminance;       /**< The maximum number of millinits (1/1000 of a nit) of the display used to master the content */
  uint32                                 uMinMasteringLuminance;       /**< The minimum number of millinits (1/1000 of a nit) of the display used to master the content */
  uint32                                 uMaxContentLightLevel;        /**< The maximum light level (MaxCLL) in millinits (1/1000 of a nit) corresponding to the brightest pixel in the content */
  uint32                                 uMaxFrameAverageLightLevel;   /**< The maximum frame average light level (MaxFALL) in millinits (1/1000 of a nit) for any single frame  which has the brightest average luminance in the content*/
} QDI_Display_HDR10Metadata_InfoType;

/*!
* \b QDI_Display_MetadataFormatType
*
*  Defines the format of metadata. (HDR, Proprietary, etc.)
*/
typedef enum
{
  QDI_DISPLAY_METADATA_FORMAT_UNDEFINED        = 0x0,         /**< Undefined metadata format */
  QDI_DISPLAY_METADATA_FORMAT_HDR10            = 0x1,         /**< HDR10 metadata format used with QDI_Display_HDR10Metadata_InfoType */
  QDI_DISPLAY_METADATA_FORMAT_HDR10PLUS        = 0x2,         /**< HDR10PLUS metadata format */
  QDI_DISPLAY_METADATA_FORMAT_PROPRIETARY_SDP0 = 0x3,         /**< Proprietary SDP 0 metadata format */
  QDI_DISPLAY_METADATA_FORMAT_PROPRIETARY_SDP1 = 0x4,         /**< Proprietary SDP 1 metadata format */
  QDI_DISPLAY_METADATA_FORMAT_PROPRIETARY_SDP2 = 0x5,         /**< Proprietary SDP 2 metadata format */
  QDI_DISPLAY_METADATA_FORMAT_MAX              = 0x6,
  QDI_DISPLAY_METADATA_FORMAT_FORCE_32BIT      = 0x7FFFFFFF,
} QDI_Display_MetadataFormatType;

/*!
* \b QDI_Display_HDCP_InfoType
*
*  Defines HDCP details of a device connected to the protected interface port.
*/
typedef struct
{
  bool32                       bRepeater;                          /**< Indicate if the device connected to the protected output is a repeater */
  uint8                        uKSV[5];                            /**< KSV of the device connected to protected interface port */
  uint32                       uDevCount;                          /**< Number of downstream devices connected to the repeater */
} QDI_Display_HDCP_InfoType;

/*!
* \b QDI_Display_KsvType
*
*  Defines key selection vector (KSV) of the HDCP device
*/
typedef struct
{
  uint8 uKSV[5];                                                   /**< KSV of a HDCP device */
} QDI_Display_KsvType;

/*!
* \b QDI_Display_KSV_ListType
*
*  Defines KSV List information of the downstream devices connected to repeater.
*/
typedef struct
{
  uint32                       uBufferSize;                        /**< Size in bytes of the psKSV buffer */
  QDI_Display_KsvType          *psKSV;                             /**< Pointer to KSV array containing the KSV of all downstream devices from the repeater */
} QDI_Display_KSV_ListType;

/*!
* \b QDI_Display_FrameBuffer_WriteBack_SurfaceAttrType
*
*  Defines Write Back Surface Information
*/
typedef struct
{
  QDI_SurfaceAttrType        sSurface;                /**< Indicate the surface passing to display for write back */
  bool32                     bFixedAlpha;             /**< Setting this field to true, will use fixed alpha value in uFixedAlphaValue for ARGB8888 color format when output to framebuffer, *
                                                       **< otherwise, propagate alpha value from source */
  uint8                      uFixedAlphaValue;        /**< Indicate the fixed alpha value for ARGB8888 color format if use fixed alpha */
  QDI_RectType               sCroppingRect;           /**< Indicate the output cropping rectangle for surface update */
} QDI_Display_FrameBuffer_WriteBack_SurfaceAttrType;

/*!
* \b QDI_Display_FrameBuffer_WriteBack_ConfigType
*
*  Defines Write Back Configuration
*/
typedef struct
{
  bool32                                                   bEnable;                 /**< Indicate enable or disable write back */
  QDI_Display_FrameBuffer_WriteBack_SurfaceAttrType      **pSurfaceList;            /**< Indicate the surface list passing to display for write back, it's a array pointer */
  uint32                                                   uNumOfSurfaces;          /**< Indicate the number of surface in the list */
  uint32                                                   uRefreshRate;            /**< Indicate refresh rate to output */
} QDI_Display_FrameBuffer_WriteBack_ConfigType;

/*!
* \b QDI_Display_FrameBuffer_WriteBack_InfoType
*
*  Defines Write Back Status
*/
typedef struct
{
  uint32                     uCurrentSurfaceIndex;    /**< Indicate the index of last updated buffer in the pSurfaceList for Get Property or buffer index to update for Set Property */
} QDI_Display_FrameBuffer_WriteBack_InfoType;

/*!
* \b QDI_Display_BrightnessPropType
*
*  The structure used to set/get the brightness level using QDI_DISPLAY_PROPERTY_BRIGHTNESS_CONFIG
*  With SetProperty, user specifies the new brightness level and transition time the driver should take to set the new level.
*  If the specified transition time is zero then user is requesting to set the brightness level immediately
*  With GetProperty, user wants to know the current brightness level and how much time is left for the previous requested level.
*  If the current transition has been completed the driver will return the previous requested level, otherwise the returned brightness
*  value will be somewhere between the last and requested level.
*/
typedef struct 
{
  uint32                     uLevel;                          /**< Brightness level in millinits (1/1000 of a nit) */
  uint32                     uTransitionTimeMs;               /**< Transition time (in millisec) */
}QDI_Display_BrightnessPropType;

/*!
* \b QDI_Display_ProgrammableLineInterruptType
*
* This structure is used to configure number of line interrupts per display using QDI_DISPLAY_PROGRAMMABLE_LINE_INTERRUPT property.
*/
typedef struct
{
  uint32              uIndex;                       /**< Line interrupt instance, if multiple line interrupts are supported */
  bool32              bEnable;                      /**< Enable/Disable line interrupts for given display */
  uint32              uTimeOffsetInUs;              /**< Time offset from the Vsync, which may be rounded to nearest line number */
  void               *pContext;                     /**< Additional information of line interrupts */
} QDI_Display_ProgrammableLineInterruptType;

/*!
* \b QDI_Display_RoutingType
*
* This structure contains audio routing information such as controller ID, stream ID.
*/
typedef struct
{
  uint32              uControllerId;                /**< DP Controller index */
  uint32              uStreamId;                    /**< Muti stream (MST) Video stream index */
} QDI_Display_RoutingType;

/*!
* \b QDI_Display_MetadataConfigPropType
*
* Defines the configuration for a metadata format on a QDI Display.
*/
typedef struct
{
  QDI_Display_MetadataActionType  eAction;       /**< Action to take on a metadata format */
  QDI_Display_MetadataFormatType  eFormat;       /**< Format of this metadata (HDR, proprietary) */
  uint32                          uInterfaceId;  /**< Bitmask of the display interfaces to send metadata on, if more than one interface is used for display.
                                                      If 0, then metadata is broadcast to all interfaces. Bit 0 is interface 0, and so on. */
  uint32                          uMetadataSize; /**< Size of metadata in bytes */
  uint8                          *puMetadata;    /**< Metadata buffer */
} QDI_Display_MetadataConfigPropType;

/*!
* \b QDI_Display_MetadataListPropType
*
* Defines the configuration of metadata formats on a QDI Display.
* The metadata is persistent until configuration is changed.
*/
typedef struct
{
  QDI_Display_MetadataConfigPropType  *psConfigs;         /**< Metadata configuration list */
  uint32                               uNumberOfConfigs;  /**< Number of metadata configurations in list */
} QDI_Display_MetadataListPropType;

/*!
* \b QDI_Display_PropertyParamsType
*
*  Defines the union of all the types  used for GetProperty and SetProperty
*/
typedef union
{
  bool32                                         bEnabled;              /**< QDI_DISPLAY_PROPERTY_DITHER, DISPLAY_PROPERTY_DEFLICKER, QDI_DISPLAY_PROPERTY_CONTENT_ADAPTIVE_BACKLIGHT,
                                                                             QDI_DISPLAY_PROPERTY_CONTENT_PROTECTION, QDI_DISPLAY_PROPERTY_SMOOTH_BACKLIGHT */
  QDI_Display_VisibleStateType                   eState;                /**< QDI_DISPLAY_PROPERTY_VISIBLESTATE */
  bool32                                         bPowerOn;              /**< QDI_DISPLAY_PROPERTY_POWER */
  uint32                                         uBackLightLevel;       /**< QDI_DISPLAY_PROPERTY_BACKLIGHT */
  QDI_Display_GammaControlPropType               sGammaControlParams;   /**< QDI_DISPLAY_PROPERTY_GAMMA_CORRECTION */
  QDI_Display_PCCPropType                        sPCCParams;            /**< QDI_DISPLAY_PROPERTY_PCC */
  QDI_RotateFlipType                             eRotateFlip;           /**< QDI_DISPLAY_PROPERTY_ROTATION */
  QDI_Display_VSyncPropType                      sVsyncParam;           /**< QDI_DISPLAY_PROPERTY_VSYNC */
  uint32                                         uLineCount;            /**< QDI_DISPLAY_PROPERTY_GETSCANLINE */
  uint32                                         uModeIndex;            /**< QDI_DISPLAY_PROPERTY_MODE_INDEX */
  QDI_Display_ModeInfoType                       sModeInfo;             /**< QDI_DISPLAY_PROPERTY_MODE_INFO.*/
  QDI_Display_CrcPropType                        sCrcParams;            /**< QDI_DISPLAY_PROPERTY_CRC */
  bool32                                         bCommitDone;           /**< QDI_DISPLAY_PROPERTY_COMMIT_DONE */
  QDI_Display_PrimarySurfacePropType             sPrimarySurface;       /**< QDI_DISPLAY_PROPERTY_PRIMARY_SURFACE */
  QDI_Display_ContentABLInfoType                 sContentAblInfo;       /**< QDI_DISPLAY_PROPERTY_CONTENT_ADAPTIVE_BACKLIGHT  */
  QDI_Display_AmbientABLInfoType                 sAmbientAblInfo;       /**< QDI_DISPLAY_PROPERTY_AMBIENT_ADAPTIVE_BACKLIGHT  */
  QDI_Display_ScreenCapturePropType              sScreenCapture;        /**< QDI_DISPLAY_PROPERTY_SCREEN_CAPTURE */
  QDI_Display_HDMI_AudioPropType                 sHdmiAudioParam;       /**< QDI_DISPLAY_PROPERTY_HDMI_AUDIO */
  QDI_Display_HDMI_AudioModeInfoPropType         sHdmiAudioModeInfo;    /**< QDI_DISPLAY_PROPERTY_HDMI_AUDIO_MODE_INFO */
  QDI_Display_HDMI_VendorInfoPropType            sHdmiVendorInfo;       /**< QDI_DISPLAY_PROPERTY_HDMI_VENDOR_INFO */
  QDI_Display_HDMI_AVIPacketPropType             sHdmiAVIPacket;        /**< QDI_DISPLAY_PROPERTY_HDMI_AVI_PACKET */
  QDI_Display_HDMI_ISRCPacketPropType            sHdmiISRCPacket;       /**< QDI_DISPLAY_PROPERTY_HDMI_ISRC_PACKET */
  QDI_Display_HDMI_ACPPacketPropType             sHdmiACPPacket;        /**< QDI_DISPLAY_PROPERTY_HDMI_ACP_PACKET */
  QDI_Display_3D_FrameFormatType                 s3DFrameFormat;        /**< QDI_DISPLAY_PROPERTY_3D_FRAME_FORMAT */
  QDI_Display_HDMI_CEC_PropType                  sHdmiCecMsgInfo;       /**< QDI_DISPLAY_PROPERTY_HDMI_CEC_MSG */
  QDI_HISCSettingsType                           sHSICSettings;         /**< QDI_DISPLAY_PROPERTY_HSIC */
  QDI_Display_SynchronizedRefreshType            sSyncRefresh;          /**< QDI_DISPLAY_PROPERTY_SYNCHRONIZED_REFRESH */
  QDI_Display_RawEDIDInfoSizePropType            sRawEdidInfoSize;      /**< QDI_DISPLAY_PROPERTY_RAW_EDID_INFO_LENGTH */
  QDI_Display_RawEDIDInfoPropType                sRawEdidInfo;          /**< QDI_DISPLAY_PROPERTY_RAW_EDID_INFO */
  bool32                                         bConnected;            /**< QDI_DISPLAY_PROPERTY_PANEL_CONNECTION_STATUS */
  QDI_Display_HDCP_InfoType                      sHDCPInfo;             /**< QDI_DISPLAY_PROPERTY_HDCP_INFO */
  QDI_Display_KSV_ListType                       sDownstreamKSVList;    /**< QDI_DISPLAY_PROPERTY_DOWNSTREAM_KSV_LIST */
  QDI_Display_HDCP_Status                        eHDCPStatus;           /**< QDI_DISPLAY_PROPERTY_HDCP_STATUS */
  uint32                                         uCABLLutLength;        /**< QDI_DISPLAY_PROPERTY_CABL_LUT_LENGTH */
  QDI_Display_ContentABLStatusType               sCABLGetInfo;          /**< QDI_DISPLAY_PROPERTY_CONTENT_ADAPTIVE_BACKLIGHT */
  QDI_Display_FrameBuffer_WriteBack_ConfigType   sWriteBackConfig;      /**< QDI_DISPLAY_PROPERTY_FRAMEBUFFER_WRITEBACK_CONFIGTYPE */
  QDI_Display_FrameBuffer_WriteBack_InfoType     sWriteBackInfo;        /**< QDI_DISPLAY_PROPERTY_FRAMEBUFFER_WRITEBACK_INFOTYPE */
  QDI_Display_SmoothBacklightPropType            sSmoothBacklightInfo;  /**< QDI_DISPLAY_PROPERTY_SMOOTH_BACKLIGHT */
  QDI_Display_RefreshRateType                    sRefreshRateInfo;      /**< QDI_DISPLAY_PROPERTY_DYNAMIC_REFRESH_RATE */
  uint32                                         uBorderColorRgb888;    /**< QDI_DISPLAY_PROPERTY_BORDER_COLOR Bits[16-23]:Red, Bits[8-15]:Green, Bits[0-7]:Blue */
  QDI_Display_DestScalerConfigType               sDestScaleConfig;      /**< QDI_DISPLAY_PROPERTY_DESTINATION_SCALE */
  QDI_Display_HDCPLevelType                      eHDCPLevel;            /**< QDI_DISPLAY_PROPERTY_HDCP_LEVEL */
  QDI_Display_BrightnessPropType                 sBrightnessConfig;     /**< QDI_DISPLAY_PROPERTY_BRIGHTNESS_CONFIG */
  QDI_Display_ProgrammableLineInterruptType      sLineInterruptConfig;  /**< QDI_DISPLAY_PROPERTY_PROGRAMMABLE_LINE_INTERRUPT */
  QDI_Display_RoutingType                        sRoutingInfo;          /**< QDI_DISPLAY_PROPERTY_ROUTING_INFO */
  QDI_Display_MetadataListPropType               sMetadataList;         /**< QDI_DISPLAY_PROPERTY_METADATA */
  QDI_Color_EncodingType                         eColorBlendSpace;      /**< QDI_DISPLAY_PROPERTY_BLEND_SPACE */
} QDI_Display_PropertyParamsType;


/* -----------------------------------------------------------------------
** 3. Layer
** ----------------------------------------------------------------------- */

/*!
* \b QDI_Layer_CapsType
* 
* Structure containing information indicating layer capabilities.
*
*/
typedef struct 
{
  bool32    bHistogramEnhance;                          /**< Indicate if layer support histogram enhance */
  bool32    bDither;                                    /**< Indicate if layer support dithering */
  bool32    bDeinterlace;                               /**< Indicate if layer support deinterlacing */
  bool32    bColorSpaceConversion;                      /**< Indicate if layer support color space conversion*/
  bool32    bInverseGammaCorrection;                    /**< Indicate if layer support inverse color space conversion.*/
  bool32    bRotation;                                  /**< Indicate if layer support 0, 90, 180, 270 degree rotation .*/
  bool32    bMirror;                                    /**< Indicate if layer support horizontal or vertical mirror mode. */
  bool32    bSurfaceFlipVsync;                          /**< Indicate if layer can flip on VSYNC. */
  bool32    bSurfaceFlipHsync;                          /**< Indicate if layer can flip on HSYNC. */
  uint32    uMaxDownScaleRatio;                         /**< Indicate maximum down-scale ratio.Real down-scale ratio is 1/uMaxUpScaleRatio.*/
  uint32    uMaxUpScaleRatio;                           /**< Indicate maximum up-scale ratio. */
  uint32    uMaxSourceWidth;                            /**< Indicate maximum source width. */
  uint32    uMaxSourceHeight;                           /**< Indicate maximum source height. */
  bool32    bColorFill;                                 /**< Indicate if layer support filling with constant color.*/
  bool32    bPixelFormatSupported[QDI_PIXEL_FORMAT_MAX];
  uint32    uPixelFormatCaps[QDI_PIXEL_FORMAT_MAX];     /**< Flags to indicate the pixel format capabilities like BWC, TILE-MODE, Decimation etc. Please refer to QDI_PixelFormatCapType*/
  bool32    bMultipipeSupported;                        /**< Indicates if layer supports multipipe stitching. */
  bool32    bContentProtection;                         /**< Indicates if layer supports content protection (fetching from surfaces allocated from a secure context bank) */
} QDI_Layer_CapsType;


/*!
* \b QDI_Layer_InfoType
*
* Structure containing information pertaining to a particular
* layer.   
*
*/
typedef struct
{
  QDI_Display_IDType                 eAttachedDisplayId; /**< The layer is connected to this display*/
  QDI_PixelFormatType                eSrcFormat;         /**< The source surface format*/
  QDI_Layer_CapsType                 sLayerCaps;         /**< Stores the feature map and current enable/disable state of features */
  bool32                             bInUse;             /**< Indicate if layer is in use*/
  QDI_RectType                       sSrcRect;           /**< The source rectangle*/
  QDI_RectType                       sDestRect;          /**< The destination rectangle*/
  QDI_ZorderType                     eZorder;            /**< The z-order when doing blend */
} QDI_Layer_InfoType;


/*!
* \b QDI_Layer_OpenParamsType
*
*  Structure holds parameters used during layer initialization
*/
typedef struct
{
  QDI_Layer_IDType           eLayerID; /**< Id of the layer to be initialized. */
  QDI_AccessLevelType        eLevel;   /**< for future use */
  QDI_Layer_IDType          *pId;      /**< Id returned after init */ 
} QDI_Layer_OpenParamsType;

/*!
* \b QDI_Layer_PropertyType
*
*  Defines list of layer properties that can be modified 
*/
typedef enum
{
  QDI_LAYER_PROPERTY_VISIBLESTATE = 0,                    /**< Used to enable layer output.*/
  QDI_LAYER_PROPERTY_HIST_ENHANCE,                        /**< Used to get or set histogram enhancement.*/
  QDI_LAYER_PROPERTY_DITHER,                              /**< Used to enable or disable dithering.*/
  QDI_LAYER_PROPERTY_DEINTERLACE,                         /**< Used to enable or disable deinterlacing */
  QDI_LAYER_PROPERTY_HSIC,                                /**< Used to configure layer hue, saturation, intensity and contrast on a vg layer. */
  QDI_LAYER_PROPERTY_DEVICE_IGC,                          /**< Used to enable/disable inverse gamma control */
  QDI_LAYER_PROPERTY_ROTATION_FLIP,                       /**< Used to get or set rotation and flip parameters.*/
  QDI_LAYER_PROPERTY_CROPPING_RECTANGLE,                  /**< Used for scaling and cropping.*/
  QDI_LAYER_PROPERTY_FLIPSURFACE_METHOD,                  /**< Indicate if need wait for VSYNC when flip surface.*/
  QDI_LAYER_PROPERTY_BLEND,                               /**< Used to set or get blending parameters.*/
  QDI_LAYER_PROPERTY_COLORKEY,                            /**< Used to set or get colorkey range. */
  QDI_LAYER_PROPERTY_ZORDER,                              /**< Used to set or get z order of layers. */
  QDI_LAYER_PROPERTY_CURRENT_SURFACE,                     /**< Used to set or get current surface. */
  QDI_LAYER_PROPERTY_COMMIT_DONE,                         /**< Used for waiting for commit done when use ASYNC commit.*/
  QDI_LAYER_PROPERTY_COLORFILL,                           /**< Used for filling the layer with constant color.*/
  QDI_LAYER_PROPERTY_SHARPENING,                          /**< Used for setting and getting sharpening level.*/
  QDI_LAYER_PROPERTY_DEVICE_HANDLE_INFO,                  /**< Used for getting device handle.*/
  QDI_LAYER_PROPERTY_CSC_ENCODING,                        /**< Used to select the CSC encoding matrix for YUV to RGB conversion. */
  QDI_LAYER_PROPERTY_CURRENT_SURFACE_ADDRESS,             /**< Used to get the current base address of surface attached to the layer. */
  QDI_LAYER_PROPERTY_MULTIPIPE,                           /**< Used to link multiple layers and virtualize them into one layer for large resolution output and large surface support. */
  QDI_LAYER_PROPERTY_PARTIAL_UPDATE_RECTANGLE,            /**< Used for specifying the partial update rectangle region within the source or destination rectangle.*/
  QDI_LAYER_PROPERTY_CRC,                                 /**< Used for getting the layer CRC value. */
  QDI_LAYER_PROPERTY_SDRWHITE_LEVEL,                      /**< Used for setting SDR white level specified in millinits (1/1000 of a nit) */
  QDI_LAYER_PROPERTY_MAX,
  QDI_LAYER_PROPERTY_FORCE_32BIT = 0x7FFFFFFF
} QDI_Layer_PropertyType;

/*!
* \b QDI_Layer_VisibleStateType
*
*  Defines list of possible visible state of a layer 
*/
typedef enum
{
  QDI_LAYER_STATE_OFF = 0,
  QDI_LAYER_STATE_ON ,
  QDI_LAYER_STATE_MAX ,
  QDI_LAYER_STATE_FORCE_32BIT = 0x7FFFFFFF
}QDI_Layer_VisibleStateType;

/*!
* \b QDI_Layer_HistPropType
*
*  Defines if the histogramm collection is enabled.
*  It will also provide the tables that are used for the feature 
*  Tables could be readif GetProperty is called
*/
typedef struct
{
  bool32        bEnabled;
  void         *pTables;
  uint32        uSizeInByte;
} QDI_Layer_HistPropType;

/*!
* \b QDI_Layer_Cropping_RectanglePropType
*
*  Defines the source rectangle of a surface and
*  its destination rectangle. That provides information
*  about the cropping operation and the scaling ratio.
*/
typedef struct
{
  QDI_RectType     sSrcRect; /**< Source rectangle */
  QDI_RectType     sDestRect; /**< Destination rectangle */
} QDI_Layer_Cropping_RectanglePropType;

/*!
* \b QDI_ColorkeyFormatType       
*
* Supported color key specific formats.
*
* The supported formats and their associated bit packings
* are listed below.
* Legend: 
*                    33222222222211111111110000000000  Bit                 \n
*                    10987654321098765432109876543210  Location            \n
*                   |--------------------------------|                     \n
*  ARGB_8888        |AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB|                     \n
*                   |--------------------------------|                     \n
*  YCBCR_888        |xxxxxxxxYYYYYYYYbbbbbbbbrrrrrrrr|                     \n
*                   |--------------------------------|                     \n
*                   |--------------------------------|                     \n
*                    33222222222211111111110000000000  Bit                 \n
*                    10987654321098765432109876543210  Location            \n
*
*
*/
typedef enum
{
  QDI_COLORKEY_FORMAT_NATIVE_RGB = 0,/**< Same as the native RGB surface format */
  QDI_COLORKEY_FORMAT_ARGB8888,
  QDI_COLORKEY_FORMAT_YCBCR888,
  QDI_COLORKEY_FORMAT_MAX,  
  QDI_COLORKEY_FORMAT_FORCE_32BIT = 0x7FFFFFFF
} QDI_ColorkeyFormatType;

/*!
* \b QDI_ColorKeyOpType
*
*  Defines information about color keying parameters
*  that will be used or that is being used ( if SetProperty or GetProperty is called )
*/
typedef struct
{
  bool32                       bColorKeyEnabled;       /**< Enable color key */
  QDI_ColorkeyFormatType       eColorKeyFormat;        /**< Color key format */
  uint32                       uColorKeyLowerRange;    /**< Color key lower range */
  uint32                       uColorKeyUpperRange;    /**< Color key upper range */
} QDI_ColorKeyOpType;
/*!
* \b QDI_Layer_Blending_PropType
*
*  Defines information about blending parameters
*  that will be used or that is being used ( if SetProperty or GetProperty is called )
*/
typedef struct
{
  bool32                       bAlphaBlendEnabled;    /**< Enable alpha blend */
  QDI_BlendOpType              sFgBlendOp;            /**< Blend operations for the layer as foreground */
  QDI_BlendOpType              sBgBlendOp;            /**< Blend operations for background layer */
  QDI_ColorKeyOpType           sFgColorKeyOp;         /**< Color key setting for the layer as foreground */
  QDI_ColorKeyOpType           sBgColorKeyOp;         /**< Color key setting for background layer */
  QDI_ZorderType               eZorder;               /**< z order */
} QDI_Layer_Blending_PropType;

/*!
* \b QDI_Layer_ColorFillPropType
*
*  Defines information about layer color fill parameters
*  that will be used or that is being used ( if SetProperty or GetProperty is called )
*/
typedef struct
{
  bool32                       bEnableColorFill;
  uint32                       uFillColor; 
  QDI_PixelFormatType          ePixelFormat;    /**< Pixel format. */
  uint32                       uWidthInPixel;   /**< Width in pixel */
  uint32                       uHeightInPixel;  /**< Height in pixel */
} QDI_Layer_ColorFillPropType;

/*!
* \b QDI_Layer_SharpeningPropType
*
*  Defines information about smoothing and sharpening parameters
*  that will be used or that are being used ( if SetProperty or GetProperty is called )
*/
typedef struct
{
  QDI_Layer_SharpeningModeType    eSharpenMode;     /**< 0 = None, 1 = Sharpen, 2 = Smooth */
  uint32                          uLevel;           /**< Percentage of smoothing/sharpening. 0 to 100 %. */
} QDI_Layer_SharpeningPropType;

/*!
* \b QDI_Layer_DeinterlacePropType
*
*  Defines information about deinterlacing parameters
*  that will be used or that are being used ( if SetProperty or GetProperty is called )
*/
typedef struct
{
  bool32                            bEnabled;              /**< Enable deinterlacing feature */
  bool32                            bReferenceOddField;    /**< 1 = Use odd lines as reference. Default even line are used */
} QDI_Layer_DeinterlacePropType;

/** Defines maximum number of layers that can be linked in multi-pipe mode */
#define QDI_MAX_LAYERS_MULTIPIPE         0x02

/** Defines maximum number of layers that can be linked to master layer */
#define QDI_MAX_LAYERS_TO_LINK           QDI_MAX_LAYERS_MULTIPIPE - 1

/*!
* \b QDI_Layer_MultiPipePropType
*
*  Defines information about multi-pipe parameters
*  that will be used or that are being used ( if SetProperty or GetProperty is called )
*  
*  This property should be set when single surface requires multiple layers to fetch
*  Calling QDI_Util_GetInfo(QDI_GETINFO_MULTIPIPE) is recommended to check if multiple layers is required
*  If QDI_Util_GetInfo(QDI_GETINFO_MULTIPIPE) returns that 2 or more layers are required for the given use case,
*  QDI client is responsible to call this API to link multiple layers together, 
*  the layer that property has been set to will work as master layer, all operations on other linked layers (slave layers) will be discarded
*
*  Set this property by specifying bMultiPipeEnabled = FALSE to unlink layers
*/
typedef struct
{
  bool32                                 bMultiPipeEnabled;                       /**< Enable layer multi-pipe feature */
  QDI_HandleType                         hListOfLayers[QDI_MAX_LAYERS_TO_LINK];   /**< Array of linked QDI layer handles  */
  uint32                                 uNumOfLinkedLayers;                      /**< Number of valid handles in hListOfLayers */ 
} QDI_Layer_MultiPipePropType;

/*!
* \b QDI_Layer_PartialUpdateRectPropType
*
* This property is used to define partial update rectangle or region of 
* interest (ROI) within the rectangle type specified by QDI_PartialUpdateCapType.  
* in display caps. When configured the driver may optimize data transfers by only considering 
* content within the ROI and ignore content outside of it.
*
* This property needs to be configured on 
* each configuration change (surface change and/or change in destination 
* or source rectangle).  The default behavior is to snap the partial update 
* ROI to the destination rectangle if this property is not explicitly set on 
* a configuration change.
*/
typedef struct
{
  QDI_RectType     sRect;  /**< Partial update rectangle with respect to rect specified by QDI_PartialUpdateCapType in display caps*/
} QDI_Layer_PartialUpdateRectPropType;

/*
* \b QDI_Layer_CrcPropType
*
* This property is used to capture the CRC across one or more frames. 
*/
typedef struct
{
  uint32    uCrcValue;     /**< CRC value */
  uint32    uFrameCount;   /**< Number of frames for CRC capture */
} QDI_Layer_CrcPropType;

/*!
* \b QDI_Layer_PropertyParamsType
*
*  Defines the union of all the types  used for GetProperty and SetProperty
*/
typedef union
{
  bool32                                     bEnabled;                   /**< QDI_LAYER_PROPERTY_DITHER, QDI_LAYER_PROPERTY_DEVICE_IGC */
  QDI_Layer_VisibleStateType                 eState;                     /**< QDI_LAYER_PROPERTY_VISIBLESTATE */
  QDI_Layer_HistPropType                     sHistParams;                /**< QDI_LAYER_PROPERTY_HIST_ENHANCE */
  QDI_RotateFlipType                         eRotateFlip;                /**< QDI_LAYER_PROPERTY_ROTATION_FLIP */
  QDI_Layer_Cropping_RectanglePropType       sRectParams;                /**< QDI_LAYER_PROPERTY_CROPPING_RECTANGLE */
  bool32                                     bFlipSurfaceVsync;          /**< QDI_LAYER_PROPERTY_FLIPSURFACE_METHOD */
  QDI_Layer_Blending_PropType                sBlendParams;               /**< QDI_LAYER_PROPERTY_BLEND */
  QDI_SurfaceAttrType                        sCurrentAttachedSurface;    /**< QDI_LAYER_PROPERTY_CURRENT_SURFACE */
  bool32                                     bCommitDone;                /**< QDI_LAYER_PROPERTY_COMMIT_DONE */
  QDI_Layer_ColorFillPropType                sColorFillParams;           /**< QDI_LAYER_PROPERTY_COLORFILL */
  QDI_Layer_SharpeningPropType               sSharpening;                /**< QDI_LAYER_PROPERTY_SHARPENING */
  QDI_HandleType                             hDevice;                    /**< QDI_LAYER_PROPERTY_DEVICE_HANDLE_INFO.*/
  QDI_Layer_DeinterlacePropType              sDeinterlaceParams;         /**< QDI_LAYER_PROPERTY_DEINTERLACE */
  QDI_HISCSettingsType                       sHSICSettings;              /**< QDI_LAYER_PROPERTY_HSIC */
  QDI_Color_EncodingType                     sCSCEncoding;               /**< QDI_LAYER_PROPERTY_CSC_ENCODING */
  QDI_AddressType                            sAddress;                   /**< QDI_LAYER_PROPERTY_CURRENT_SURFACE_ADDRESS */
  QDI_Layer_MultiPipePropType                sMultiPipeParams;           /**< QDI_LAYER_PROPERTY_MULTIPIPE */
  QDI_Layer_PartialUpdateRectPropType        sPartialUpdateRectParams;   /**< QDI_LAYER_PROPERTY_PARTIAL_UPDATE_RECTANGLE */
  QDI_Layer_CrcPropType                      sCrcParams;                 /**< QDI_LAYER_PROPERTY_CRC */
  uint32                                     uSDRWhiteLevel;             /**< QDI_LAYER_PROPERTY_SDRWHITE_LEVEL */
} QDI_Layer_PropertyParamsType;


/*!
* \b QDI_MultiPipeRequirementInfoType
*  Defines information about the number of layers required in multi-pipe mode, as well as the cropping rectangle for each layer
* 
*/
typedef struct
{
  uint32                                  uNumRequiredLayers;                             /**< Number of required layers */
  QDI_Layer_Cropping_RectanglePropType    sCroppingRectangles[QDI_MAX_LAYERS_MULTIPIPE];  /**< Source and destination rectangle for each required layer */
} QDI_MultiPipeRequirementInfoType;

/*!
* \b QDI_MultiPipe_InfoType
*  Defines information about layer requirements in multi-pipe mode given cropping rectangles and display mode index
*
*/
typedef struct
{
  struct 
  {
    QDI_HandleType                          hDisplay;                 /**< Display handle */
    uint32                                  uModeIndex;               /**< Display mode index */
    QDI_Layer_Cropping_RectanglePropType    sCroppingRectangle;       /**< Source and destination rectangle of a layer */
    QDI_FlagsType                           uFlags;                   /**< Flag to inform any additional operations */
  } sInputParams;
  
  struct 
  {
    QDI_MultiPipeRequirementInfoType        sLayerRequirementInfo;    /**< MultiPipe layer requirements */
  } sOutputParams;
} QDI_MultiPipe_InfoType;

/*!
* \b QDI_GetInfo_ID
*
* Defines QDI utility information ID, used by QDI_Util_GetInfo to specify the type of information client is retrieving
* 
*/
typedef enum
{
  QDI_GETINFO_NONE = 0,                  /**<  Not supported */
  QDI_GETINFO_MULTIPIPE,                 /**<  Used to retrieve number of layers and cropping rectangle requirements in multi-pipe mode */
  QDI_GETINFO_MAX,
  QDI_GETINFO_FORCE_32BIT = 0x7FFFFFFF
} QDI_GetInfo_ID;


/* -----------------------------------------------------------------------
** Deprecated section
**
** Please do not use it, this section will be remove soon.
**
** -----------------------------------------------------------------------*/

/** Deprecated */
#define  QDI_LAYER_ACCESS_EXCLUSIVE  QDI_ACCESS_EXCLUSIVE
#define  QDI_LAYER_ACCESS_SHARED     QDI_ACCESS_SHARED
#define  QDI_LAYER_ACCESS_MAX        QDI_ACCESS_MAX


#endif /* __QDI_TYPES_H__ */

