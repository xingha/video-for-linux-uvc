#include "mipi_camera_api.hpp"


int main(int argc,char*argv[])
{
    mipi_camera::mipi_camera_api cap;
    cv::Mat frame;
    cv::namedWindow("frame",cv::WINDOW_FREERATIO);
    cv::resizeWindow("frame",cv::Size(680, 480));
    int fd = cap.open();
    int fcount=0;
    while(true)
    {
        cap.read(frame);
        fcount++;
        std::cout<<"["<<fcount<<"]"<<"image size"<<frame.size()<<std::endl;
        cv::imshow("frame", frame);
        if(cv::waitKey(10)==27)break;
    }
    cap.release();
}
