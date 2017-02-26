#ifndef ABSTRACTMODE_H
#define ABSTRACTMODE_H

#include <QObject>

class AbstractMode : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMode(QObject *parent = 0);

signals:

public slots:
};

#endif // ABSTRACTMODE_H