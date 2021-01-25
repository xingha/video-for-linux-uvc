#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>
#include <stack>

using namespace std::chrono;

class Timer
{
public:
    std::stack<high_resolution_clock::time_point> tictoc_stack;

    void tic()
    {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        tictoc_stack.push(t1);
    }

    double toc(std::string msg = "", bool flag = true)
    {
        if (tictoc_stack.size() > 0)
        {
            double diff = duration_cast<milliseconds>(high_resolution_clock::now() - tictoc_stack.top()).count();
            if (msg.size() > 0)
            {
                if (flag)
                    printf("%s time elapsed: %f ms\n", msg.c_str(), diff);
            }

            tictoc_stack.pop();
            return diff;
        }
        else
        {
            return 0;
        }
    }
    void reset()
    {
        tictoc_stack = std::stack<high_resolution_clock::time_point>();
    }
    void gettime(char *currentTime)
    {
        time_t rawtime;
        struct tm *ptminfo;
        time(&rawtime);
        ptminfo = localtime(&rawtime);
        sprintf(currentTime, "%02d-%02d-%02d %02d:%02d:%02d\n",
                ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
                ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
    }
};
#endif