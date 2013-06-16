#include "promise.h"

Promise::Promise(QObject *parent) :
    QObject(parent)
{
    s = pending;
}

Promise::Promise(std::initializer_list<Promise*> promises, QObject *parent) :
    Promise(parent)
{
    tasksCount = promises.size();
    datas.clear();
    datas.reserve(tasksCount);
    int i = 0;
    for(auto p : promises){
        p->setParent(this);
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
    }
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
