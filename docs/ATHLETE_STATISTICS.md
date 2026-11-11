# Athlete statistics

The season athlete library opens a focused Qt Quick screen for the selected
athlete: identity, four summary facts, four event filters and one position chart.

`AthleteStatistics.qml` lays out the facts and event-type toggles.
`AthletePositionPlot.qml` draws positions with Qt Quick Shapes, with first place
at the top. Hovering a point shows a compact contextual window with the hill,
date, event type, position and points. Left/right arrows inspect points with the
keyboard. The chart supports empty and single-result histories.

`JumperStatsWindow::report()` supplies filtered positions and existing summary
facts. `setEventFilter()` uses the same event filtering as the backend, so facts
and chart update together. No generic Node filters or metric selector are shown.
Legacy Widgets charts and CSV export remain available to the Widgets interface.
No save format changes are needed.
