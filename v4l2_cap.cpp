#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <linux/videodev2.h>
// #include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#define VIDEO0 "/dev/video0"


int show_img(void* addr, int lenght){
    FILE* fp2 = fopen("demo.jpg","wb");
    fwrite(addr,lenght,1,fp2);
    fclose(fp2);
    printf("pass");
    return 0;
}

int mainloop()
{
    // 1.打开摄像头
    int fd = open(VIDEO0, O_RDWR);
    if (!fd)
    {
        perror("error: not found camera.");
        return -1;
    };

    // 2.查看摄像头的信息
    struct v4l2_capability cap;
    if (-1 == (ioctl(fd, VIDIOC_QUERYCAP, &cap)))
    {
        perror("error: capability failed.");
        return -1;
    }
    printf("Driver Name: %s\nCard Name: %s\nBus info: %s\nDriver Version: %u.%u.%u\n",
           cap.driver, cap.card, cap.bus_info, (cap.version >> 16) & 0XFF,
           (cap.version >> 8) & 0XFF, cap.version & 0XFF);

    // 3.查看摄像头支持的视频流格式
    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("Support format:\n");
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
    {
        printf("\t%d, %s\n", fmtdesc.index, fmtdesc.description);
        fmtdesc.index++;
    }

    // 4.设置视频帧的格式, 查看是否支持bgr24，查看当前视频帧信息；
    struct v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd,VIDIOC_G_FMT,&fmt);
    printf("Oringe Frame Format:\n\tfwidth:%d\n\tfheight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
    if(ioctl(fd,VIDIOC_TRY_FMT,&fmt)==-1)
    {
        if(errno==EINVAL){
            printf("NOT support rgb24 format.\n");
            return -1;
        }
    }
    
    // v4l2_cropcap 结构体设置摄像头捕捉区域，v4l2_format 结构体用来设置摄像头的视频制式、帧格式等；
    // struct v4l2_cropcap crop_f;
    // crop_f.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // crop_f.bounds.left = 10;
    // crop_f.bounds.top = 10;
    // crop_f.bounds.width = 1900;
    // crop_f.bounds.height = 1000;
    // if(ioctl(fd, VIDIOC_CROPCAP,&crop_f)==-1)
    //     printf("NOT crop frame");
    fmt.fmt.pix.width = 1920;
    fmt.fmt.pix.height = 1080;
    ioctl(fd,VIDIOC_S_FMT,&fmt);  // 设置帧格式
    ioctl(fd,VIDIOC_G_FMT,&fmt);  // 查看帧格式
    printf("Current Frame Format:\n\tfwidth:%d\n\tfheight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);


    // 5.申请视频帧缓冲区,申请一个可缓冲4个缓冲帧的缓冲区
    struct v4l2_requestbuffers req_buff;
    req_buff.count = 1;
    req_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req_buff.memory = V4L2_MEMORY_MMAP;
    if(ioctl(fd,VIDIOC_REQBUFS,&req_buff)==-1)
    {
        perror("error: 申请缓存失败");
        return -1;
    }
    else{
        printf("申请缓存成功\n");
    }

    // 5.管理视频帧缓冲区，映射缓冲帧的地址，长度
    struct buff{
        void* start;
        unsigned int lenght;
    }*buffer;  //用于映射和获取缓冲帧的地址和长度；
    buffer= (buff*)calloc(req_buff.count, sizeof (*buffer)); //分配内存大小；
    if(!buffer) {
        printf("Out of memory/n");
        return -1;
    }
    for(unsigned int number_buf;number_buf<req_buff.count;++number_buf)
    {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.memory = V4L2_MEMORY_MMAP;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.index = number_buf;
        if(ioctl(fd,VIDIOC_QUERYBUF,&buf)==-1)  //获取缓冲帧地址，长度
        return(-1);
        buffer[number_buf].lenght = buf.length;  //映射长度
        //映射缓冲帧内存地址
        buffer[number_buf].start=mmap (NULL,buf.length,PROT_READ | PROT_WRITE ,MAP_SHARED,fd, buf.m.offset);
        if(MAP_FAILED==buffer[number_buf].start)
        return -1;
    }

    // 6.循环读取帧数据,启动读取，关闭读取；
    enum v4l2_buf_type type;
    for(unsigned int nu=0;nu<4;++nu)
    {
        struct v4l2_buffer buf;  //这个buf和第5步中的buf没关系，这是为什么？
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = nu;
        ioctl(fd,VIDIOC_QBUF,&buf);
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  //这个是用来干什么
    if(ioctl(fd,VIDIOC_STREAMON,&type)>=0)
        printf("启动读取数据\n");

    // 7.显示帧数据,保存帧
    // int f_c = 10;
    // while(f_c)
    // {
    struct v4l2_buffer mbuff;
    memset(&mbuff,0,sizeof(mbuff));
    mbuff.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mbuff.memory=V4L2_MEMORY_MMAP;
    ioctl(fd,VIDIOC_DQBUF,&mbuff);
    show_img(buffer[mbuff.index].start, buffer[mbuff.index].lenght);
    // f_c--;
    // usleep(1000);
    // }

    // 8.关闭设备，释放内存
    if(ioctl(fd,VIDIOC_STREAMOFF,&type)>=0)
        printf("停止读取流\n");
    for(int i=0;i<4;++i)
    {
        munmap(buffer[i].start, buffer[i].lenght);
    }
    printf("munmap buffer\n");
    free(buffer);
    printf("free buffer\n");
    close(fd);
    printf("close file.");
    return -1;
}

int main(int argc, char *argv[]){
    int frame_num=0;
    cv::namedWindow("frame",cv::WINDOW_FREERATIO);
    cv::resizeWindow("frame", cv::Size(640,480));
    while(true)
    {
        mainloop();
        cv::Mat img = cv::imread("demo.jpg");
        // cv::Mat img;
        // img = cv::Mat(1920, 1080, CV_8UC3, addr, 0);
        cv::imshow("frame", img);
        if(cv::waitKey(10)==27){
            break;
        }
    }
}
