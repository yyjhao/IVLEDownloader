#include "promise.h"

#include <QDebug>

Promise::Promise(QObject *parent) :
    QObject(parent)
{
    s = pending;
}

Promise* Promise::all(QList<Promise *> promises, QObject *parent)
{
    Promise* np = new Promise(parent);
    np->tasksCount = promises.size();
    np->datas.reserve(np->tasksCount);
    int i = 0;
    if(np->tasksCount == 0){
        np->resolve();
    }
    for(auto p : promises){
        p->setParent(np);
        np->datas.push_back(QVariant());
        p->then([=](const QVariant& data){
            if(np->s == pending){
                np->tasksCount--;
                np->datas.replace(i, data);
                if(np->tasksCount == 0){
                    np->resolve(QVariant(np->datas));
                }
            }
        }, [=](const QVariant& data){
            if(np->s == pending){
                np->reject(data);
            }
        });
        i++;
    }
    return np;
}

Promise* Promise::some(QList<Promise *> promises, QObject *parent)
{
    Promise* np = new Promise(parent);
    np->tasksCount = promises.size();
    np->datas.reserve(np->tasksCount);
    int i = 0;
    if(np->tasksCount == 0){
        np->resolve();
    }
    for(auto p : promises){
        p->setParent(np);
        np->datas.push_back(QVariant());
        p->then([=](const QVariant& data){
            if(np->s == pending){
                np->tasksCount--;
                np->datas.replace(i, data);
                if(np->tasksCount == 0){
                    np->resolve(QVariant(np->datas));
                }
            }
        }, [=](const QVariant& data){
            if(np->s == pending){
                np->tasksCount--;
                np->datas.replace(i, QVariant());
                if(np->tasksCount == 0){
                    np->resolve(QVariant(np->datas));
                }
            }
        });
        i++;
    }
    return np;
}

Promise* Promise::pipe(const Monad& suc,
                        const Monad& f)
{
    Promise* p = new Promise(this->parent());
    this->then([=](QVariant data){
        suc(data)->then([=](const QVariant& data){
            p->resolve(data);
        }, [=](const QVariant& error){
            p->reject(error);
        }, [=](const QVariant& progress){
            p->advance(progress);
        });
    }, [=](QVariant error){
        f(error)->then([=](const QVariant& data){
            p->resolve(data);
        }, [=](const QVariant& error){
            p->reject(error);
        }, [=](const QVariant& progress){
            p->advance(progress);
        });
    });
    return p;
}

Promise* Promise::pipe(const Monad& suc)
{
    Promise* p = new Promise(this->parent());
    this->then([=](QVariant data){
        suc(data)->then([=](const QVariant& data){
            p->resolve(data);
        }, [=](const QVariant& error){
            p->reject(error);
        }, [=](const QVariant& progress){
            p->advance(progress);
        });
    }, [=](QVariant error){
        p->reject(error);
    });
    return p;
}

Promise* Promise::then(const std::function<void(const QVariant&)> &suc)
{
    if(s == pending) connect(this, &Promise::done, suc);
    else if(s == success) suc(this->data);
    return this;
}

Promise* Promise::then(const std::function<void(const QVariant&)>& suc, const std::function<void(const QVariant&)>& f)
{
    if(s == pending){
        connect(this, &Promise::done, suc);
        connect(this, &Promise::fail, f);
    } else if(s == success) suc(this->data);
    else f(this->data);
    return this;
}

Promise* Promise::then(const std::function<void(const QVariant&)>& suc, const std::function<void(const QVariant&)>& f,
                        const std::function<void(const QVariant&)>& p)
{
    if(s == pending){
        connect(this, &Promise::done, suc);
        connect(this, &Promise::fail, f);
        connect(this, &Promise::progress, p);
    }else if(s == success) suc(this->data);
    else f(this->data);
    return this;
}

Promise* Promise::resolve(const QVariant& data)
{
    if(s != pending){
        return this;
    }else{
        s = success;
        emit done(data);
        this->data = data;
        deleteLater();
        return this;
    }
}

Promise* Promise::reject(const QVariant& data)
{
    if(s != pending){
        return this;
    }else{
        s = failure;
        emit fail(data);
        this->data = data;
        deleteLater();
        return this;
    }
}

Promise* Promise::advance(const QVariant& data)
{
    emit progress(data);
    return this;
}

Promise::State Promise::state(){
    return s;
}
