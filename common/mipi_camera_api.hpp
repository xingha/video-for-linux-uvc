#ifndef _MIPI_CAMERA_API_H_
#define  _MIPI_CAMERA_API_H_

extern "C" {
    #include <errno.h>
    #include <fcntl.h>
    #include <math.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <getopt.h> /* getopt_long() */
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <pthread.h>

    #include <stdio.h>
    #include <stdint.h>
    #include <linux/videodev2.h>
    
}


#include <mutex>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "yuv.hpp"
#include "v4l2camera.hpp"
#include "device_name.hpp"
#include "timer.hpp"


#define SRC_W         1920
#define SRC_H         1080
#define SRC_FPS       30
#define SRC_BPP       24
#define DST_W         1920
#define DST_H         1080
#define DST_BPP       24

// #define SRC_V4L2_FMT  V4L2_PIX_FMT_YUV420
#define SRC_V4L2_FMT  V4L2_PIX_FMT_BGR24
// #define SRC_RKRGA_FMT RK_FORMAT_YCbCr_420_P
// #define DST_RKRGA_FMT RK_FORMAT_RGB_888

static uint8_t* rgbbuf=nullptr;
static Timer frame_interval;
namespace mipi_camera
{
    struct camera_run_params
    {
        /* data */
        char dev_name[20];
        int src_w=0; 
        int src_h=0;
        int src_fps=0;
        int src_fmt=0;
        int *flag=0;
    };
    
    class mipi_camera_api
    {
    private:
        /* data */
        // const char cam_device[10] = "usb";
        int g_run_flag = 1;
        char *dev_name;
        camera_run_params *run_params=nullptr;
        // int fcount=0;
        pthread_t camera_t=0;

    public:
        int open(const std::string cam_device="usb");// initial devices
        void read(cv::Mat &frame);
        void release();
        uint8_t *g_mem_buf=nullptr;
        mipi_camera_api(/* args */)    
        {
            frame_interval.tic();
        };
        ~mipi_camera_api(){
            // mipi_camera_api::release();
            if(rgbbuf){
                free(rgbbuf);
            }
            rgbbuf=nullptr;
        };
    };
    
}

#endif // !_MIPI_CAMERA_API_H_
