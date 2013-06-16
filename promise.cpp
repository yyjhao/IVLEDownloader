#include "promise.h"

#include <QDebug>

Promise::Promise(QObject *parent) :
    QObject(parent)
{
    s = pending;
}

Promise::Promise(QList<Promise *> promises, QObject *parent) :
    Promise(parent)
{
    tasksCount = promises.size();
    datas.reserve(tasksCount);
    int i = 0;
    for(auto p : promises){
        p->setParent(this);
        datas.push_back(QVariant());
        connect(p, &Promise::done, [=](const QVariant& data){
            if(s == pending){
                tasksCount--;
                datas.replace(i, data);
                if(tasksCount == 0){
                    resolve(QVariant(datas));
                }
            }
        });

        connect(p, &Promise::fail, [=](const QVariant& data){
            if(s == pending){
                reject(data);
            }
        });
        i++;
    }
}

Promise::Promise(const QList<std::function<Promise* (const QVariant&)>>& funcs, QObject *parent) :
    Promise(parent)
{
    tasksCount = 0;
    tasks = funcs;
    step(QVariant());
}

Promise* Promise::pipe(const Monad& suc,
                        const Monad& f)
{
    Promise* p = new Promise(this->parent());
    this->then([=](QVariant data){
        auto pp = suc(data);
        connect(pp, &Promise::done, p, &Promise::done);
        connect(pp, &Promise::fail, p, &Promise::fail);
        connect(pp, &Promise::progress, p, &Promise::progress);
    }, [=](QVariant error){
        auto pp = f(error);
        connect(pp, &Promise::done, p, &Promise::done);
        connect(pp, &Promise::fail, p, &Promise::fail);
        connect(pp, &Promise::progress, p, &Promise::progress);
    });
    return p;
}

Promise* Promise::pipe(const Monad& suc)
{
    Promise* p = new Promise(this->parent());
    this->then([=](QVariant data){
        auto pp = suc(data);
        connect(pp, &Promise::done, p, &Promise::resolve);
        connect(pp, &Promise::fail, p, &Promise::reject);
        connect(pp, &Promise::progress, p, &Promise::progress);
    }, [=](QVariant error){
        p->reject(error);
    });
    return p;
}


void Promise::step(const QVariant& data){
    if(tasks.size() == 0){
        resolve(data);
    }else{
        tasks.takeFirst()(data)->then([=](const QVariant data){
            QVariantMap map;
            map["data"] = data;
            map["ind"] = tasksCount;
            emit progress(map);

            tasksCount++;

            step(data);
        }, [=](const QVariant error){
            QVariantMap map;
            map["ind"] = tasksCount;
            map["error"] = error;
            reject(map);
        })->setParent(this);
    }
}

Promise* Promise::then(const std::function<void(const QVariant&)> &suc)
{
    connect(this, &Promise::done, suc);
    return this;
}

Promise* Promise::then(const std::function<void(const QVariant&)>& suc, const std::function<void(const QVariant&)>& f)
{
    connect(this, &Promise::done, suc);
    connect(this, &Promise::fail, f);
    return this;
}

Promise* Promise::then(const std::function<void(const QVariant&)>& suc, const std::function<void(const QVariant&)>& f,
                        const std::function<void(const QVariant&)>& p)
{
    connect(this, &Promise::done, suc);
    connect(this, &Promise::fail, f);
    connect(this, &Promise::progress, p);
    return this;
}

void Promise::resolve(const QVariant& data){
    if(s != pending){
        return;
    }else{
        s = success;
        emit done(data);
        deleteLater();
    }
}

void Promise::reject(const QVariant& data){
    if(s != pending){
        return;
    }else{
        s = failure;
        emit fail(data);
        deleteLater();
    }
}

Promise::State Promise::state(){
    return s;
}
