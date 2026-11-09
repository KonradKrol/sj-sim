pragma Singleton
import QtQuick 2.15

QtObject {
    readonly property string localeName: navigation ? navigation.localeName : "pl_PL"
    readonly property bool dark: appearance ? appearance.dark : false
    readonly property int colorTheme: appearance ? appearance.colorTheme : 0
    readonly property var palette: colorsFor(colorTheme,dark)
    function colorsFor(index,night) { return palettes[Math.max(0,Math.min(2,index))*2+(night ? 1 : 0)] }
    readonly property var palettes: [
        {background:"#f1f0e9",surface:"#ffffff",raised:"#e7eaf0",ink:"#112b49",muted:"#51657c",outline:"#718298",accent:"#274dd5",accentText:"#ffffff",selection:"#dfebfb",stadium:"#142e42",signal:"#d8ee85",divider:"#d9dfe4",headerMuted:"#b5c8df",track:"#24517b",grid:"#d5dfe9"},
        {background:"#111d28",surface:"#1a2c3c",raised:"#243b4e",ink:"#f1f6ff",muted:"#b0c2d8",outline:"#607c9a",accent:"#96c2ff",accentText:"#08244b",selection:"#183d64",stadium:"#142e42",signal:"#d8ee85",divider:"#34495c",headerMuted:"#b5c8df",track:"#24517b",grid:"#18334f"},
        {background:"#fff0f7",surface:"#fffafd",raised:"#f6deee",ink:"#39172e",muted:"#735065",outline:"#9b718b",accent:"#bd0065",accentText:"#ffffff",selection:"#ffd5eb",stadium:"#421333",signal:"#ff9ed1",divider:"#e7c4d9",headerMuted:"#eac1dd",track:"#742653",grid:"#e8c8dc"},
        {background:"#200f20",surface:"#2e182d",raised:"#44213d",ink:"#fff0fa",muted:"#d4afc9",outline:"#a87698",accent:"#ff70bf",accentText:"#340021",selection:"#62264e",stadium:"#39112e",signal:"#ffa4d6",divider:"#633653",headerMuted:"#eac1dd",track:"#782a60",grid:"#49223d"},
        {background:"#f3f5f4",surface:"#ffffff",raised:"#e6ece9",ink:"#20292d",muted:"#586368",outline:"#77858a",accent:"#206c63",accentText:"#ffffff",selection:"#d7eee6",stadium:"#233734",signal:"#9bdfc9",divider:"#d5ddda",headerMuted:"#bed3ca",track:"#355e54",grid:"#d7e1dd"},
        {background:"#141a18",surface:"#1e2723",raised:"#2c3731",ink:"#eff5f1",muted:"#b7c9bf",outline:"#7a9586",accent:"#8bd7c2",accentText:"#062b25",selection:"#244a3f",stadium:"#1c302a",signal:"#9bdfc9",divider:"#3b4c43",headerMuted:"#bed3ca",track:"#355e54",grid:"#293e33"}
    ]
    readonly property bool compact: appearance ? appearance.compactUi : false
    readonly property real uiScale: appearance ? appearance.uiScale : 1
    function dp(value) { return value * uiScale }
    function fontSize(value) { return value * scale }
    readonly property real controlHeight: dp(compact ? 26 : 42)
    readonly property real scale: uiScale * (appearance ? appearance.textScale : 1)
    readonly property int motion: appearance && appearance.reducedMotion ? 0 : treatment.motion
    readonly property color background: palette.background
    readonly property color surface: palette.surface
    readonly property color raised: palette.raised
    readonly property color ink: palette.ink
    readonly property color muted: palette.muted
    readonly property color outline: palette.outline
    readonly property color accent: palette.accent
    readonly property color accentText: palette.accentText
    readonly property color selection: palette.selection
    readonly property color warning: dark ? "#f2c780" : "#87550c"
    readonly property color advancementBase: dark ? "#21483c" : "#d7efdd"
    readonly property color advancementSurface: Qt.rgba(advancementBase.r*.8+surface.r*.2,advancementBase.g*.8+surface.g*.2,advancementBase.b*.8+surface.b*.2,1)
    readonly property color success: dark ? "#92d9a2" : "#22633a"
    readonly property color advancementInk: Qt.rgba(success.r*.8+muted.r*.2,success.g*.8+muted.g*.2,success.b*.8+muted.b*.2,1)
    readonly property color danger: dark ? "#ffb4a7" : "#a32f25"
    readonly property color stadium: palette.stadium
    readonly property color stadiumDanger: "#ffb4a7"
    readonly property color snow: "#f1f6ff"
    readonly property color signal: palette.signal
    readonly property string mono: "IBM Plex Mono"
    readonly property string bodyFamily: "Barlow"
    readonly property string displayFamily: "Barlow Condensed"
    readonly property color divider: palette.divider
    readonly property color headerMuted: palette.headerMuted
    readonly property color track: palette.track
    readonly property color grid: palette.grid
    readonly property real body: 12.5 * scale
    readonly property real caption: 10.5 * scale
    readonly property real heading: (compact ? 17 : 22) * scale
    readonly property real display: (compact ? 32 : 54) * scale
    readonly property real title: (compact ? 22 : 30) * scale
    readonly property real section: (compact ? 14.5 : 18) * scale
    readonly property FontMetrics titleMetrics: FontMetrics { font.family: Theme.displayFamily; font.pointSize: Theme.title }
    readonly property FontMetrics bodyMetrics: FontMetrics { font.family: Theme.bodyFamily; font.pointSize: Theme.body }
    readonly property FontMetrics headingMetrics: FontMetrics { font.family: Theme.displayFamily; font.pointSize: Theme.heading }
    readonly property FontMetrics captionMetrics: FontMetrics { font.family: Theme.bodyFamily; font.pointSize: Theme.caption }
    readonly property real headerHeight: titleMetrics.height + captionMetrics.height + dp(24)
    readonly property real gap: dp(compact ? 4 : 12)
    readonly property real radius: dp(treatment.panelRadius)
    readonly property real surfaceSweep: treatment.sweep
    readonly property var treatments: [
        {roundness:-1,asymmetry:0,cornerLimit:24,controlRadius:6,controlBorder:0,focusBorder:2,pressScale:1,checkRadius:4,tabRadius:10,panelRadius:10,sweep:24,motion:160,tracking:0},
        {roundness:.65,asymmetry:1,cornerLimit:24,controlRadius:14,controlBorder:1.5,focusBorder:3,pressScale:.96,checkRadius:10,tabRadius:20,panelRadius:12,sweep:28,motion:190,tracking:.3},
        {roundness:.18,asymmetry:0,cornerLimit:4,controlRadius:3,controlBorder:1,focusBorder:2,pressScale:1,checkRadius:2,tabRadius:3,panelRadius:6,sweep:10,motion:120,tracking:0}
    ]
    readonly property var treatment: treatments[colorTheme]
    readonly property bool expressive: colorTheme===1
    readonly property bool minimal: colorTheme===2
    readonly property real surfaceRoundness: treatment.roundness
    readonly property real surfaceAsymmetry: treatment.asymmetry
    readonly property real surfaceCornerLimit: dp(treatment.cornerLimit)
    readonly property real controlRadius: dp(treatment.controlRadius)
    readonly property real controlBorder: treatment.controlBorder
    readonly property real focusBorder: treatment.focusBorder
    readonly property real pressScale: treatment.pressScale
    readonly property int feedbackEasing: expressive ? Easing.OutBack : Easing.OutCubic
    readonly property int actionWeight: expressive ? Font.Bold : Font.DemiBold
    readonly property int displayWeight: expressive ? Font.Black : Font.ExtraBold
    readonly property real actionTracking: treatment.tracking
    readonly property real checkRadius: treatment.checkRadius
    readonly property color tabFill: expressive ? accent : stadium
    readonly property color tabInk: expressive ? accentText : snow
    readonly property real tabRadius: dp(treatment.tabRadius)
}
