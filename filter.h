#ifndef FILTER_H
#define FILTER_H

#include <QVector>

class Filter
{
public:
    Filter(int size = 10);
    void reset();
    int update(int p, int d);
    int value();
    void setSize(int);

private:
    int m_size;
    int datacount;
    int last;

    QVector<int> ping;
    QVector<int> distance;

};

#endif // FILTER_H
