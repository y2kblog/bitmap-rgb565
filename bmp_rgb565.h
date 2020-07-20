/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BMP_RGB565_H_
#define _BMP_RGB565_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Include system header files -----------------------------------------------*/
#include <stdint.h>

/* Include user header files -------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifndef COLOR_R
#define COLOR_R(_C_COLOR_)      (uint8_t)((_C_COLOR_)>>16)
#endif
#ifndef COLOR_G
#define COLOR_G(_C_COLOR_)      (uint8_t)((_C_COLOR_)>>8)
#endif
#ifndef COLOR_B
#define COLOR_B(_C_COLOR_)      (uint8_t)(_C_COLOR_)
#endif
#ifndef COL_RGB_SET
#define COL_RGB_SET(_C_COLOR_)  COLOR_R(_C_COLOR_),COLOR_G(_C_COLOR_),COLOR_B(_C_COLOR_)
#endif

/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef void * (*BMP_RGB565_Malloc_Function)(size_t);
typedef void   (*BMP_RGB565_free_Function)(void *);

/* Exported enum tag ---------------------------------------------------------*/
/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void		 BMP_RGB565_setAllocFunc(BMP_RGB565_Malloc_Function, BMP_RGB565_free_Function);
extern uint8_t * BMP_RGB565_create      (uint32_t, uint32_t);
extern void      BMP_RGB565_free        (uint8_t *);
extern uint32_t	 BMP_RGB565_getWidth    (uint8_t *);
extern uint32_t  BMP_RGB565_getHeight   (uint8_t *);
extern uint32_t  BMP_RGB565_getFileSize (uint8_t *);
extern uint32_t  BMP_RGB565_getImageSize(uint8_t *);
extern uint32_t  BMP_RGB565_getOffset   (uint8_t *);
extern void      BMP_RGB565_setPixelRGB (uint8_t *, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
extern void      BMP_RGB565_getPixelRGB (uint8_t *, uint32_t, uint32_t, uint8_t *, uint8_t *, uint8_t *);
extern void      BMP_RGB565_drawLineRGB (uint8_t *, int32_t, int32_t, int32_t, int32_t, uint8_t, uint8_t, uint8_t);
extern void      BMP_RGB565_drawRectRGB (uint8_t *, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
extern void      BMP_RGB565_fillRGB     (uint8_t *, uint8_t, uint8_t, uint8_t);
extern uint8_t * BMP_RGB565_copy(uint8_t *);
extern uint8_t * BMP_RGB565_resize_bicubic(uint8_t *, uint32_t, uint32_t);
extern int       BMP_RGB565_colorScale(float, float, float, uint8_t *, uint8_t *, uint8_t *);

#ifdef __cplusplus
}
#endif

#endif /* _BMP_RGB565_H_ */

/***************************************************************END OF FILE****/
