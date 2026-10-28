#pragma once
#include "WeatherPresets.h"
#include "Scoring.h"
#include "../hills/HillProfile.h"
#include <QObject>
#include <QVariantMap>
#include <QVariantList>
#include <QDate>
#include <memory>
#include <array>

// UI-owned configuration; running jobs receive an immutable field snapshot.
class SimulationWeatherSetup : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap values READ values NOTIFY changed)
    Q_PROPERTY(QVariantList references READ references NOTIFY changed)
    Q_PROPERTY(QVariantList preview READ preview NOTIFY previewChanged)
    Q_PROPERTY(QVariantList basePreview READ basePreview NOTIFY changed)
    Q_PROPERTY(QVariantList presets READ presets NOTIFY changed)
    Q_PROPERTY(double previewSeconds READ previewSeconds WRITE setPreviewSeconds NOTIFY previewChanged)
    Q_PROPERTY(QString message READ message NOTIFY changed)
    Q_PROPERTY(double previewOrigin READ previewOrigin WRITE setPreviewOrigin NOTIFY previewChanged)
public:
    explicit SimulationWeatherSetup(QObject *parent=nullptr);
    void setHill(const HillProfile &);
    void clearHill();
    void setDate(const QDate &date) {eventDate=date;}
    QVariantMap values() const;
    QVariantList references() const;
    QVariantList preview() const;
    QVariantList basePreview() const;
    QVariantList presets() const;
    double previewSeconds() const {return previewTime;}
    double previewOrigin() const {return origin;}
    void setPreviewOrigin(double);
    QString message() const {return error;}
    std::shared_ptr<const SimulationV2::WeatherField> field() const {return weather;}
    const SimulationV2::WindParameters &windParameters() const {return wind;}
    const SimulationV2::PrecipitationParameters &precipitationParameters() const {return precipitation;}
    Q_INVOKABLE void applyPreset(int preset);
    Q_INVOKABLE void suggest();
    Q_INVOKABLE void reroll();
    Q_INVOKABLE void setValue(const QString &key,double value);
    Q_INVOKABLE void setReference(int index,const QString &key,double value);
    Q_INVOKABLE QVariantMap referenceAt(double distance) const;
    Q_INVOKABLE void putReference(double distance,double strength,double direction);
    Q_INVOKABLE void removeReference(int index);
    Q_INVOKABLE QVariantMap station(int index) const;
    Q_INVOKABLE void setStation(int index,const QString &key,double value);
    void setPreviewSeconds(double);
signals:
    void changed();
    void previewChanged();
private:
    HillProfile hill;
    bool hasHill=false,modified=false,precipitationEdited=false;
    int preset=-1;
    double variability=0,differences=0,previewTime=0,requestedDirection=0,requestedStrength=0,origin=0;
    SimulationV2::Surface surface=SimulationV2::Surface::Snow;
    SimulationV2::Seed seed=0;
    std::array<double,6> stationZeroDirections{};
    SimulationV2::WindParameters wind;
    SimulationV2::PrecipitationParameters precipitation;
    std::shared_ptr<const SimulationV2::WeatherField> weather;
    QDate eventDate=QDate::currentDate();
    QString error;
    void rebuild();
    void inferControls();
    void regenerate(bool includePrecipitation);
};
