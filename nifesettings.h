#ifndef NIFESETTINGS_H
#define NIFESETTINGS_H

struct NifeSettings
{
    int selectedSystem;
    int mainX;
    int mainY;
    int mainWidth;
    int mainHeight;
    bool showArtwork;
};

void writeNifeSettings(const NifeSettings& settings);

void readNifeSettings(NifeSettings& settings);

#endif // NIFESETTINGS_H
