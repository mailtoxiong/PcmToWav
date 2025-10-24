#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <QString>
#include <QStringList>
#include <QVector>

#include <initializer_list>

class PresetManager
{
public:
    static const int BandCount = 10;

    PresetManager();

    QStringList presetNames() const;
    QVector<int> presetValues(const QString &presetName) const;

private:
    struct Preset
    {
        QString name;
        QVector<int> values;
    };

    QVector<Preset> m_presets;

    QVector<int> makeValues(std::initializer_list<int> values) const;
};

#endif // PRESETMANAGER_H
