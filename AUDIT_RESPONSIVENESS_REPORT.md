# Qt responsiveness audit — 15 findings

**Scope.** Read-only audit of the Qt Widgets user interface, focused on whether
the application remains operable on small laptop displays, narrow windows,
high-DPI/scaled desktops, and with translated or unusually long content. No
production code was changed. The audit covered all 57 `.ui` files under
`source/UI`, the code that constructs or sizes widgets at runtime, and a
temporary off-screen Qt layout probe. The temporary probe was removed after
use.

**Runtime limitation.** The complete application was not launched because this
checkout needs a Qt 6-capable build while only Qt 5.15 is locally available.
The layout probe instantiated the standard Designer forms without application
logic and measured their `sizeHint()` and `minimumSizeHint()`. It therefore
confirms layout floors and explicit sizing, but screenshots and end-to-end
interaction still need verification in the real Qt 6 build.

**Reference viewport.** A nominal `1366x768` laptop does not provide all 768
vertical pixels to an ordinary window: the title bar and desktop panel/taskbar
reduce the available client area. The repair target should consequently be an
available client area of at most `1280x680`, with `1024x600` as the compact
stress case. Testing only a maximized `1920x1080` window will miss most issues
in this report.

**Priority.** P0 means a normal workflow has a hard size constraint that can
put required content or actions outside the available screen. P1 means a major
screen clips, compresses, or overflows at a compact width/height. P2 means a
systemic scaling/localization weakness or missing regression protection.

## 1. Application settings is fixed taller than a 768p laptop can safely show

- **Classification / priority:** confirmed responsiveness failure, P0.
- **Source evidence:** the form is designed at `392x835` in
  `source/UI/AppSettings/AppSettingsWindow.ui:5-10`. Immediately after
  `setupUi`, the constructor calls `setFixedSize(size())` at
  `source/UI/AppSettings/AppSettingsWindow.cpp:28`. There is no outer
  `QScrollArea`.
- **Measured behavior:** the standard form's layout minimum hint was
  `388x803`; the real constructor then locks the window to `392x835`.
- **Failure trigger:** open Settings on a 1366x768 laptop, at 125–200% desktop
  scaling, or with a large system font. The lower controls cannot be brought
  into view by resizing the dialog.
- **Smallest safe repair direction:** remove the fixed-size call, place the
  settings body in a widget-resizable `QScrollArea`, and keep the important
  action row reachable outside or at the bottom of that scroll area.

## 2. Database editor enforces a 910-pixel minimum height

- **Classification / priority:** confirmed responsiveness failure, P0.
- **Source evidence:** the top-level dialog explicitly sets minimum height 910
  at `source/UI/DatabaseEditor/DatabaseEditorWindow.ui:19-24`. Its main tab
  widget independently requires `785x750` at lines 112-117. The four scroll
  areas inside individual tabs do not relax the top-level height.
- **Measured behavior:** requesting `1024x700` produced an actual window size
  of `1024x910`; requesting `800x600` produced `800x910`.
- **Failure trigger:** open Database Editor on any display whose available
  height is below 910 pixels, including essentially every 768p laptop.
- **Smallest safe repair direction:** remove both hard vertical minima; allow
  the tab widget to shrink; keep each tab's content scrollable; verify the tab
  bar and all editor actions at `1024x600`.

## 3. Manual KO-group editor is a fixed `600x820` modal

- **Classification / priority:** confirmed responsiveness failure, P0.
- **Source evidence:** `getManualKOGroupsFromDialogInputs()` constructs a
  dialog and calls `setFixedSize(600, 820)` at
  `source/UI/Competition/CompetitionManagerWindow.cpp:445-448`. The dialog is
  modal (`exec()` at line 490). Its principal list comes from a form designed
  at 794 pixels high (`source/UI/EditorWidgets/KOSystem/KOGroupsListView.ui:5-10`).
- **Failure trigger:** choose manual KO groups on a 768p display. Window chrome
  makes the modal taller than the usable desktop, and resizing is forbidden;
  the user may be unable to reach or reliably operate the confirmation UI.
- **Smallest safe repair direction:** make the modal resizable, cap its initial
  size to `QScreen::availableGeometry()`, let the group list take the remaining
  space, and use a normal compact `QDialogButtonBox` rather than the fixed
  `111x111` submit button created at lines 476-478.

## 4. The live competition screen has a desktop-width layout floor

- **Classification / priority:** confirmed layout overflow, P1; likely P0 for
  the core competition workflow after runtime widgets are inserted.
- **Source evidence:** the form starts at `1542x904` in
  `source/UI/Competition/CompetitionManagerWindow.ui:5-10`. Its top command
  bar is a single non-wrapping horizontal layout (lines 25-87), and its main
  content is another three-column horizontal layout (line 101 and lines
  1398-1450). No splitter or outer scroll container is present.
- **Measured behavior:** the static form already reports a `1240x396` minimum
  layout hint. At narrower requested sizes Qt accepts the outer dialog size but
  the child layout has insufficient width, so content must clip or collapse.
- **Runtime amplification:** the constructor adds a result-detail widget with
  `QSizePolicy::Maximum` at
  `source/UI/Competition/CompetitionManagerWindow.cpp:157-160`; that widget is
  designed around `387x534`
  (`source/UI/Competition/JumperCompetitionResultsWidget.ui:5-15`). Team mode
  inserts yet another tree into the main horizontal layout at
  `CompetitionManagerWindow.cpp:137-141`.
- **Failure trigger:** run a competition in a window narrower than about 1240
  client pixels, use team mode, or enlarge UI text. The start list, results,
  jump controls, and jumper details compete for an impossible horizontal span.
- **Smallest safe repair direction:** replace the central row with a
  `QSplitter` or responsive stacked layout; provide a compact breakpoint that
  moves jumper details below results; move secondary commands into a menu or
  wrapping toolbar; keep the Next action outside scrollable content.

## 5. Competition configuration reserves fixed widths for alternative views

- **Classification / priority:** confirmed inflexible layout, P1.
- **Source evidence:** the window is authored at `1320x888`
  (`source/UI/Competition/CompetitionConfigWindow.ui:5-10`) with start-list
  controls and a large toolbox in one horizontal row (line 21 and line 314).
  At runtime, the mutually exclusive teams and jumpers views are nevertheless
  assigned fixed widths of 390 and 280 pixels at
  `source/UI/Competition/CompetitionConfigWindow.cpp:201-202`.
- **Measured behavior:** even before the runtime editor widgets are inserted,
  the form has a `675x274` minimum hint. Dynamic hill, wind, snow, and rules
  editors add their own size hints to the same desktop-oriented composition.
- **Failure trigger:** configure a competition at 1024x600, use a high-DPI
  scale, or show longer translated labels. Fixed start-list widths consume
  space the central editor needs, while the window provides no overall scroll
  or compact rearrangement.
- **Smallest safe repair direction:** remove fixed widths; use stretch factors
  and sensible *small* minimums; put the start-list/editor boundary in a
  splitter; at a compact breakpoint stack the start list above the toolbox.

## 6. Season manager has an 1136-pixel static minimum before dynamic content

- **Classification / priority:** confirmed layout overflow, P1.
- **Source evidence:** the manager is designed at `1405x1003`
  (`source/UI/Seasons/SimulationSaveManagerWindow.ui:5-10`) and uses toolboxes
  whose pages are designed around `845x632` (for example lines 285-290). It has
  no outer scroll area.
- **Measured behavior:** the static form reports a `1136x455` minimum layout
  hint. A requested width of 1024 or 800 is accepted only at the top-level
  window; the child layout cannot fit within it.
- **Runtime amplification:** the constructor adds a database list with a
  600-pixel minimum width at
  `source/UI/Seasons/SimulationSaveManagerWindow.cpp:275-281`.
- **Failure trigger:** manage a season at widths below roughly 1136 pixels,
  especially on the jumpers-list page. Content is clipped/compressed and there
  is no parent-level horizontal escape route.
- **Smallest safe repair direction:** remove the 600-pixel child minimum; make
  toolbox pages independently scrollable; split or stack list/editor pairs;
  ensure the primary season/competition actions remain visible at 1024x600.

## 7. Save statistics puts an entire filter console in one horizontal row

- **Classification / priority:** confirmed layout overflow, P1.
- **Source evidence:** `SimulationRatingsWindow` starts at `1816x936`
  (`source/UI/Seasons/Stats/SimulationRatingsWindow.ui:5-10`). Its first row is
  one `QHBoxLayout` with 22 items, including export, two spin boxes, calendar
  and hill selectors, four injected filter groups, and display toggles (lines
  21-291). Only the Records tab body has a scroll area (line 538); the filter
  row and other tabs do not.
- **Measured behavior:** the form reports a `1544x643` minimum layout hint—the
  largest measured in the audit.
- **Failure trigger:** open save statistics on a 1366-wide laptop or any
  narrower window. Filters and actions exceed the client width before table
  content is considered.
- **Smallest safe repair direction:** turn filters into a collapsible/filter
  drawer or a multi-row grid; allow labels and groups to wrap; give every dense
  tab a local scroll strategy; keep Export available in a toolbar/menu.

## 8. Jumper statistics repeats the same non-wrapping filter-bar failure

- **Classification / priority:** confirmed layout overflow, P1.
- **Source evidence:** the dialog is designed at `1215x844`
  (`source/UI/Seasons/Stats/JumperStatsWindow.ui:5-10`). Its header is one
  21-item horizontal layout containing identity, flags, calendar/hill inputs,
  and several injected filter groups (lines 23 onward).
- **Measured behavior:** the static form's minimum layout hint is `1210x251`;
  an ordinary 1024-wide window is already below the content floor.
- **Failure trigger:** inspect a jumper at 1024 width, increase text size, or
  display a long jumper name. Header controls have no reflow policy.
- **Smallest safe repair direction:** separate identity from filtering; wrap or
  grid the filters below the title; make long names elide or wrap without
  displacing actions; test both Polish and English text.

## 9. Fixed 500-pixel result panes make season pages vertically rigid

- **Classification / priority:** confirmed inflexible sizing, P1.
- **Source evidence:** both live classification results and archived
  classification results are forced to 500 pixels high at
  `source/UI/Seasons/SimulationSaveManagerWindow.cpp:232` and line 267. They
  sit inside pages that also contain selectors, labels, and action controls.
- **Failure trigger:** use season management at 600–680 pixels of available
  height. The result pane cannot yield vertical space, so surrounding controls
  are clipped or the toolbox/page requires more height than the window.
- **Smallest safe repair direction:** replace fixed heights with expanding
  policies and modest minimums; use a splitter if the result table shares a
  page with another major panel; let the table's own scrollbars handle rows.

## 10. Dynamic settings dialogs are frozen before their content is installed

- **Classification / priority:** confirmed sizing-order bug, P1.
- **Source evidence:** the wind and inrun-snow dialogs call
  `setFixedSize(dialog->size())` *before* assigning their layout or inserting
  the editor, at
  `source/UI/Competition/CompetitionManagerWindow.cpp:1627-1635` and
  `1656-1664`. The wind editor itself is designed at `409x526`
  (`source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.ui:5-10`). The
  same pattern appears in
  `source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.cpp:107-119` and
  `source/UI/JumpManipulation/JumpManipulatorConfigWindow.cpp:80-97`.
- **Failure trigger:** open one of these nested editors on a small or scaled
  display. The dialog cannot resize to its inserted content or to the screen;
  depending on the platform's initial dialog size, content is clipped or the
  layout produces an unusable window.
- **Smallest safe repair direction:** install content first, call
  `adjustSize()`, cap the result to the current screen's available geometry,
  and leave the dialog resizable. Add a scroll area around long editor bodies.

## 11. Archived competition results assumes a 1400-pixel desktop

- **Classification / priority:** confirmed oversized initial layout, P1.
- **Source evidence:** the archive double-click handler resizes its dialog to
  `1400x800` at
  `source/UI/Seasons/SimulationSaveManagerWindow.cpp:848-853`. It then lays out
  the results table/tree, a `JumperCompetitionResultsWidget`, and a fixed
  150-pixel webhook button in a single horizontal row (lines 854-881).
- **Failure trigger:** open archived results on a 1366-wide screen (or smaller)
  and select an entry so the detail widget is populated. Window chrome and the
  fixed adjacent controls leave inadequate width; there is no responsive
  stacking or screen clamp.
- **Smallest safe repair direction:** derive the initial size from available
  geometry; use a splitter; stack details below the result view at compact
  widths; move the webhook action to a dialog button/toolbar.

## 12. Results tables repeatedly size every column to its full contents

- **Classification / priority:** confirmed compact-width degradation, P1.
- **Source evidence:** the live competition uses `ResizeToContents` for result
  and team headers at
  `source/UI/Competition/CompetitionManagerWindow.cpp:140-161` and repeats the
  policy after updates throughout the file (for example lines 727-748). The
  archive results calls `resizeColumnsToContents()` at
  `source/UI/Seasons/SimulationSaveManagerWindow.cpp:859`. Save-statistics
  tables do the same repeatedly at
  `source/UI/Seasons/Stats/SimulationRatingsWindow.cpp:206-535`.
- **Failure trigger:** show long names, translated headers, large numeric
  values, or a compact window. Content-based widths maximize every column at
  once and push useful columns out of view; repeated recalculation can also
  make the layout visibly jump as results change.
- **Smallest safe repair direction:** define a per-table column policy: fixed
  compact widths for rank/flag/numeric fields, stretch for the primary name,
  interactive or bounded content sizing for secondary columns, elision plus
  tooltips for overflow, and horizontal scrolling as the final fallback.

## 13. Large Designer geometries are used without screen-aware startup sizing

- **Classification / priority:** confirmed systemic startup problem, P1.
- **Source evidence:** major dialogs have desktop-sized initial geometries:
  competition configuration `1320x888`, competition manager `1542x904`, form
  generator `1425x843`, Help `1362x878`, new-season configuration `1247x1035`,
  season manager `1405x1003`, jumper statistics `1215x844`, save statistics
  `1816x936`, and single-jump results `1325x891` (each at lines 5-10 of its
  corresponding `.ui` file). Repository-wide search found no use of `QScreen`,
  `availableGeometry()`, `saveGeometry()`, or `restoreGeometry()`.
- **Measured nuance:** several forms can technically shrink—their Designer
  geometry is not always an explicit minimum. That does not make startup safe:
  the code opens them with `exec()`/`show()` and never chooses a size based on
  the current monitor.
- **Failure trigger:** first launch on a small display, move the application to
  a smaller secondary monitor, or change display scaling between launches.
- **Smallest safe repair direction:** introduce one shared window-sizing
  helper that restores geometry only if it intersects the current available
  screen, otherwise uses a percentage of available geometry; never treat the
  Designer canvas size as a portable launch size.

## 14. There is no coherent compact-mode or outer-scroll architecture

- **Classification / priority:** confirmed architectural gap, P1.
- **Source evidence:** only 8 of the 57 Designer forms contain any
  `QScrollArea`, and those are generally local list/editor regions rather than
  a fallback for the full workflow. The principal competition, competition
  setup, season manager, new-season, single-jump setup/results, help, and
  settings forms do not have a whole-content compact fallback. No `QSplitter`
  or dock widget exists in the `.ui` files.
- **Failure trigger:** any combination of low resolution, snapped half-screen
  use, large fonts, translated content, or multiple side-by-side panels.
- **Smallest safe repair direction:** define two layout modes rather than
  patching each pixel value independently: desktop mode with side-by-side
  panels and compact mode with stacked/collapsible panels. Use splitters for
  user-controlled boundaries and scroll areas only around bodies, not primary
  action bars.

## 15. Fixed pixel boxes and non-wrapping text are fragile under DPI and translation

- **Classification / priority:** confirmed systemic scaling risk, P2.
- **Source evidence:** the UI corpus contains 143 `minimumSize` properties, 154
  `maximumSize` properties, 34 fixed `QSizePolicy` declarations, and 563
  explicit point-size declarations. Examples include fixed flag/image boxes in
  `source/UI/Competition/JumperCompetitionResultsWidget.ui:43-90`, fixed-width
  start-list widgets in `CompetitionConfigWindow.cpp:201-202`, and a
  non-wrapping save-name label in
  `source/UI/Seasons/SimulationSaveManagerWindow.ui:60-79`. Help explicitly
  disables tab elision at `source/UI/HelpWindow.ui:77-79`.
- **Failure trigger:** 125–200% scaling, accessibility fonts, font fallback
  when Quicksand/Ubuntu variants are missing, long user-provided names, or
  English text that is wider than Polish.
- **Smallest safe repair direction:** audit each hard minimum/maximum and retain
  only semantic sizes (for example, an icon's aspect ratio); use font metrics,
  word wrapping/elision, tooltips, and layouts instead of text-sized pixel
  boxes; test translated strings and missing-font fallback.

## Recommended repair sequence

1. Add a shared screen-aware sizing policy and a repeatable viewport test
   harness. This prevents later per-screen repairs from regressing startup.
2. Remove the three hard blockers first: fixed Settings size, Database Editor's
   910-pixel minimum, and the fixed 600x820 KO modal.
3. Refactor the core workflows in this order: Competition Manager, Competition
   Configuration, Simulation Save Manager, then New Season. Keep primary
   actions permanently reachable.
4. Refactor both statistics headers into compact filter drawers/grids and set
   explicit, bounded table-column policies.
5. Sweep remaining fixed sizes, long labels, and dynamically constructed
   dialogs; then validate translation, font fallback, and high DPI.

## Implementation guidance for the fixing agent

- Treat `QScreen::availableGeometry()`—not raw screen resolution—as the upper
  bound. Re-evaluate it when a top-level window changes screens.
- Prefer layout-managed widgets. Avoid `setFixedSize`, large minimum sizes, and
  fixed spacer widths unless the dimension is genuinely invariant.
- Keep Accept/Save/Next/Cancel actions in a stable footer or toolbar outside
  scrolling bodies. A user should never have to resize a window to finish or
  cancel a modal workflow.
- Use `QSplitter` for two or three peer data panels. For compact mode, stack or
  collapse secondary panels such as jumper details and filters.
- Preserve native scrolling in tables and lists. An outer scroll area should
  protect form/editor content, not create nested scrolling around every data
  view.
- Do not blindly replace every `ResizeToContents` with `Stretch`; assign a
  policy by column role and verify that numeric columns remain readable.
- Store geometry per top-level window, validate restored rectangles against
  currently connected screens, and provide sane first-run defaults.

## Verification matrix and acceptance criteria

Run the real Qt 6 application in both Polish and English, with normal and large
system fonts, at these **available client sizes**:

| Client area | Purpose |
|---|---|
| `1024x600` | compact stress case; every workflow must remain completable |
| `1280x680` | practical small-laptop target below a nominal 1366x768 screen |
| `1366x728` | common laptop with reserved desktop chrome |
| `1920x1080` | verify desktop mode still uses additional space well |
| `960x540` at 200% scaling | high-DPI/fallback check; scrolling is acceptable |

For each size, exercise Main Menu → Settings, Database Editor, Single Jumps,
Single Competition, Season creation, Season Manager, a live individual and
team competition, manual KO groups, archived results, Jumper Statistics, and
Save Statistics.

A repaired screen passes only when:

1. It initially opens wholly within the current screen's available geometry.
2. The user can reach every primary action and cancel/close the workflow using
   mouse and keyboard without changing display resolution.
3. No controls overlap, disappear behind siblings, or collapse to unusable
   widths; long text wraps or elides intentionally.
4. Tables retain a useful primary column, scroll horizontally when necessary,
   and do not continuously jump widths as data changes.
5. Moving the window between monitors or changing scale does not strand it
   off-screen, and restored geometry is clamped to an available screen.
6. Qt warnings are checked during the run, and screenshots are captured for
   every matrix cell to make future visual regressions reviewable.
