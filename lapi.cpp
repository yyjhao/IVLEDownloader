#include "lapi.h"

Lapi::Lapi(QString token, QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    this->token = token;
}

Promise* Lapi::validate()
{
    auto p = new Promise(this);
    ajaxGet(genRequest("Validate"), [=](QVariant data){
        auto map = data.toMap();
        bool valid = map.value("Success").toBool();
        if(valid){
            QString newToken = map.value("Token").toString();
            if(newToken != token){
                this->token = newToken;
            }
            p->resolve(this->token);
        }else{
            p->reject(QVariant("invalid token"));
        }
    }, [=](QNetworkReply::NetworkError err){
        p->reject(QVariant(err));
    });
    return p;
}

Promise* Lapi::fetchUserInfo()
{
    return ajaxGet(genRequest("UserName_Get"));
}

Promise* Lapi::fetchWorkbin(const QString& id)
{
    return ajaxGet(genRequest("Workbins", QString("CourseID=%1&Duration=0").arg(id)));
}

Promise* Lapi::fetchModules()
{
    return ajaxGet(genRequest("Modules", "Duration=0&IncludeAllInfo=false"));
}

Promise* Lapi::fetchAnnouncement()
{
    return ajaxGet(genRequest("Announcements_Unread", "TitleOnly=true"));
}


QString Lapi::getWorkBinDownloadUrl(const QString& id){
    return QString("https://ivle.nus.edu.sg/api/downloadfile.ashx?APIKey=%1&AuthToken=%2&ID=%3&target=workbin").arg(APIKEY).arg(token).arg(id);
}

void Lapi::ajaxGet(const QNetworkRequest& re,
    const std::function<void(QVariant)>& success,
    const std::function<void(QNetworkReply::NetworkError)>& failure)
{
    auto r = manager->get(re);
    connect(r, &QNetworkReply::finished, [=](){
        if (r->error() == QNetworkReply::NoError){
            QByteArray re = r->readAll();
            auto data = QJsonDocument::fromJson(re).toVariant();
            if(data.isValid()) success(data);
            else success(QString(re).remove('"'));
        }else{
            failure(r->error());
        }
        r->deleteLater();
    });

}

Promise* Lapi::ajaxGet(const QNetworkRequest& re)
{
    auto p = new Promise(this);
    ajaxGet(re, [=](QVariant data){
        p->resolve(data);
    }, [=](QNetworkReply::NetworkError error){
        p->reject(error);
    });
    return p;
}

QNetworkRequest Lapi::genRequest(const QString& service, const QString& additionalQuery)
{
    QString addition;
    if(!additionalQuery.isEmpty()){
        addition = QString("&") + additionalQuery;
    }
    return QNetworkRequest(QUrl(QString("https://ivle.nus.edu.sg/api/Lapi.svc/%1?APIKey=%2&Token=%3&AuthToken=%3&output=json%4")
            .arg(service)
            .arg(APIKEY)
            .arg(token)
            .arg(addition)));
}

void Lapi::setToken(const QString& t)
{
    token = t;
}

QNetworkAccessManager* Lapi::getManager()
{
    return manager;
}
