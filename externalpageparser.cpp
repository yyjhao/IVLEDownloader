#include "externalpageparser.h"

ExternalPageParser::ExternalPageParser(const QVariantMap& c, QObject *parent) :
    QObject(parent)
{
    config = c;
    manager = new QNetowrkAccessManager(this);
}

Promise* ExternalPageParser::fetchFileInfo(const QStringList& list)
{
    QList<Promise*> ps;
    int ind = 0;
    for(QString name : list){
        if(config.contains(name)){
            auto& course = config[name];
            for(auto it = course.begin(); it != course.end(); it++){
                ps.push_back(process(name, it->key(), it->value()));
            }
        }
    }
    return new Promise(ps, this)->pipe([=](const QVariant& datas){
        QVariantList list = datas.toList();
        QVariantMap map;
        for(auto d : list){
            QPair<QString, QString>
        }
        
    })
}

void ExternalPageParser::setConfig(const QVariantMap& c)
{
    config = c;
}

Promise* ExternalPageParser::process(const QPair<QString, QString> info)
{

}