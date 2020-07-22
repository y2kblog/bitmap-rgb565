#include <stdio.h>
#include <stdlib.h>
#include "bmp_rgb565.h"

int main(void)
{
  FILE *fp;
  size_t writtenSize;
  uint8_t *pbmp, *pbmp_resize;

  // Set allocate functions
  BMP_RGB565_setAllocFunc(malloc, free);

  // Create image (100pixel x 100pixel)
  pbmp = BMP_RGB565_create(100, 100);
  if(pbmp == NULL) {
    printf("Failed to create image\n");
    return -1;
  }

  // Fill image yellow
  BMP_RGB565_fillRGB(pbmp, COL_RGB_SET(0xFFFF00));

  // Draw Red line (20,50) - (70,60)
  BMP_RGB565_drawLineRGB(pbmp, 20, 50, 70, 60, COL_RGB_SET(0xFF0000));

  // Draw green rectangle (10,20) - (30,30)
  BMP_RGB565_drawRectRGB(pbmp, 10, 20, 30, 30, COL_RGB_SET(0x00FF00));

  // Draw text
  BMP_RGB565_drawTextRGB(pbmp, "ABC", BMP_RGB565_FONT_6X10, 10, 80, COL_RGB_SET(0x0000FF));

  // Save image
  fp = fopen("output.bmp", "wb");
  if (fp == NULL) {
      printf("Failed to open file\n");
      return -1;
  }
  writtenSize = fwrite((uint8_t *)pbmp, sizeof(uint8_t), BMP_RGB565_getFileSize(pbmp), fp); 
  if( (writtenSize * sizeof(uint8_t)) != BMP_RGB565_getFileSize(pbmp)) {
    printf("Failed to write image\n");
    return -1;
  }
  fclose(fp);

  // Resize image (200pixel x 70pixel)
  pbmp_resize = BMP_RGB565_resize_bicubic(pbmp, 200, 70);
  if(pbmp_resize == NULL) {
    printf("Failed to create resize image\n");
    return -1;
  }

  // Save resize image
  fp = fopen("output_resize.bmp", "wb");
  if (fp == NULL) {
      printf("Failed to open file\n");
      return -1;
  }
  writtenSize = fwrite((uint8_t *)pbmp_resize, sizeof(uint8_t), BMP_RGB565_getFileSize(pbmp_resize), fp); 
  if( (writtenSize * sizeof(uint8_t)) != BMP_RGB565_getFileSize(pbmp_resize)) {
    printf("Failed to write image\n");
    return -1;
  }
  fclose(fp);

  BMP_RGB565_free(pbmp);
  BMP_RGB565_free(pbmp_resize);
  return 0;
}
