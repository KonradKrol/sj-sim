# UI scale checkpoint

Implemented so far:
- Persistent `QuickAppearance.uiScale` (default 0.70) and independent `textScale`.
- Two percentage inputs in appearance settings; central `Theme.dp()` geometry helper.
- Theme typography, common theme spacing and transient widths use UI scale.
- Event dates display DD.MM.YYYY; ISO storage remains compatible.

Remaining from the current UI request:
- Apply Theme.dp() consistently to control dimensions, screen spacing and icons. Current scaling is partial; no global scene transform is used.
- Simplify live results and KO tables: remove selection toolbar/multiselection and sorting headers while preserving jump preview and team expansion.
- Update appearance regressions for the two settings and verify both 70% and 100%, including small windows.
- Add explicit fixed-date-format regression assertions.
