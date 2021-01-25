
#include "mipi_camera_api.hpp"

static std::mutex mutex;
static void camera_callback(void *p, int w, int h, int *flag )
{
    uint8_t* srcbuf=nullptr;
    srcbuf = (uint8_t*)malloc(DST_W * DST_H * SRC_BPP / 8);
    mutex.lock();
    memcpy(srcbuf,(uint8_t *)p, DST_W * DST_H * SRC_BPP / 8);
    // YUV420toRGB24(w, h, (uint8_t *)p, rgbbuf);
    // ffmpeg_convert_yuv420_to_rgb24(w, h, srcbuf, rgbbuf, DST_BPP);
    memcpy(rgbbuf,srcbuf,DST_W * DST_H * SRC_BPP / 8);
    mutex.unlock();
    free(srcbuf);
    srcbuf=nullptr;
    // printf("image data:%d\n",rgbbuf[0]);
    // memcpy( g_mem_buf, rgbbuf, DST_W * DST_H * DST_BPP / 8);
}

static void *startCamera(void *args){
    mipi_camera::camera_run_params *run_params= (mipi_camera::camera_run_params *)args;
    rgbbuf = (uint8_t *)malloc(DST_W * DST_H * DST_BPP / 8);
    printf("Thread create ...\n");
    cameraRun(run_params->dev_name, run_params->src_w, run_params->src_h, run_params->src_fps, run_params->src_fmt, camera_callback,run_params->flag);
    printf("Thread Exit 0\n");
    pthread_exit(0);
}

int mipi_camera::mipi_camera_api::open(const std::string cam_device)
{
    struct stat st;

    g_mem_buf = (uint8_t *)malloc(DST_W * DST_H * DST_BPP / 8);
    run_params = (camera_run_params *)malloc(sizeof(camera_run_params));

    if (strcmp(cam_device.c_str(), "mipi") == 0)
        dev_name = get_device("rkisp");
    if (strcmp(cam_device.c_str(), "usb") == 0)
	    dev_name = get_device("uvc");
    if (!dev_name) {
        printf("do not get usb camera or mipi camera vide node.\n");
        return -1;
    }
    else
    {
        dev_name = get_device("uvc");
        // printf("%s\n",dev_name);
        memset(run_params->dev_name,0,sizeof(run_params->dev_name));
        strcpy(run_params->dev_name,dev_name);
        // run_params.dev_name = dev_name;
        run_params->src_w =SRC_W;
        run_params->src_h=SRC_H;
        run_params->src_fps=SRC_FPS;
        run_params->src_fmt=SRC_V4L2_FMT;
        run_params->flag = &g_run_flag;
        // printf("dev_name:%s\n",run_params->dev_name);
        // printf("flag:%d\n",*(run_params->flag));
        int ret = pthread_create(&camera_t,NULL,startCamera,(void *)run_params);

        // cameraRun(dev_name, SRC_W, SRC_H, SRC_FPS, SRC_V4L2_FMT, camera_callback, &g_run_flag);
        return 0;
    }
}


void mipi_camera::mipi_camera_api::read(cv::Mat &frame){
    while(true){
        // printf("1");
        if(rgbbuf!=nullptr){
            // printf("Has bufdata...\n");
            mutex.lock();
            memcpy(g_mem_buf, rgbbuf, DST_W * DST_H * DST_BPP / 8);
            frame = cv::Mat(DST_H, DST_W, CV_8UC3, g_mem_buf, 0);
            mutex.unlock();
            // std::cout << frame.cols<<"X"<<frame.rows<<" "<<frame.channels() << std::endl;
            break;
        }else{
            printf("---------- No bufdata...\n");
            continue;
        }
    }
}

void mipi_camera::mipi_camera_api::release(){
    if (camera_t) {
        g_run_flag = 0;
        pthread_detach(camera_t);
        printf("----------Thread finished\n");
        // camera_t = 0;
    }
    if(g_mem_buf){
        free(g_mem_buf);
    }
    g_mem_buf=nullptr;

    if(run_params){
        free(run_params);
    }
    run_params=nullptr;
}
