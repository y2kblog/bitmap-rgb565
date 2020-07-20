/* Include system header files -----------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Include user header files -------------------------------------------------*/
#include "bmp_rgb565.h"

/* Imported variables --------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#ifndef M_PI
#define M_PI 3.141592653
#endif

#ifndef RANGE
#define RANGE(x, min, max)	( (x < min) ? min : (x > max) ? max : x )
#endif

static const uint32_t FileHeaderSize  = 14; /* = 0x0E */
static const uint32_t InfoHeaderSize  = 40; /* = 0x28 */
static const uint32_t BitFieldSize    = 16;
static const uint32_t AllHeaderOffset = 70; /* FileHeaderSize + InfoHeaderSize + BitFieldSize */

/* Private types -------------------------------------------------------------*/
/* Private enum tag ----------------------------------------------------------*/
/* Private struct/union tag --------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static BMP_RGB565_Malloc_Function bmp_rgb565_malloc = malloc;
static BMP_RGB565_free_Function bmp_rgb565_free = free;

/* Exported function prototypes ----------------------------------------------*/
void	  BMP_RGB565_setAllocFunc(BMP_RGB565_Malloc_Function, BMP_RGB565_free_Function);
uint8_t * BMP_RGB565_create      (uint32_t, uint32_t);
void      BMP_RGB565_free        (uint8_t *);
uint32_t  BMP_RGB565_getWidth    (uint8_t *);
uint32_t  BMP_RGB565_getHeight   (uint8_t *);
uint32_t  BMP_RGB565_getFileSize (uint8_t *);
uint32_t  BMP_RGB565_getImageSize(uint8_t *);
uint32_t  BMP_RGB565_getOffset   (uint8_t *);
void      BMP_RGB565_setPixelRGB (uint8_t *, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
void      BMP_RGB565_getPixelRGB (uint8_t *, uint32_t, uint32_t, uint8_t *, uint8_t *, uint8_t *);
void      BMP_RGB565_drawLineRGB (uint8_t *, int32_t, int32_t, int32_t, int32_t, uint8_t, uint8_t, uint8_t);
void      BMP_RGB565_drawRectRGB (uint8_t *, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t);
void      BMP_RGB565_fillRGB     (uint8_t *, uint8_t, uint8_t, uint8_t);
uint8_t * BMP_RGB565_copy(uint8_t *);
uint8_t * BMP_RGB565_resize_bicubic(uint8_t *, uint32_t, uint32_t);
int 	  BMP_RGB565_colorScale(float, float, float, uint8_t *, uint8_t *, uint8_t *);

/* Private function prototypes -----------------------------------------------*/
static uint16_t convertRGBtoRGB565(uint8_t, uint8_t, uint8_t);
static uint32_t BMP_RGB565_read_uint32_t(uint8_t *);
static uint16_t BMP_RGB565_read_uint16_t(uint8_t *);
static void BMP_RGB565_write_uint32_t(uint32_t, uint8_t *);
static void BMP_RGB565_write_uint16_t(uint16_t, uint8_t *);
static uint32_t BMP_RGB565_getBytesPerRow(uint32_t);

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Set memory allocation funciton.
  * 		Call only once, before calling any other function from parson API.
  * 		If not called, malloc and free from stdlib will be used for all allocations.
  * @param  malloc_func memory allocate function.
  * @param  free_func memory free function.
  * @retval None
  */
void BMP_RGB565_setAllocFunc(BMP_RGB565_Malloc_Function malloc_func, BMP_RGB565_free_Function free_func)
{
	bmp_rgb565_malloc = malloc_func;
	bmp_rgb565_free = free_func;
}

/**
  * @brief  Create BMP RGB565 image.
  * @param  width width of image [pixel]
  * @param  height height of image [pixel]
  * @retval pointer to the created image. When error, return NULL
  */
uint8_t *BMP_RGB565_create(uint32_t width, uint32_t height)
{
    uint8_t *pbmp;
    uint32_t bytes_per_row = BMP_RGB565_getBytesPerRow(width);
    uint32_t image_size = bytes_per_row * height;
    uint32_t data_size = AllHeaderOffset + image_size;

    /* Allocate the bitmap data */
    pbmp = (uint8_t *)bmp_rgb565_malloc(sizeof(uint8_t) * data_size);
    if (pbmp == NULL)
        return NULL;
    for(uint32_t i = 0; i < data_size; i++)
        *(pbmp + i) = 0;

    // Set header's default values
    uint8_t *tmp = pbmp;
    *(tmp  +  0) = 'B';                                        // 'B' : Magic number
    *(tmp  +  1) = 'M';                                        // 'M' : Magic number
    BMP_RGB565_write_uint32_t(data_size        , tmp + 0x02);  // File Size
    BMP_RGB565_write_uint16_t(0                , tmp + 0x06);  // Reserved1
    BMP_RGB565_write_uint16_t(0                , tmp + 0x08);  // Reserved2
    BMP_RGB565_write_uint32_t(AllHeaderOffset  , tmp + 0x0A);  // Offset
    tmp += FileHeaderSize;    // Next

    // Info header
    BMP_RGB565_write_uint32_t( InfoHeaderSize + BitFieldSize, tmp + 0x00);   // HeaderSize
    BMP_RGB565_write_uint32_t( width           , tmp + 0x04);  // width  (*** Signed value ***)
    BMP_RGB565_write_uint32_t( height          , tmp + 0x08);  // height (*** Signed value ***)
    BMP_RGB565_write_uint16_t( 1               , tmp + 0x0C);  // planes
    BMP_RGB565_write_uint16_t( 16              , tmp + 0x0E);  // Bit count
    BMP_RGB565_write_uint32_t( 3               , tmp + 0x10);  // Bit compression
    BMP_RGB565_write_uint32_t( image_size      , tmp + 0x14);  // Image size
    BMP_RGB565_write_uint32_t( 0               , tmp + 0x18);  // X pixels per meter
    BMP_RGB565_write_uint32_t( 0               , tmp + 0x1C);  // Y pixels per meter
    BMP_RGB565_write_uint32_t( 0               , tmp + 0x20);  // Color index
    BMP_RGB565_write_uint32_t( 0               , tmp + 0x24);  // Important index
    tmp += InfoHeaderSize;    // Next

    // Bit field
    BMP_RGB565_write_uint32_t( 0x0000F800      , tmp + 0x00);  // red
    BMP_RGB565_write_uint32_t( 0x000007E0      , tmp + 0x04);  // green
    BMP_RGB565_write_uint32_t( 0x0000001F      , tmp + 0x08);  // blue
    BMP_RGB565_write_uint32_t( 0x00000000      , tmp + 0x0C);  // reserved

    return pbmp;
}

/**
  * @brief  Free BMP RGB565 image.
  * @param  pbmp pointer to a image
  * @retval None
  */
void BMP_RGB565_free(uint8_t *pbmp)
{
	bmp_rgb565_free(pbmp);
}


/**
  * @brief  Get width in pixel of a image.
  * @param  pbmp pointer to a image
  * @retval width [pixel]
  */
uint32_t BMP_RGB565_getWidth(uint8_t *pbmp)
{
    return BMP_RGB565_read_uint32_t(pbmp + FileHeaderSize + 0x04);
}

/**
  * @brief  Get height in pixel of a image.
  * @param  pbmp pointer to a image
  * @retval height [pixel]
  */
uint32_t BMP_RGB565_getHeight(uint8_t *pbmp)
{
    return BMP_RGB565_read_uint32_t(pbmp + FileHeaderSize + 0x08);
}

/**
  * @brief  Get file size of a image.
  * @param  pbmp pointer to a image
  * @retval file size [byte]
  */
uint32_t BMP_RGB565_getFileSize(uint8_t *pbmp)
{
    return BMP_RGB565_read_uint32_t(pbmp + 0x02);
}

/**
  * @brief  Get image size of a image.
  * @param  pbmp pointer to a image
  * @retval Image size [byte]
  */
uint32_t BMP_RGB565_getImageSize(uint8_t *pbmp)
{
    return BMP_RGB565_read_uint32_t(pbmp + FileHeaderSize + 0x14);
}

/**
  * @brief  Get header offset size of a image.
  * @param  pbmp pointer to a image
  * @retval Header offset size [byte]
  */
uint32_t BMP_RGB565_getOffset(uint8_t *pbmp)
{
    return BMP_RGB565_read_uint32_t(pbmp + 0x0A);
}

/**
  * @brief  Draw a color in RGB format on a specified pixel.
  * @param  pbmp pointer to a image
  * @param  x	x of a image(Range:[0,width-1] ) [pixel]
  * @param  y	y of a image(Range:[0,height-1]) [pixel]
  * @param  r	Red   value [0, 255] (Lower 3 bits are ignored)
  * @param  g	Green value [0, 255] (Lower 2 bits are ignored)
  * @param  b	Blue  value [0, 255] (Lower 3 bits are ignored)
  * @retval None
  */
void BMP_RGB565_setPixelRGB(uint8_t *pbmp, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width  = BMP_RGB565_getWidth (pbmp);
    uint32_t height = BMP_RGB565_getHeight (pbmp);

    if(pbmp == NULL || x >= width || y >= height)
        return;

    uint32_t bytes_per_row = BMP_RGB565_getBytesPerRow(width);
    uint16_t col = convertRGBtoRGB565(r, g, b);

    BMP_RGB565_write_uint16_t( col, pbmp + AllHeaderOffset + bytes_per_row * (height - y - 1) + (x << 1));
}

/**
  * @brief  Get a color in RGB format on a specified pixel.
  * @param  pbmp pointer to a image
  * @param  x	x of a image(Range:[0,width-1] ) [pixel]
  * @param  y	y of a image(Range:[0,height-1]) [pixel]
  * @param  r	Pointer to a red   value [0, 255] (Lower 3 bits are 0)
  * @param  g	Pointer to a green value [0, 255] (Lower 2 bits are 0)
  * @param  b	Pointer to a blue  value [0, 255] (Lower 3 bits are 0)
  * @retval None
  */
void BMP_RGB565_getPixelRGB(uint8_t *pbmp, uint32_t x, uint32_t y, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint32_t width  = BMP_RGB565_getWidth(pbmp);
    uint32_t height = BMP_RGB565_getHeight(pbmp);

    if(pbmp == NULL || x >= width || y >= height)
        return;

    uint32_t bytes_per_row = BMP_RGB565_getBytesPerRow(width);
    uint16_t col = BMP_RGB565_read_uint16_t(pbmp + AllHeaderOffset + bytes_per_row * (height - y - 1) + (x << 1));
    *r = (uint8_t)(col >> 11) << 3; if(*r == 0xF8) *r = 0xFF;
    *g = (uint8_t)(col >>  5) << 2; if(*g == 0xFC) *g = 0xFF;
    *b = (uint8_t) col        << 3; if(*b == 0xF8) *b = 0xFF;
}


/**
  * @brief  Draws a straight line in a specified RGB color.
  * @param  pbmp pointer to a image
  * @param  x0	Start x position of a line(Range:[0,width-1] ) [pixel]
  * @param  y0  Start y position of a line(Range:[0,width-1] ) [pixel]
  * @param  x1	End   x position of a line(Range:[0,width-1] ) [pixel]
  * @param  y1  End   y position of a line(Range:[0,width-1] ) [pixel]
  * @param  r	Red   value [0, 255] (Lower 3 bits are ignored)
  * @param  g	Green value [0, 255] (Lower 2 bits are ignored)
  * @param  b	Blue  value [0, 255] (Lower 3 bits are ignored)
  * @retval None
  * @detail Bresenham's line algorithm
  *         ref : https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
  *         ref : https://ja.wikipedia.org/wiki/%E3%83%96%E3%83%AC%E3%82%BC%E3%83%B3%E3%83%8F%E3%83%A0%E3%81%AE%E3%82%A2%E3%83%AB%E3%82%B4%E3%83%AA%E3%82%BA%E3%83%A0#.E6.9C.80.E9.81.A9.E5.8C.96
  */
void BMP_RGB565_drawLineRGB(uint8_t *pbmp,
		int32_t x0, int32_t y0, int32_t x1, int32_t y1,
        uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width  = BMP_RGB565_getWidth (pbmp);
    uint32_t height = BMP_RGB565_getHeight (pbmp);

    if(pbmp == NULL || x0 < 0 || x0 >= width || x1 < 0 || x1 >= width || y0 < 0 || y0 >= height || y1 < 0 || y1 >= height)
        return;

    uint32_t bytes_per_row = BMP_RGB565_getBytesPerRow(width);
    uint16_t col = convertRGBtoRGB565(r, g, b);

    int32_t dx = x1 - x0 > 0 ? x1 - x0 : x0 - x1;
    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t dy = y1 - y0 > 0 ? y1 - y0 : y0 - y1;
    int32_t sy = y0 < y1 ? 1 : -1;
    int32_t err = dx - dy;
    int32_t e2;

    uint8_t *pbmp_data = pbmp + AllHeaderOffset;

    for (;;)
    {
        BMP_RGB565_write_uint16_t( col, pbmp_data + bytes_per_row * (height - y0 - 1) + x0 * 2);

        if (x0 == x1 && y0 == y1)
            break;

        e2 = 2*err;
        if (e2 > -dy) {err -= dy;   x0 += sx;}
        if (e2 <  dx) {err += dx;   y0 += sy;}
    }
}


/**
  * @brief  Draws a Rectangle in a specified RGB color.
  * @param  pbmp pointer to a image
  * @param  x0	Start x position of a line(Range:[0,width-1] ) [pixel]
  * @param  y0  Start y position of a line(Range:[0,width-1] ) [pixel]
  * @param  x1	End   x position of a line(Range:[0,width-1] ) [pixel]
  * @param  y1  End   y position of a line(Range:[0,width-1] ) [pixel]
  * @param  r	Red   value [0, 255] (Lower 3 bits are ignored)
  * @param  g	Green value [0, 255] (Lower 2 bits are ignored)
  * @param  b	Blue  value [0, 255] (Lower 3 bits are ignored)
  * @retval None
  */
void BMP_RGB565_drawRectRGB(uint8_t *pbmp,
		uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
        uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t width = BMP_RGB565_getWidth(pbmp);
    uint32_t height = BMP_RGB565_getHeight(pbmp);

    if (pbmp == NULL || x0 >= width || x1 >= width || y0 >= height || y1 >= height)
        return;

    uint32_t swap;
    if(x0 > x1)
    {
        swap = x0;
        x0 = x1;
        x1 = swap;
    }
    if (y0 > y1)
    {
        swap = y0;
        x0 = y1;
        y1 = swap;
    }

    uint32_t bytes_per_row = BMP_RGB565_getBytesPerRow(width);
    uint16_t col = convertRGBtoRGB565(r, g, b);

    uint8_t *pbmp_data = pbmp + AllHeaderOffset;

    int32_t y_addr_start = bytes_per_row*(height - y0 - 1);
    int32_t y_addr_end   = bytes_per_row*(height - y1 - 1);
    int32_t x_addr_start = x0 * 2;
    int32_t x_addr_end   = x1 * 2;
    for(int32_t y_addr = y_addr_start; y_addr >= y_addr_end; y_addr -= bytes_per_row)
    {
        for(int32_t x_addr = x_addr_start; x_addr <= x_addr_end; x_addr += 2)
            BMP_RGB565_write_uint16_t( col, pbmp_data + y_addr + x_addr);
    }
}

/**
  * @brief  Fill image in a specified RGB color.
  * @param  pbmp pointer to a image
  * @param  r	Red   value [0, 255] (Lower 3 bits are ignored)
  * @param  g	Green value [0, 255] (Lower 2 bits are ignored)
  * @param  b	Blue  value [0, 255] (Lower 3 bits are ignored)
  * @retval None
  */
void BMP_RGB565_fillRGB(uint8_t *pbmp, uint8_t r, uint8_t g, uint8_t b)
{
    if (pbmp == NULL)
        return;

    BMP_RGB565_drawRectRGB(pbmp, 0, 0, BMP_RGB565_getWidth(pbmp)-1, BMP_RGB565_getHeight(pbmp)-1, r, g, b);
}


/**
  * @brief  Copy image.
  * @param  pbmp pointer to a source image
  * @retval pointer to the copied image. When error, return NULL
  */
uint8_t *BMP_RGB565_copy(uint8_t *pbmp)
{
    uint8_t *pbmpDst = BMP_RGB565_create(BMP_RGB565_getWidth(pbmp), BMP_RGB565_getHeight(pbmp));
    if(pbmpDst == NULL)
        return NULL;
    memcpy(pbmpDst, pbmp, BMP_RGB565_getFileSize(pbmp));
    return pbmpDst;
}


/**
  * @brief  Bicubic Interpolation.
  * @param  pbmpSrc pointer to a source image
  * @param  width width of interpolated image [pixel]
  * @param  height height of interpolated image [pixel]
  * @retval pointer to the created image. When error, return NULL.
  * @detail ref: http://docs-hoffmann.de/bicubic03042002.pdf (p.8)
  */
uint8_t *BMP_RGB565_resize_bicubic(uint8_t *pbmpSrc, uint32_t width, uint32_t height)
{
	uint8_t *pbmpDst;
	int src_x_size, src_y_size, dst_x_size, dst_y_size;
	float C[4][3];
	float d0[3], d2[3], d3[3], a0[3], a1, a2, a3;
	int x, y;
	float dx, dy;
	float tx, ty;
    uint8_t rgb1[3], rgb2[3], rgb_out[3];

	pbmpDst = BMP_RGB565_create(width, height);
	if(pbmpDst == NULL)
		return NULL;

    for (int i = 0; i < 4; i++)
    for (int j = 0; j < 3; j++)
        C[i][j] = 0.0f;

	src_x_size = BMP_RGB565_getWidth (pbmpSrc);
	src_y_size = BMP_RGB565_getHeight(pbmpSrc);
	dst_x_size = width;
	dst_y_size = height;

	tx = (float)src_x_size / dst_x_size;
	ty = (float)src_y_size / dst_y_size;

	for (int dstCol = 0; dstCol < dst_y_size; dstCol++)
	{
		for (int dstRow = 0; dstRow < dst_x_size; dstRow++)
		{
			x = (int) (tx * dstRow);
			y = (int) (ty * dstCol);

			dx = tx * dstRow - x;
			dy = ty * dstCol - y;

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j <= 3; j++)
				{
					BMP_RGB565_getPixelRGB(pbmpSrc, RANGE((x - 1) + i, 0, src_x_size-1), RANGE(y - 1 + j, 0, src_y_size-1), &rgb1[0], &rgb1[1], &rgb1[2]);
					BMP_RGB565_getPixelRGB(pbmpSrc, RANGE((x)     + i, 0, src_x_size-1), RANGE(y - 1 + j, 0, src_y_size-1), &rgb2[0], &rgb2[1], &rgb2[2]);
					for(int rgb_i = 0; rgb_i < 3; rgb_i++)
						d0[rgb_i] = rgb1[rgb_i] - rgb2[rgb_i];

					BMP_RGB565_getPixelRGB(pbmpSrc, RANGE((x + 1) + i, 0, src_x_size-1), RANGE(y - 1 + j, 0, src_y_size-1), &rgb1[0], &rgb1[1], &rgb1[2]);
					BMP_RGB565_getPixelRGB(pbmpSrc, RANGE((x)     + i, 0, src_x_size-1), RANGE(y - 1 + j, 0, src_y_size-1), &rgb2[0], &rgb2[1], &rgb2[2]);
					for(int rgb_i = 0; rgb_i < 3; rgb_i++)
						d2[rgb_i] = rgb1[rgb_i] - rgb2[rgb_i];

					BMP_RGB565_getPixelRGB(pbmpSrc, RANGE((x + 2) + i, 0, src_x_size-1), RANGE(y - 1 + j, 0, src_y_size-1), &rgb1[0], &rgb1[1], &rgb1[2]);
					BMP_RGB565_getPixelRGB(pbmpSrc, RANGE((x)     + i, 0, src_x_size-1), RANGE(y - 1 + j, 0, src_y_size-1), &rgb2[0], &rgb2[1], &rgb2[2]);
					for(int rgb_i = 0; rgb_i < 3; rgb_i++)
						d3[rgb_i] = rgb1[rgb_i] - rgb2[rgb_i];

					BMP_RGB565_getPixelRGB(pbmpSrc, RANGE((x)     + i, 0, src_x_size-1), RANGE(y - 1 + j, 0, src_y_size-1), &rgb1[0], &rgb1[1], &rgb1[2]);
					for(int rgb_i = 0; rgb_i < 3; rgb_i++)
						a0[rgb_i] = rgb1[rgb_i];

					for(int rgb_i = 0; rgb_i < 3; rgb_i++)
					{
						a1 = -1.0f / 3 * d0[rgb_i] +            d2[rgb_i] - 1.0f / 6 * d3[rgb_i];
						a2 =  1.0f / 2 * d0[rgb_i] + 1.0f / 2 * d2[rgb_i];
						a3 = -1.0f / 6 * d0[rgb_i] - 1.0f / 2 * d2[rgb_i] + 1.0f / 6 * d3[rgb_i];
						C[j][rgb_i] = a0[rgb_i] + a1 * dx + a2 * dx * dx + a3 * dx * dx * dx;

						float d0_2, d2_2, d3_2, a0_2;
						d0_2 = C[0][rgb_i] - C[1][rgb_i];
						d2_2 = C[2][rgb_i] - C[1][rgb_i];
						d3_2 = C[3][rgb_i] - C[1][rgb_i];
						a0_2 = C[1][rgb_i];
						a1 = -1.0f / 3 * d0_2 +            d2_2 - 1.0f / 6 * d3_2;
						a2 =  1.0f / 2 * d0_2 + 1.0f / 2 * d2_2;
						a3 = -1.0f / 6 * d0_2 - 1.0f / 2 * d2_2 + 1.0f / 6 * d3_2;

						rgb_out[rgb_i] = RANGE( (int)(a0_2 + a1 * dy + a2 * dy * dy + a3 * dy * dy * dy + 0.5f), 0, 255);
					}
					BMP_RGB565_setPixelRGB(pbmpDst, dstRow + i, dstCol,  rgb_out[0], rgb_out[1], rgb_out[2]);
				}
			}
		}
	}
	return pbmpDst;
}


///// Support function
/**
  * @brief  Color scale function for thermography.
  * @param  val    Value
  * @param  maxVal Maximum value
  * @param  minVal Minimum value
  * @param  r	   Pointer to a red   value [0, 255]
  * @param  g	   Pointer to a green value [0, 255]
  * @param  b	   Pointer to a blue  value [0, 255]
  * @retval status (0: Success, otherwise: Failure)
  */
int BMP_RGB565_colorScale(float val, float maxVal, float minVal, uint8_t *r, uint8_t *g, uint8_t *b)
{
    float ratio;	// 0 ~ 1
    if (maxVal > minVal)
    	ratio = (val - minVal) / (maxVal - minVal);
    else {
        //printf("Error: maxVal <= minVal\n");
        return -1;
    }

    // with Black/White
    int32_t ratio_buf = 6.0f * ratio;
    uint8_t col_val = 127.5f * (cosf(6.0f * M_PI * ratio) + 1.0f);	// 127.5 means 255/2
         if (ratio_buf >= 6) {*r = 255;           *g = 255;           *b = 255;         }   // White
    else if (ratio_buf >= 5) {*r = 255;           *g = col_val;       *b = col_val;     }   // Red - White
    else if (ratio_buf >= 4) {*r = 255;           *g = col_val;       *b = 0;           }   // Yellow - Red
    else if (ratio_buf >= 3) {*r = col_val;       *g = 255;           *b = 0;           }   // Green - Yellow
    else if (ratio_buf >= 2) {*r = 0;             *g = 255;           *b = col_val;     }   // Sky - Green
    else if (ratio_buf >= 1) {*r = 0;             *g = col_val;       *b = 255;         }   // Blue - Sky
    else if (ratio_buf >= 0) {*r = 0;             *g = 0;             *b = 255-col_val; }   // Black - Blue
    else                     {*r = 0;             *g = 0;             *b = 0;           }   // Black

    /*
    // without Black/White
    int32_t ratio_buf = 4.0f * ratio;
    uint8_t col_val = 127.5f * (-cosf(4.0f * M_PI * ratio) + 1.0f);
         if (ratio_buf >= 4){r = 255;     g = 0;       b = 0;       }   // Red
    else if (ratio_buf >= 3){r = 255;     g = col_val; b = 0;       }   // Yellow - Red
    else if (ratio_buf >= 2){r = col_val; g = 255;     b = 0;       }   // Green - Yellow
    else if (ratio_buf >= 1){r = 0;       g = 255;     b = col_val; }   // Sky - Green
    else if (ratio_buf >= 0){r = 0;       g = col_val; b = 255;     }   // Blue - Sky
    else                    {r = 0;       g = 0;       b = 255;     }   // Blue
    */

	return 0;
}


/* Private functions ---------------------------------------------------------*/
static uint16_t convertRGBtoRGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return  ((uint16_t) (r >> 3) << 11)
          | ((uint16_t) (g >> 2) << 5)
          |  (uint16_t) (b >> 3);
}

// Calculate the number of bytes used to store a single image row.
// This is always rounded up to the next multiple of 4.
static uint32_t BMP_RGB565_getBytesPerRow(uint32_t width)
{
    if (width & 0x00000001)
        return ((width + 1) << 1);
    else
        return  (width << 1);
}


/**************************************************************
    Reads a little-endian unsigned int from the file.
    Returns non-zero on success.
**************************************************************/
static uint32_t BMP_RGB565_read_uint32_t(uint8_t *pSrc)
{
    uint32_t retval = 0x00000000;
    retval |= (uint32_t)*(pSrc + 3) << 24;
    retval |= (uint32_t)*(pSrc + 2) << 16;
    retval |= (uint32_t)*(pSrc + 1) <<  8;
    retval |= (uint32_t)*(pSrc    );
    return retval;
}

/**************************************************************
    Reads a little-endian unsigned int from the file.
    Returns non-zero on success.
**************************************************************/
static uint16_t BMP_RGB565_read_uint16_t(uint8_t *pSrc)
{
    uint16_t retval = 0x0000;
    retval |= (uint16_t)*(pSrc + 1) <<  8;
    retval |= (uint16_t)* pSrc;
    return retval;
}

/**************************************************************
    Writes a little-endian unsigned short int to the file.
    Returns non-zero on success.
**************************************************************/
static void BMP_RGB565_write_uint32_t(uint32_t Src, uint8_t *pDst)
{
    *(pDst + 3) = (uint8_t)( ( Src & 0xff000000 ) >> 24 );
    *(pDst + 2) = (uint8_t)( ( Src & 0x00ff0000 ) >> 16 );
    *(pDst + 1) = (uint8_t)( ( Src & 0x0000ff00 ) >> 8  );
    *pDst       = (uint8_t)  ( Src & 0x000000ff )        ;
}

/**************************************************************
	Writes a little-endian unsigned short int to the file.
	Returns non-zero on success.
**************************************************************/
static void BMP_RGB565_write_uint16_t(uint16_t Src, uint8_t *pDst)
{
    *(pDst + 1) = (uint8_t)( ( Src & 0xff00 ) >> 8 );
	*pDst       = (uint8_t)  ( Src & 0x00ff )       ;
}

/***************************************************************END OF FILE****/
