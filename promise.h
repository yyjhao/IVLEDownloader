#ifndef PROMISE_H
#define PROMISE_H

#include <QObject>
#include <QVariant>

class Promise : public QObject
{
    Q_OBJECT
public:
    enum State{
        pending,
        success,
        failure
    };
    explicit Promise(QObject *parent = 0);
    explicit Promise(std::initializer_list<Promise*>, QObject *parent = 0);
    State state();
    
signals:
    void done(const QVariant&);
    void fail(const QVariant&);
public slots:
    void resolve(const QVariant&);
    void reject(const QVariant&);

private:
    State s;
    int tasksCount;
    QVariantList datas;
};

#endif // PROMISE_H
