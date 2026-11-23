#pragma once

#include <QString>

// A short, non-blocking confirmation shown at the bottom of the main window for a few seconds
// (for example "Wysłano na Discorda."). It never takes focus or blocks clicks. Use a message
// box instead when the player has to read an explanation or decide something.
namespace Notice {
void show(const QString &text);
}
