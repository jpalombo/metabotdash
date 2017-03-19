#ifndef PID_H
#define PID_H


class Pid
{
public:
    Pid(int Kp, int Ki, int Kd);

    int kp();
    int ki();
    int kd();
    void setKp(int);
    void setKi(int);
    void setKd(int);

    int update(int);
    void reset(int seed);

private:
    int mKp;
    int mKi;
    int mKd;

    int integral;
    int lasterr;
};

#endif // PID_H
