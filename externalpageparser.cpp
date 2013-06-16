#include "externalpageparser.h"

ExternalPageParser::ExternalPageParser(const QVariantMap& c, QObject *parent) :
    QObject(parent)
{
    config = c;
}

Promise* ExternalPageParser::fetchFiles(const QStringList&)
{
    return new Promise();
}

void ExternalPageParser::setConfig(const QVariantMap& c)
{
    config = c;
}
