/*
 * An action used to show one recent file in the recent file menu
 * By Yao Yujian
 */

#ifndef RECENTFILEACTION_H
#define RECENTFILEACTION_H

#include <QAction>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>
class recentFileAction : public QAction
{
    Q_OBJECT
public:
    explicit recentFileAction(QObject *parent = 0);
    
signals:
    
public slots:
    void setFile(const QString& filePath);

private slots:
    void openFile();
private:
    QUrl filePath;
};

#endif // RECENTFILEACTION_H
