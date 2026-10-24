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
- Completed: explicit DD.MM.YYYY assertions across pl_PL/en_US/de_DE, Polish numeric leap-day input and ISO storage compatibility.

Additional queued task (user request):
- Completed: stable last-jump identity and independent visual marker across manual/bulk jumps, KO and team model rebuilding; confirmed advancement remains green under selection and hover. See LATEST_JUMP_FEEDBACK_2026-10-24.md.
- Completed: KO grouping crash and manual-edit lifetime fixes; see TEAM_KO_UX_2026-10-24.md.
