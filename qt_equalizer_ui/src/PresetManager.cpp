#include "PresetManager.h"

#include <QtGlobal>

PresetManager::PresetManager()
{
    m_presets.reserve(9);

    m_presets.append({QStringLiteral("Flat"), makeValues({0, 0, 0, 0, 0, 0, 0, 0, 0, 0})});
    m_presets.append({QStringLiteral("Rock"), makeValues({-1, 3, 5, 4, 1, -1, -2, -1, 2, 4})});
    m_presets.append({QStringLiteral("Pop"), makeValues({-1, 2, 4, 5, 3, -1, -2, -1, 1, 2})});
    m_presets.append({QStringLiteral("Jazz"), makeValues({0, 2, 3, 2, 0, -1, -1, 0, 2, 3})});
    m_presets.append({QStringLiteral("Classical"), makeValues({0, 1, 2, 3, 4, 3, 2, 1, 0, 0})});
    m_presets.append({QStringLiteral("Vocal"), makeValues({-2, -1, 2, 4, 5, 4, 2, 1, 0, 1})});
    m_presets.append({QStringLiteral("Dance"), makeValues({2, 4, 6, 4, 0, -2, -1, 2, 4, 5})});
    m_presets.append({QStringLiteral("Bass Boost"), makeValues({8, 7, 6, 4, 2, 0, -1, -2, -3, -4})});
    m_presets.append({QStringLiteral("Treble Boost"), makeValues({-4, -3, -2, -1, 0, 2, 4, 6, 7, 8})});
}

QStringList PresetManager::presetNames() const
{
    QStringList names;
    names.reserve(m_presets.size());
    for (const Preset &preset : m_presets) {
        names.append(preset.name);
    }
    return names;
}

QVector<int> PresetManager::presetValues(const QString &presetName) const
{
    for (const Preset &preset : m_presets) {
        if (preset.name.compare(presetName, Qt::CaseInsensitive) == 0) {
            return preset.values;
        }
    }

    return m_presets.isEmpty() ? QVector<int>(BandCount, 0) : m_presets.first().values;
}

QVector<int> PresetManager::makeValues(std::initializer_list<int> values) const
{
    QVector<int> result;
    result.reserve(BandCount);

    for (int value : values) {
        if (result.size() == BandCount) {
            break;
        }
        result.append(value);
    }

    while (result.size() < BandCount) {
        result.append(0);
    }

    return result;
}
