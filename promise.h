#ifndef PROMISE_H
#define PROMISE_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>

class Promise : public QObject
{
    Q_OBJECT
public:
    enum State{
        pending,
        success,
        failure
    };
    typedef std::function<Promise*(const QVariant&)> Monad;
    explicit Promise(QObject *parent = 0);
    explicit Promise(QList<Promise*>, QObject *parent = 0);
    explicit Promise(const QList<std::function<Promise*(const QVariant&)>>&, QObject *parent = 0);
    State state();
    Promise* pipe(const Monad&, const Monad&);
    Promise* pipe(const Monad&);

    Promise* then(const std::function<void(const QVariant&)>&);
    Promise* then(const std::function<void(const QVariant&)>&, const std::function<void(const QVariant&)>&);
    Promise* then(const std::function<void(const QVariant&)>&, const std::function<void(const QVariant&)>&, const std::function<void(const QVariant&)>&);

    // then (done, fail, progress)
    
signals:
    void done(const QVariant&);
    void fail(const QVariant&);
    void progress(const QVariant&);
public slots:
    void resolve(const QVariant& = QVariant());
    void reject(const QVariant& = QVariant());

private:
    State s;
    int tasksCount;
    QVariantList datas;
    QList<std::function<Promise* (const QVariant&)>> tasks;
    void step(const QVariant&);
};

#endif // PROMISE_H
