#pragma once

#include <string>
#include <QDebug>

// Linux builds deliberately use this no-op compatibility layer. The bundled
// DPP binary is Windows-only; simulation features remain available while
// Discord webhook delivery is disabled.
namespace dpp {
namespace colors {
constexpr unsigned int coffee = 0;
constexpr unsigned int summer_sky = 0;
}

class embed_footer {
public:
    embed_footer &set_text(const std::string &) { return *this; }
};

class embed {
public:
    embed &set_color(unsigned int) { return *this; }
    embed &set_title(const std::string &) { return *this; }
    embed &set_description(const std::string &) { return *this; }
    embed &add_field(const std::string &, const std::string &, bool) { return *this; }
    embed &set_footer(const embed_footer &) { return *this; }
};

class message {
public:
    std::string content;

    message() = default;
    explicit message(const std::string &text) : content(text) {}
    message &add_embed(const embed &) { return *this; }
};

class webhook {
public:
    explicit webhook(const std::string &) {}
};

class cluster {
public:
    explicit cluster(const std::string &) {}
    void execute_webhook(const webhook &, const message &) const {}
};
} // namespace dpp

inline QDebug operator<<(QDebug debug, const std::string &value)
{
    return debug << QString::fromStdString(value);
}
