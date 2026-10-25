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

Completed jump-history request:
- Earlier-series selector, Shift-click series preview for individual/team results and expandable weighted calculations.
- Footer Shift-click hint replaces the Back hint; Back shortcut remains available.
- Team Quick results omit the combined distances column; per-series points remain.
- Pre-jump lead/deficit is hidden before the first jump of a series and for a rounded zero-point tie.
- Preview pages are deleted before rebuilding, preventing accumulation during repeated selection.
See JUMP_HISTORY_2026-10-25.md for behavior and validation.

Completed priority: shared EmptyState layout recursion removed; full 399-render suite has zero recursive-layout warnings. See LAYOUT_RECURSION_FIX_2026-10-24.md.
