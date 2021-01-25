/***************************************************************************
 *   Copyright (C) 2012 by Tobias Müller                                   *
 *   Tobias_Mueller@twam.info                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
	Convert from YUV420 format to YUV444.

	\param width width of image
	\param height height of image
	\param src source
	\param dst destination
*/

#include <yuv.hpp>

// #include <rga/RgaApi.h>
#define CLIP(color) (uint8_t)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

void YUV420toYUV444(int width, int height, uint8_t* src, uint8_t* dst) {
	int line, column;
	uint8_t *py, *pu, *pv;
	uint8_t *tmp = dst;

	// In this format each four bytes is two pixels. Each four bytes is two Y's, a Cb and a Cr.
	// Each Y goes to one of the pixels, and the Cb and Cr belong to both pixels.
	uint8_t *base_py = src;
	uint8_t *base_pu = src+(height*width);
	uint8_t *base_pv = src+(height*width)+(height*width)/4;

	for (line = 0; line < height; ++line) {
		for (column = 0; column < width; ++column) {
			py = base_py+(line*width)+column;
			pu = base_pu+(line/2*width/2)+column/2;
			pv = base_pv+(line/2*width/2)+column/2;

			*tmp++ = *py;
			*tmp++ = *pu;
			*tmp++ = *pv;
		}
	}
}

// int YUV420toRGB24_old(int width, int height, uint8_t* src, uint8_t* dst) {
//     if (width < 1 || height < 1 || src == NULL || dst == NULL)
//         return -1;
//     const long len = width * height;
//     uint8_t* yData = src;
//     uint8_t* vData = &yData[len];
//     uint8_t* uData = &vData[len >> 2];

//     int bgr[3];
//     int yIdx,uIdx,vIdx,idx;
//     for (int i = 0;i < height;i++){
//         for (int j = 0;j < width;j++){
//             yIdx = i * width + j;
//             vIdx = (i/2) * (width/2) + (j/2);
//             uIdx = vIdx;
//   /*  YUV420 转 BGR24
//             bgr[0] = (int)(yData[yIdx] + 1.732446 * (uData[vIdx] - 128)); // b分量
//             bgr[1] = (int)(yData[yIdx] - 0.698001 * (uData[uIdx] - 128) - 0.703125 * (vData[vIdx] - 128));// g分量
//             bgr[2] = (int)(yData[yIdx] + 1.370705 * (vData[uIdx] - 128)); // r分量
//  */
//  /*  YUV420 转 RGB24 注意如转换格式不对应会导致颜色失真*/
//             bgr[0] = (int)(yData[yIdx] + 1.370705 * (vData[uIdx] - 128)); // r分量
//             bgr[1] = (int)(yData[yIdx] - 0.698001 * (uData[uIdx] - 128) - 0.703125 * (vData[vIdx] - 128));// g分量
//             bgr[2] = (int)(yData[yIdx] + 1.732446 * (uData[vIdx] - 128)); // b分量

//             for (int k = 0;k < 3;k++){
//                 idx = (i * width + j) * 3 + k;
//                 if(bgr[k] >= 0 && bgr[k] <= 255)
//                     dst[idx] = bgr[k];
//                 else
//                     dst[idx] = (bgr[k] < 0)?0:255;
//             }
//         }
//     }
//     return 0;
// }


int YUV420toRGB24(int width, int height,const uint8_t *src, uint8_t *dest)
{
	int i, j;

	const uint8_t *ysrc = src;
	const uint8_t *usrc, *vsrc;


    vsrc = src + width * height;
    usrc = vsrc + (width * height) / 4;


	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j += 2) {
			int u1 = (((*usrc - 128) << 7) +  (*usrc - 128)) >> 6;
			int rg = (((*usrc - 128) << 1) +  (*usrc - 128) +
					((*vsrc - 128) << 2) + ((*vsrc - 128) << 1)) >> 3;
			int v1 = (((*vsrc - 128) << 1) +  (*vsrc - 128)) >> 1;

			*dest++ = CLIP(*ysrc + v1);
			*dest++ = CLIP(*ysrc - rg);
			*dest++ = CLIP(*ysrc + u1);
			ysrc++;

			*dest++ = CLIP(*ysrc + v1);
			*dest++ = CLIP(*ysrc - rg);
			*dest++ = CLIP(*ysrc + u1);

			ysrc++;
			usrc++;
			vsrc++;
		}
		/* Rewind u and v for next line */
		if (!(i&1)) {
			usrc -= width / 2;
			vsrc -= width / 2;
		}
	}
}

// int YUV420toRGB24_RGA(unsigned int src_fmt, uint8_t* src_buf,
//                       int src_w, int src_h,
//                       unsigned int dst_fmt, int dst_fd,
//                       int dst_w, int dst_h)
// {
//     int ret;
//     rga_info_t src;
//     rga_info_t dst;
//     int Format;

//     memset(&src, 0, sizeof(rga_info_t));
//     src.fd = -1; //rga_src_fd;
//     src.virAddr = src_buf;
//     src.mmuFlag = 1;

//     memset(&dst, 0, sizeof(rga_info_t));
//     dst.fd = dst_fd;
//     dst.mmuFlag = 1;


//     rga_set_rect(&src.rect, 0, 0, src_w, src_h, src_w, src_h, src_fmt);
//     rga_set_rect(&dst.rect, 0, 0, dst_w, dst_h, dst_w, dst_h, dst_fmt);
//     ret = c_RkRgaBlit(&src, &dst, NULL);
//     if (ret)
//         printf("c_RkRgaBlit0 error : %s\n", strerror(errno));
//     return ret;
// }

void ffmpeg_convert_yuv420_to_rgb24(int width, int height ,const uint8_t *src, uint8_t *dest, int bpp)
{
    const long len = width * height;
	const uint8_t *ysrc = src;
	const uint8_t *usrc, *vsrc;
    const int rgb_size = bpp * width * height / 8;

    vsrc = src + width * height;
    usrc = vsrc + (width * height) / 4;
    

    const uint8_t* in_buf[4]={ysrc,vsrc,usrc,0};

    int inlinesize[4] = {width, width/2, width/2, 0}; 
    uint8_t *prgb24 =(uint8_t *) malloc(rgb_size);
    uint8_t *rgb24[1] = { prgb24 };

    int rgb24_stride[1] = { (int)(bpp * width / 8 )};
    // Convert from YUV to RGB
    struct SwsContext *sws_ctx = NULL;
    sws_ctx =sws_getContext(width,height,AV_PIX_FMT_YUV420P,width,height, AV_PIX_FMT_BGR24 ,SWS_BILINEAR,NULL,NULL,NULL);

    sws_scale(sws_ctx, in_buf, inlinesize, 0, height, rgb24, rgb24_stride);

    memcpy(dest, rgb24[0], rgb_size);
    free(prgb24);
    prgb24=nullptr;
	sws_freeContext(sws_ctx);
}