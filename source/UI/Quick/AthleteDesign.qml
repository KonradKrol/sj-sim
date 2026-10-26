import QtQuick 2.15

QtObject {
    property int variant: 0
    readonly property var colors: [
        {paper:"#111d25",surface:"#1b2c37",ink:"#f0f4ec",muted:"#b3c3cc",line:"#3e535f",accent:"#d5f575",accentInk:"#14201b"},
        Theme.dark ? {paper:"#292d2c",surface:"#343b38",ink:"#f2ebda",muted:"#c3c6b8",line:"#667268",accent:"#f0a17d",accentInk:"#2d241f"} : {paper:"#f1ebdb",surface:"#fffaf0",ink:"#233635",muted:"#57665a",line:"#9aa68e",accent:"#a34429",accentInk:"#fff6e8"},
        Theme.dark ? {paper:"#142b2c",surface:"#213c3c",ink:"#e3f6ef",muted:"#a9c9bd",line:"#517d70",accent:"#91e3c4",accentInk:"#173c32"} : {paper:"#e5f0e9",surface:"#fbfff9",ink:"#183c32",muted:"#4d7064",line:"#9ab9a5",accent:"#126a55",accentInk:"#f0fff5"},
        {paper:"#14213d",surface:"#213458",ink:"#fff4db",muted:"#c1c8dc",line:"#6378a0",accent:"#f2cf8b",accentInk:"#302717"},
        Theme.dark ? {paper:"#25263d",surface:"#343650",ink:"#f2f0ff",muted:"#bfc1dc",line:"#6b6b95",accent:"#cbbdff",accentInk:"#292044"} : {paper:"#ebeaf7",surface:"#faf9ff",ink:"#292c4d",muted:"#626383",line:"#aaa7c6",accent:"#6850b6",accentInk:"#fffaff"}
    ]
    readonly property var palette: colors[variant]
    readonly property color paper: palette.paper
    readonly property color surface: palette.surface
    readonly property color ink: palette.ink
    readonly property color muted: palette.muted
    readonly property color line: palette.line
    readonly property color accent: palette.accent
    readonly property color accentInk: palette.accentInk
    readonly property string headingFont: variant===2 ? Theme.bodyFamily : Theme.displayFamily
    readonly property string detailFont: variant===1 || variant===3 ? Theme.mono : Theme.bodyFamily
}
