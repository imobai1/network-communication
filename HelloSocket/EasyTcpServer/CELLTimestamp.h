#ifndef _CELLTIMESTAMP_H_
#define _CELLTIMESTAMP_H_
// �����߾��ȼ�ʱ��ʱ�������������ͷ�ļ���
#include <chrono>
using namespace std::chrono;

class CELLTimetamp
{
public:
    // ���캯�� - ��ʼ��������������ʱ�����
    CELLTimetamp() {
        update();
    }
    // �������� - ����Ҫ��������������Ϊû����Ҫ�ͷŵ���Դ��
    ~CELLTimetamp() {}

    // �������ڸ���ʱ�������ǰʱ�䡣
    void update() {
        // ʹ�ø߾���ʱ�Ӽ�¼��ǰʱ�䣬������洢��_begin�����С�
        _begin = high_resolution_clock::now();
    }

    /**
    *   �������ڻ�ȡ�ѹ�ȥ��������
    *   @return ���ϴθ��������ѹ�ȥ��������
    */
    double getElapsedSecond() {
        // ���ѹ�ȥ��΢����ת��Ϊ�룬�����ء�
        return getElapsedTimeInMicroSec() * 0.000001;      
    }

    /**
    *   �������ڻ�ȡ�ѹ�ȥ�ĺ�������
    *   @return ���ϴθ��������ѹ�ȥ�ĺ�������
    */
    double getElapsedTimeInMilliSec() {
        // ���ѹ�ȥ��΢����ת��Ϊ���룬�����ء�
        return this->getElapsedTimeInMicroSec() * 0.001;

    }

    /**
    *   �������ڻ�ȡ�ѹ�ȥ��΢������
    *   @return ���ϴθ��������ѹ�ȥ��΢������
    */
    long long getElapsedTimeInMicroSec() {
        // ���㵱ǰʱ����洢��ʱ�����_begin��֮���ʱ��������΢��Ϊ��λ���ء�
        return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count(); 
    }

protected:
    // �洢������ʱ�ĳ�ʼʱ��㡣
    time_point<high_resolution_clock> _begin;
    // high_resolution_clock��ϵͳ�Ͽ��õ���С��ʱ���ڵ�ʱ�ӡ�
};

#endif // !_CELLTIMESTAMP_H_