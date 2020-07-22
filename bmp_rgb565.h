/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BMP_RGB565_H_
#define _BMP_RGB565_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Include system header files -----------------------------------------------*/
#include <stdint.h>

/* Include user header files -------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/** @def
 * Enable needed fonts here
 */
// #define USE_FONT_4X6
// #define USE_FONT_5X8
// #define USE_FONT_5X12
// #define USE_FONT_6X8
#define USE_FONT_6X10
// #define USE_FONT_7X12
// #define USE_FONT_8X8
// #define USE_FONT_8X12
// #define USE_FONT_8X14
// #define USE_FONT_10X16
// #define USE_FONT_12X16
// #define USE_FONT_12X20
// #define USE_FONT_16X26
// #define USE_FONT_22X36
// #define USE_FONT_24X40
// #define USE_FONT_32X53

#ifndef COLOR_R
#define COLOR_R(_C_COLOR_) (uint8_t)((_C_COLOR_) >> 16)
#endif
#ifndef COLOR_G
#define COLOR_G(_C_COLOR_) (uint8_t)((_C_COLOR_) >> 8)
#endif
#ifndef COLOR_B
#define COLOR_B(_C_COLOR_) (uint8_t)(_C_COLOR_)
#endif
#ifndef COL_RGB_SET
#define COL_RGB_SET(_C_COLOR_) COLOR_R(_C_COLOR_), COLOR_G(_C_COLOR_), COLOR_B(_C_COLOR_)
#endif

/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef void *(*BMP_RGB565_Malloc_Function)(size_t);
typedef void (*BMP_RGB565_free_Function)(void *);

/* Exported enum tag ---------------------------------------------------------*/

/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
    /** 
 * Font source : https://www.mikrocontroller.net/user/show/benedikt
 *               https://www.mikrocontroller.net/topic/goto_post/1291943
 */
typedef struct
{
   uint8_t *p;
   int8_t char_width;
   int8_t char_height;
} BMP_RGB565_font_st;


#ifdef USE_FONT_4X6
extern const BMP_RGB565_font_st BMP_RGB565_FONT_4X6;
#endif
#ifdef USE_FONT_5X8
extern const BMP_RGB565_font_st BMP_RGB565_FONT_5X8;
#endif
#ifdef USE_FONT_5X12
extern const BMP_RGB565_font_st BMP_RGB565_FONT_5X12;
#endif
#ifdef USE_FONT_6X8
extern const BMP_RGB565_font_st BMP_RGB565_FONT_6X8;
#endif
#ifdef USE_FONT_6X10
extern const BMP_RGB565_font_st BMP_RGB565_FONT_6X10;
#endif
#ifdef USE_FONT_7X12
extern const BMP_RGB565_font_st BMP_RGB565_FONT_7X12;
#endif
#ifdef USE_FONT_8X8
extern const BMP_RGB565_font_st BMP_RGB565_FONT_8X8;
#endif
#ifdef USE_FONT_8X12
extern const BMP_RGB565_font_st BMP_RGB565_FONT_8X12;
#endif
#ifdef USE_FONT_8X14
extern const BMP_RGB565_font_st BMP_RGB565_FONT_8X14;
#endif
#ifdef USE_FONT_10X16
extern const BMP_RGB565_font_st BMP_RGB565_FONT_10X16;
#endif
#ifdef USE_FONT_12X16
extern const BMP_RGB565_font_st BMP_RGB565_FONT_12X16;
#endif
#ifdef USE_FONT_12X20
extern const BMP_RGB565_font_st BMP_RGB565_FONT_12X20;
#endif
#ifdef USE_FONT_16X26
extern const BMP_RGB565_font_st BMP_RGB565_FONT_16X26;
#endif
#ifdef USE_FONT_22X36
extern const BMP_RGB565_font_st BMP_RGB565_FONT_22X36;
#endif
#ifdef USE_FONT_24X40
extern const BMP_RGB565_font_st BMP_RGB565_FONT_24X40;
#endif
#ifdef USE_FONT_32X53
extern const BMP_RGB565_font_st BMP_RGB565_FONT_32X53;
#endif

/* Exported function prototypes ----------------------------------------------*/
extern void BMP_RGB565_setAllocFunc(BMP_RGB565_Malloc_Function, BMP_RGB565_free_Function);
extern uint8_t *BMP_RGB565_create(uint32_t, uint32_t);
extern void BMP_RGB565_free(uint8_t *);
extern uint32_t BMP_RGB565_getWidth(uint8_t *);
extern uint32_t BMP_RGB565_getHeight(uint8_t *);
extern uint32_t BMP_RGB565_getFileSize(uint8_t *);
extern uint32_t BMP_RGB565_getImageSize(uint8_t *);
extern uint32_t BMP_RGB565_getOffset(uint8_t *);
extern void BMP_RGB565_setPixelRGB(uint8_t *, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
extern void BMP_RGB565_getPixelRGB(uint8_t *, uint32_t, uint32_t, uint8_t *, uint8_t *, uint8_t *);
extern void BMP_RGB565_drawLineRGB(uint8_t *, int32_t, int32_t, int32_t, int32_t, uint8_t, uint8_t, uint8_t);
extern void BMP_RGB565_drawRectRGB(uint8_t *, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
extern void BMP_RGB565_fillRGB(uint8_t *, uint8_t, uint8_t, uint8_t);
extern uint8_t * BMP_RGB565_copy(uint8_t *);
extern void BMP_RGB565_drawTextRGB(uint8_t *, char *, BMP_RGB565_font_st, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
extern uint8_t *BMP_RGB565_resize_bicubic(uint8_t *, uint32_t, uint32_t);
extern int BMP_RGB565_colorScale(float, float, float, uint8_t *, uint8_t *, uint8_t *);

#ifdef __cplusplus
}
#endif

#endif /* _BMP_RGB565_H_ */

/***************************************************************END OF FILE****/
