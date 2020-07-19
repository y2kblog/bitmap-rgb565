# Introduction
This library is a free and open source RGB565 format Bitmap library.
RGB565 format is suitable for embedded systems because it does not consume much memory.
You can create/delete bitmap images, get the color of a point, draw points/lines/rectangles, etc.
This library also includes color scale function for thermography and bicubic interpolation function.  
The library consists of only two files: `bmp_rgb565.c` and `bmp_rgb565.h`.

# Test
`test.c` is test program.  
After downloading this repository, you can run the test program by executing the following command.  
```
gcc -o program test.c bmp_rgb565.c && ./program
```
