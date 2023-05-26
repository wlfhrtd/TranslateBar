#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QVariantMap>
#include <QSettings>


class SettingsManager : public QObject
{
private:
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

public slots:
    void saveSettings(QString identifier1, QString identifier2, QVariantMap settingsObj);
    QVariantMap loadSettings(QString identifier1, QString identifier2);

};

#endif // SETTINGSMANAGER_H
