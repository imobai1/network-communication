#ifndef _CELLTIMESTAMP_H_
#define _CELLTIMESTAMP_H_
// 包含高精度计时和时间间隔计算所需的头文件。
#include <chrono>
using namespace std::chrono;

class CELLTimetamp
{
public:
    // 构造函数 - 初始化对象并立即更新时间戳。
    CELLTimetamp() {
        update();
    }
    // 析构函数 - 不需要特殊清理工作，因为没有需要释放的资源。
    ~CELLTimetamp() {}

    // 函数用于更新时间戳到当前时间。
    void update() {
        // 使用高精度时钟记录当前时间，并将其存储在_begin变量中。
        _begin = high_resolution_clock::now();
    }

    /**
    *   函数用于获取已过去的秒数。
    *   @return 自上次更新以来已过去的秒数。
    */
    double getElapsedSecond() {
        // 将已过去的微秒数转换为秒，并返回。
        return getElapsedTimeInMicroSec() * 0.000001;      
    }

    /**
    *   函数用于获取已过去的毫秒数。
    *   @return 自上次更新以来已过去的毫秒数。
    */
    double getElapsedTimeInMilliSec() {
        // 将已过去的微秒数转换为毫秒，并返回。
        return this->getElapsedTimeInMicroSec() * 0.001;

    }

    /**
    *   函数用于获取已过去的微秒数。
    *   @return 自上次更新以来已过去的微秒数。
    */
    long long getElapsedTimeInMicroSec() {
        // 计算当前时间与存储的时间戳（_begin）之间的时长，并以微秒为单位返回。
        return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count(); 
    }

protected:
    // 存储对象构造时的初始时间点。
    time_point<high_resolution_clock> _begin;
    // high_resolution_clock是系统上可用的最小计时周期的时钟。
};

#endif // !_CELLTIMESTAMP_H_