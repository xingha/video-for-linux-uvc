#ifndef _YUV_H_
#define _YUV_H_
extern "C"{
    #include "libswscale/swscale.h"
    #include "stdint.h"
    #include "stdlib.h"
}

void YUV420toYUV444(int width, int height, uint8_t* src, uint8_t* dst);
// int YUV420toRGB24_old(int width, int height, uint8_t* src, uint8_t* dst);
int YUV420toRGB24(int width, int height,const uint8_t *src, uint8_t *dest);
void ffmpeg_convert_yuv420_to_rgb24(int width, int height ,const uint8_t *src, uint8_t *dest, int bpp);
// int YUV420toRGB24_RGA(unsigned int src_fmt, uint8_t* src_buf,
//                       int src_w, int src_h,
//                       unsigned int dst_fmt, int dst_fd,
//                       int dst_w, int dst_h);

#endif
