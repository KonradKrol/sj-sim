pragma Singleton
import QtQuick 2.15

QtObject {
    readonly property bool dark: appearance ? appearance.dark : false
    readonly property bool compact: appearance ? appearance.compactUi : false
    readonly property int controlHeight: compact ? 34 : 42
    readonly property real scale: appearance ? appearance.textScale : 1
    readonly property int motion: appearance && appearance.reducedMotion ? 0 : 160
    readonly property color background: dark ? "#111d28" : "#f1f0e9"
    readonly property color surface: dark ? "#1a2c3c" : "#ffffff"
    readonly property color raised: dark ? "#243b4e" : "#e7eaf0"
    readonly property color ink: dark ? "#f1f6ff" : "#112b49"
    readonly property color muted: dark ? "#b0c2d8" : "#51657c"
    readonly property color outline: dark ? "#607c9a" : "#718298"
    readonly property color accent: dark ? "#96c2ff" : "#274dd5"
    readonly property color accentText: dark ? "#08244b" : "#ffffff"
    readonly property color selection: dark ? "#183d64" : "#dfebfb"
    readonly property color warning: dark ? "#f2c780" : "#87550c"
    readonly property color success: dark ? "#92d9a2" : "#22633a"
    readonly property color danger: dark ? "#ffb4a7" : "#a32f25"
    readonly property color stadium: "#142e42"
    readonly property color snow: "#f1f6ff"
    readonly property color signal: "#d8ee85"
    readonly property string mono: "IBM Plex Mono"
    readonly property string bodyFamily: "Barlow"
    readonly property string displayFamily: "Barlow Condensed"
    readonly property color divider: dark ? "#34495c" : "#d9dfe4"
    readonly property color headerMuted: "#b5c8df"
    readonly property color track: "#24517b"
    readonly property color grid: dark ? "#18334f" : "#d5dfe9"
    readonly property real body: 12.5 * scale
    readonly property real caption: 10.5 * scale
    readonly property real heading: 22 * scale
    readonly property real display: 54 * scale
    readonly property real title: 30 * scale
    readonly property real section: 18 * scale
    readonly property FontMetrics titleMetrics: FontMetrics { font.family: Theme.displayFamily; font.pointSize: Theme.title }
    readonly property FontMetrics bodyMetrics: FontMetrics { font.family: Theme.bodyFamily; font.pointSize: Theme.body }
    readonly property FontMetrics headingMetrics: FontMetrics { font.family: Theme.displayFamily; font.pointSize: Theme.heading }
    readonly property FontMetrics captionMetrics: FontMetrics { font.family: Theme.bodyFamily; font.pointSize: Theme.caption }
    readonly property real headerHeight: titleMetrics.height + captionMetrics.height + 24
    readonly property int gap: compact ? 8 : 12
    readonly property int radius: 10
}
