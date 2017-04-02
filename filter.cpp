#include "filter.h"
#include <QDebug>

Filter::Filter(int size) :
    m_size(size), last(0), lastUnconfidence(0), ping(m_size), distance(m_size)
{ 
    reset();
}

void Filter::reset()
{
    datacount = 0;
    last = 30000;
    for (int i = 0; i < m_size; i++)
        distance[i] = -1;
}

int Filter::update(int p, int d)
{
    int i;
    // Reject dodgy p values
    if (p > 30000 || p < 10)
        return last;

    if (last == 30000)
        last = p;

    // Check if there is already a reading with this d value
    for (i = 0; i < m_size; i++)
        if (distance[i] == d) {
            return last;
        }

    ping[datacount % m_size] = p;
    distance[datacount % m_size] = d;
    datacount++;

    if (datacount < m_size)  { // not enough history yet
        last = p;
        return last;
    }

    // We've now got some history so can work out slope of line
    // Work out the slope between all pairs of points and take the median
    int vecsize = (m_size * (m_size-1))/2;
    QVector<float> mvec(vecsize);
    i = 0;
    for (int j = 1; j < m_size; j++) {
        for (int k = 0; k < j; k++) {
            mvec[i] = (float)(ping[j] - ping[k]) / (float)(distance[j] - distance[k]);
            i++;
        }
    }
    std::sort(mvec.begin(), mvec.end());
    float m = mvec[vecsize/2];

    // Now using this slope, work out what the current reading would be
    // starting at all of the points and take the median
    QVector<int> pvec(m_size-1);
    i = 0;
    for (int j = 0; j < m_size; j++) {
        if (distance[j] != d) {   // skip current point
            pvec[i] = (int)((float)(d-distance[j]) * m) + ping[j];
            i++;
        }
    }
    std::sort(pvec.begin(), pvec.end());
    last = pvec[(m_size-1)/2];
    lastUnconfidence = qAbs(last - p) * 100 / p;
    return last;
}

int Filter::value()
{
    return last;
}

int Filter::unconfidence()
{
    // returns percentage difference between last value  and last reading
    qDebug() << last << lastUnconfidence;
    return lastUnconfidence;
}

void Filter::setSize(int size)
{
    m_size = size;
}

