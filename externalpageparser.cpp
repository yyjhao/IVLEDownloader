#include "externalpageparser.h"

ExternalPageParser::ExternalPageParser(const Config& c, QObject *parent) :
    QObject(parent)
{
    config = c;
    manager = new QNetworkAccessManager(this);
}

Promise* ExternalPageParser::fetchFileInfo(const QStringList& list)
{
    QList<Promise*> ps;
    int ind = 0;
    for(QString name : list){
        if(config.contains(name)){
            auto& course = config[name];
            for(auto it = course.constBegin(); it != course.constEnd(); it++){
                ps.push_back(process(name, it.key(), it.value()));
            }
        }
    }
    return Promise::some(ps, this)->pipe([=](const QVariant& datas){
        QVariantList list = datas.toList();
        QMap<QString, QMap<QString, QVariantMap>> aggregate;
        for(auto d : list){
            if(d.isValid()){
                auto map = d.toMap();
                QString course = map["course"].toString();
                QString dir = map["course"].toString();
                QVariantMap files = map["result"].toMap();
                if(dir == QString(".")){
                    aggregate[course]["files"] = files;
                }else{
                    aggregate[course]["folders"][dir] = files;
                }
            }
        }
        QVariantMap map;
        for(auto it = aggregate.constBegin(); it != aggregate.constEnd(); ++it){
            auto& mm = it.value();
            QVariantMap m;
            for(auto itt = mm.constBegin(); itt != mm.constEnd(); ++itt){
                m[itt.key()] = itt.value();
            }
            map[it.key()] = m;
        }
        return (new Promise(this))->resolve(QVariant(map));
    });
}

void ExternalPageParser::setConfig(const Config& c)
{
    config = c;
}

const QVariantMap ExternalPageParser::resultListToMap(const QList<QString>& result, const QUrl& base)
{
    QVariantMap m;
    for(auto r : result){
        auto filename = r.right(r.size() - r.lastIndexOf("/") - 1);
        if(filename.indexOf(".") == -1){
            filename += QString(".html");
        }
        m[base.resolved(QUrl(r)).toString()] = filename;
    }
    return m;
}

Promise* ExternalPageParser::process(const QString& course, const QString& dir, const QPair<QUrl, QString>& info)
{
    Promise* p = new Promise(this);
    auto r = manager->get(QNetworkRequest(info.first));
    connect(r, &QNetworkReply::finished, [=](){
        if (r->error() == QNetworkReply::NoError){
            QByteArray content = r->readAll();

            QWebPage* page = new QWebPage(this);
            page->mainFrame()->setHtml(content);
            auto mf = page->mainFrame();
            auto els = mf->findAllElements(info.second);
            qDebug()<<"selector"<<info.second<<els.count();
            QList<QString> result;
            for(const auto& el : els){
                result.push_back(el.attribute("href"));
            }
            QVariantMap data;
            data["course"] = course;
            data["dir"] = dir;
            data["result"] = resultListToMap(result, info.first);
            p->resolve(data);

            page->deleteLater();
        }else{
            p->reject(QVariant());
        }
        r->deleteLater();
    });
    return p;
}
