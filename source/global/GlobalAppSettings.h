#ifndef GLOBALAPPSETTINGS_H
#define GLOBALAPPSETTINGS_H

#include "JumpDataInfoChoice.h"

class QString;

class GlobalAppSettings
{
private:
    GlobalAppSettings();
    static GlobalAppSettings * m_globalAppSettings;

public:
    GlobalAppSettings(GlobalAppSettings &) = delete;
    static GlobalAppSettings * get();

private:
    short languageID = 0;
    bool showCalendarEditorHelp = true;
    bool showSeasonJumpersAndHillsHelp = true;
    JumpDataInfoChoice jumpDataInfoChoice;

    // One Discord webhook address used to publish results images (see DiscordWebhook).
    QString discordWebhookUrl;

    bool liveCompetition = true;
    double liveCompetitionSpeedMulti = 1;

public:
    short getLanguageID() const;
    void setLanguageID(const short &newLanguageID);

    enum Language{
        Polish,
        English,
        Czech
    };

    bool loadFromJson();
    bool writeToJson();
    bool getShowCalendarEditorHelp() const;
    void setShowCalendarEditorHelp(bool newShowCalendarEditorHelp);
    bool getShowSeasonJumpersAndHillsHelp() const;
    void setShowSeasonJumpersAndHillsHelp(bool newShowSeasonJumpersAndHillsHelp);
    JumpDataInfoChoice getJumpDataInfoChoice() const;
    JumpDataInfoChoice & getJumpDataInfoChoiceReference();
    void setJumpDataInfoChoice(const JumpDataInfoChoice &newJumpDataInfoChoice);
    QString getDiscordWebhookUrl() const;
    void setDiscordWebhookUrl(const QString &url);
    bool getLiveCompetition() const;
    void setLiveCompetition(bool newLiveCompetition);
    double getLiveCompetitionSpeedMulti() const;
    void setLiveCompetitionSpeedMulti(double newLiveCompetitionSpeedMulti);
};

#endif // GLOBALAPPSETTINGS_H
