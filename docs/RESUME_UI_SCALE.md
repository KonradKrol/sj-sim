# UI scale checkpoint

Implemented so far:
- Persistent `QuickAppearance.uiScale` (default 0.70) and independent `textScale`.
- Two percentage inputs in appearance settings; central `Theme.dp()` geometry helper.
- Theme typography, common theme spacing and transient widths use UI scale.
- Event dates display DD.MM.YYYY; ISO storage remains compatible.

Remaining from the current UI request:
- Apply Theme.dp() consistently to control dimensions, screen spacing and icons. Current scaling is partial; no global scene transform is used.
- Completed in the team/KO UX changes: plain live result tables without search, selection toolbar, multiselection or sorting headers; team expansion is controlled by three saved modes.
- Updated appearance regressions cover independent parameters, repeated typed edits, stepping, persistence and a small window.
- Add explicit fixed-date-format regression assertions.

Additional queued task (user request):
- Fix intermittent last-jump result highlighting. The athlete/team that most recently jumped must remain highlighted reliably after manual jumps, bulk simulation, KO updates and team-model rebuilds. Check interaction with advancement/selection/podium colors.
- Completed: KO grouping crash and manual-edit lifetime fixes; see TEAM_KO_UX_2026-10-24.md.
