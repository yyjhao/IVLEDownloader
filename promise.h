#ifndef PROMISE_H
#define PROMISE_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>

class Promise : public QObject
{
    Q_OBJECT
public:
    static Promise* all(QList<Promise*>, QObject *parent = 0);
    static Promise* some(QList<Promise*>, QObject *parent = 0);
    enum State{
        pending,
        success,
        failure
    };
    typedef std::function<Promise*(const QVariant&)> Monad;
    explicit Promise(QObject *parent = 0);
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
    Promise* resolve(const QVariant& = QVariant());
    Promise* reject(const QVariant& = QVariant());
    Promise* advance(const QVariant& = QVariant());

private:
    State s;
    int tasksCount;
    QVariantList datas;
    QVariant data;
};

#endif // PROMISE_H
