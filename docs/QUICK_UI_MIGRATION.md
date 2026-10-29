# Fresh Sj.Sim Qt Quick interface

Work is on `feature/qml-competition-config`. `main` is unchanged.
The previous `source/UI/Competition/Quick` experiment is not used by the new default interface.

## Run

```
scripts/build-quick-ui.sh
scripts/run-quick-ui.sh
scripts/run-quick-ui.sh --widgets
```

The standard qmake project now defaults to the fresh UI. `CONFIG+=widgets_only`
builds the original interface without any Qt Quick dependency. Existing preview
build flags remain available for the historical experiments.

## Design

The alpine sports identity pairs the existing Sj.Sim logo with warm chalk surfaces,
navy, cobalt actions and chartreuse accents. Barlow Condensed ExtraBold establishes
the display hierarchy, Barlow carries labels and prose, and IBM Plex Mono separates
numerical data. Asymmetric swept corners, numbered section markers and the home
screen's large season panel carry the identity across the application. Polish and
English labels use short, factual language.

The font files are bundled, so the identity does not depend on installed fonts.
They come from the official Google Fonts repository (`ofl/barlow`,
`ofl/barlowcondensed`, `ofl/ibmplexmono`). Their unmodified SIL Open Font Licenses
are included alongside the fonts in `source/UI/Quick/fonts` and in the resources.

One shared application footer carries the `1.3.0 beta-3` badge. Its version
comes from the same C++ constant used by Widgets and result sharing; the beta
label uses the chartreuse accent in both themes. Task pages and short dialogs
do not repeat the badge or appearance controls.

Shared semantic tokens define light/dark colours and typography. Controls show
keyboard focus; text sizing and reduced motion are persistent appearance options.
The home banner is omitted in short windows; the current logo remains in the header
and all three simulation modes stay directly available.

## Boundaries

New view code lives in `source/UI/Quick`. Stable QObject view nodes describe existing
controls and invoke their C++ handlers. Qt Quick draws the controls;
original Widgets remain as controllers and as a separately selectable UI.
Table adapters query the existing models lazily, preserve multi-selection and
cell editing, and forward original list actions. Charts are Qt Quick items that
read original chart series and forward point inspection to the original handlers.
Platform file pickers remain native desktop dialogs.

`QuickUi` presents task pages in one main window with shared Back navigation,
breadcrumbs and appearance settings. Original controller event loops and return
values remain available to C++ callers. Reparented controllers retain their original
owner's lifetime, so completed competition models cannot outlive their data.
Only confirmations, short value pickers and progress indicators remain modal.
Their original Qt layouts remain intact beneath the Quick scene because Qt's
standard dialogs rely on those layouts internally. Confirm/cancel actions stay
pinned below independently scrolling content.

The later authorization to fix C++ bugs also covers the scoring and live-contest
repairs listed below. The view adapter never independently simulates or saves.
The saved-data format is unchanged; historical stored scores are not rewritten.

## Validation

Run the integration suite with:

```sh
scripts/test-quick-ui.sh
```

`tests/quick-ui-smoke.cpp` uses disposable copies of the shipped data and settings.
The suite exercises the real controllers and models, including complete individual,
KO and team competitions, automatic simulation, a staged live jump, closing during
live presentation, manual KO selection, live condition/manipulation editors,
save creation, season configuration, archives, classification history, statistics,
ratings, all database categories, jump results and charts, standalone jump details,
settings, help and ancillary dialogs. It checks text entry through Quick keyboard
input, source validation, multi-selection, searching, menu actions and runtime
Polish/English switching. Progress-dialog cancellation is tested against the source
operation. Pointer-driven tests open every season tool, including the form
generator through both entry points; they wait through background refreshes before
choosing menu entries. Nested navigation tests verify one window, Back behavior,
retained parent state and shared appearance settings. Calendar tests cover direct
edits, selection, historical locks, event ownership/order, copying, undo, calendar
switching and deletion, custom training rules and preset save/load. Wind tests verify individual edits, bulk copying, undo, chosen-direction
generation and cancellation without changing stored settings.

The render sweep covers widths of 1440, 1024, 520 and 360 pixels, light/dark themes
and enlarged text. Captures are written to `build-quick-ui/ui-renders`.
The suite also verifies configuration tab switching and that the live jump summary
uses actual result data and remains visible at desktop test sizes. The suite
produces 350 counted renders, including the five UX workflows, roster editing, bounded presets, the form
generator, calendar fields, custom rules and links,
plus large-text variants. The original Widgets-only
qmake configuration also builds successfully without linking Qt Quick or QML.
Validation here uses Qt 5.15.3 with the offscreen software renderer; native window
manager behavior and other GPU drivers are outside that automated coverage.

## Start lists, preparation and live competition

`QuickRoster` supplies a shared list editor for individual competition rosters,
saved athlete lists and team member order. Available athletes use search, country
filters and checkboxes; selected athletes use an ordered list with Ctrl/Shift
selection, block dragging, a direct position field, keyboard moves, removal and
undo. Saved season lists can add/remove athletes or apply their order directly.
A narrow window switches between selection and ordering; desktop windows keep
both lists alongside each other. Team starters and reserves have explicit labels.
Original import, qualification, result-order and KO controls remain available.

The roster's opaque runtime keys identify domain object pointers. Shipped legacy
athletes can share a null UUID, so UUID-based deduplication would silently drop
valid competitors. This view does not rewrite stored identities or save formats.

Season preparation presents the calendar's event, hill and rules as information.
It exposes roster, starting gate and generated conditions without duplicating
calendar editors or overriding the event's importance.

The form generator has a bounded table workspace and a pinned Generate action.
On short/narrow windows its parameters have a separate tab. Preset windows are
bounded to the application/screen, keep their actions visible and scroll long
choices within the available space. Choices open upward when needed. Form preset
replacement searches form presets. Parameter edits and preset loading stage a draft;
All / Selected scope and an explicit Apply action update athlete settings. Generation
is disabled until the draft is applied, and requires a save and at least one athlete.
Apply stays pinned on the parameters tab; scope never changes implicitly with selection.

The competition workspace keeps the current athlete and last revealed jump above
parallel result/start-list tables on laptop and desktop windows. Target labels
use intrinsic text sizing instead of a guessed grid height. The default result
columns include each series' distance and total points; per-series points remain
available on demand. Team result rows also include their athletes' distances.
The start-list model publishes a snapshot under proper reset notifications when
a new round/group changes its athletes or their order; ordinary jump status changes
use data notifications. Competition regressions compare every projected row and
cursor to the current manager state and validate both model contracts.
The current KO group stays with the start list. Gate and jump actions remain in
the main toolbar alongside live mode and direct Progress / Conditions / Jump
buttons for simulation tools, weather editors, detailed data and virtual classifications.

Focused checks `--rosters-only` and `--geometry-only` cover real pointer dragging,
model contracts, source ordering, saved lists, reserves, actual preset replacement
and loading, pinned actions, popup bounds and long-list keyboard selection. These
checks are included in the default suite. Competition tests additionally verify
that critical panels stay visible together on laptops, the athlete/last-jump
panels do not intersect, distances are present by default and the category panel closes with
Escape. The original Widgets alternative is retained.

## Screen organization

- The home screen combines the current logo, the three simulation modes, library
  counts and application tools. Ctrl+1/2/3 starts the corresponding mode.
- Competition and single-jump setup use pinned tabs for configuration, athlete/hill
  editors, rules and weather. Essential selections stay on the overview; the
  simulation action remains fixed below the workspace. Settings separate simulation,
  database tools and sharing. Athlete editors separate data, skills and characteristics;
  hill editors separate data, scoring, parameters and characteristics. Wide windows
  use a section rail, and overflowing tab rows have explicit navigation controls.
- Wind editing shows all sector summaries together and exposes four parameters for
  the selected sector. Users can edit all sectors, copy one sector to the others,
  choose headwind/tailwind/calm, generate a profile or undo the last operation.
- Live competition keeps round progress and weather above independently scrolling
  athlete/controls, results and start-list panes on desktop. A compact jump summary
  stays above the results; expanded jump details are optional. Narrow windows
  use a single scrollable column. Menus invoke the original automatic simulation,
  cancellation and sharing actions.
- Season management starts with the next event and current classification. Task tabs
  expose every original workspace; save and tools stay above them, and the next-event
  action stays below the workspace on desktop. On narrow or short calendar screens
  it is omitted to give editing room; the competition overview retains it. The form
  generator has a direct action alongside Tools and Save.
- Calendar editing uses a list and an inspector on desktop, and separate list/edit
  tabs in narrow or short windows. The smallest viewports use a direct list/back
  transition and put secondary event operations in one menu. A click selects without
  navigating; Edit, Enter, double-click or the row arrow opens editing. Selection
  has a count and a clear action. Field edits apply directly, including multi-selection;
  one-step undo retains original event identities and historical results. Add,
  copy, delete and reorder operate on entire competition/training/trial blocks.
  Rules and qualification/classification links have their own inspector tabs.
  Main events, trainings and trial rounds retain separate rules and importance.
  Choosing custom rules opens their editor directly.
  Calendar creation selects the new calendar immediately; names, switching and
  preset operations are available in the same workspace. Remaining database and
  season editors use editable lazy tables and contextual menus.
- Tables retain original data, selection, editable/checkable cells, sorting and
  double-click actions, and add search, visible keyboard focus and state cues.
  Arrow keys navigate cells, F2 edits them, and Space toggles checkable cells.
  Ctrl+A selects all records; Insert adds and Delete removes where allowed, using
  the original validation and confirmation handlers.
- Charts retain the original series and point-inspection handlers, with zoom/reset
  and keyboard inspection. Detailed jump information and editors use normal page
  navigation; short confirmations use the shared controls without application chrome.

Closing a dialog releases its Quick scene before its controller can
be retired; reopening reconstructs the view from current controller state.

## Verified C++ repairs

- Wind averaging is deterministic; randomness remains in the generator. Scoring
  exposure and displayed sector ranges share the same sector-length calculation,
  including fractional and single-sector cases, without mutating raw wind samples.
- Leader and advancement lines use half-metre scoring, accumulated individual/team
  totals, ties, remaining places, KO lucky losers, wind/gate options and the coach's
  95% HS rule. Unavailable targets show a dash. Judges remain an estimate from recent
  jumps: the target predicts a score, not an unknown future judges' decision.
  Initial labels are calculated before the first jump, and changing the base gate
  refreshes the targets for the first athlete of later rounds too.
- Judges' minimum and maximum marks are both removed for ascending marks too.
  Measured wind remains recorded when wind points are disabled, and the 95% HS
  metadata remains available when gate points are disabled. The displayed threshold
  rounds upward to the next legal half metre.
- Live results freeze until the jump presentation finishes. Target lines refresh
  for the next athlete, visibility preferences remain respected, and closing during
  a presentation exits its nested wait immediately.
- Bulk wind controls retain valid settings pointers, submit normally and apply
  changes to controls before confirmation. Cancel preserves the stored profile.
  Accepting new competition wind settings regenerates the actual wind and targets.
- Negative result indices are rejected. Regression checks exercise these cases
  before the screen sweep; `--backend-only` runs them without the full render pass.
- Database editor pointers and the last-opened list index start in a defined state.
  Editable table values are distinguished from checkbox roles, including models
  whose default flags allow checkboxes without providing a check state.

- Action menus use explicit item insertion. Synthetic season tools update atomically,
  so the background refresh cannot remove menu entries while a user clicks them.
- Calendar catalog insert/remove notifications now bracket the actual data change.
  Removing the final calendar clears models and editing state. Preset loading safely
  replaces event/classification graphs and maps every hill to the current save;
  completed or cancelled events prevent replacement. Reordering qualifications
  after their target competition is rejected.

- Classification editors initialize their source pointer, reject stale selections
  after calendar changes and preserve classification identity when applying edits.
  Existing event links remain valid.

Focused checks are available as `--navigation-only`, `--menus-only` and
`--workflow-only`; the default integration run includes them too.

## Logic and UX audit, 2026-10-21

[The audit report](BUGFIX_AUDIT_2026-10-21.md) records thirteen individually
committed fixes, their reproduction scenarios, regression checks and commit IDs.
Run `scripts/test-quick-ui.sh --audit-only` for the focused audit checks; they are
also included in the default integration suite. The full updated suite passes
with 292 responsive renders, and the Widgets-only build remains supported.


## Five-workflow UX redesign, 2026-10-21

[The UX report](UX_IMPROVEMENTS_2026-10-21.md) describes explicit form
parameter application, visible table search/selection, direct saved-list actions,
contextual competition tools and calendar selection without forced navigation.
Run `scripts/test-quick-ui.sh --ux-only` for these workflows and an individual
competition; the default suite also includes these checks. Competition options
use secondary Progress / Conditions / Jump sections; live presentation stays in
the main toolbar, and weather readings open Conditions directly. Short desktop
windows use tighter header spacing to retain both critical tables, including KO.


## Available space and last jump, 2026-10-21

[The follow-up report](SPACE_AND_LAST_JUMP_2026-10-21.md) covers full-height
catalog lists in the database, season and save selection; independent scrolling
for editors; and compact secondary actions. `--space-only` checks catalog growth
and editor boundaries. The competition summary now exposes judges, wind, gate,
landing, compensation components and totals, and positions directly. Judges have
separate marks with discarded extremes; signed wind and total compensation use
accessible positive/negative colours. The last jump comes first in narrow windows.


## Competition refinements and qualification sources, 2026-10-21

The last-jump card has a distinct place badge, athlete flag and muted leader gap;
team competitions use aggregate team results. Its width is capped at 720 px,
the athlete card at 320 px and their combined row at 1052 px. Content determines
their height, leaving the remaining workspace to full-width results/start-list tables.
Direct Progress, Conditions and Jump buttons occupy the existing control bar.
Previous-round places remain visible before a jump and during live presentation
until the current result is revealed. Rank snapshots avoid dereferencing result
vector elements after mutation.

Transient dialog titles/bodies scroll above pinned responsive action rows; native
resize/layout changes and application resizing trigger a coalesced geometry fit.
Number fields count every press immediately and repeat after 250 ms at 40 ms intervals.
Calendar qualification sources now correctly precede their dependent competition;
reordering checks and load-time repairs enforce the same rule.

See `docs/COMPETITION_REFINEMENTS_2026-10-21.md` for the complete manual checklist,
implementation notes and validation, including `--interaction-only` regressions.


## Season editing workflow, 2026-10-22

Calendar browsing/editing is independent of the active playback calendar. The
calendar workspace exposes creation, explicit activation, bulk insertion,
multi-selection block dragging and integrated classification editing. A shared
undo/redo timeline covers edits, catalog operations and preset replacement while
protecting newly played results. Displaced qualification sources remain pending,
are serialized, block dependent starts and are restored when the order is valid.
Classification names remain editable after use; scoring is locked and copying
creates an independent classification. Points-table notifications and decimal
editing are corrected, and overview standings synchronize on activation.

Standard dialog actions follow the application language. Message content uses
its full width below the title, with pinned actions. Competition preparation
starts with Auto and displays deterministic, bounded wind-dependent gate
recommendations with an apply action. The competition status bar shows the hill
flag, signed wind colors and leader rather than the start counter; last-jump rank
stays blank during flight. The next-event footer adds the flag, type, HS and format.

The full suite passes with 350 renders, including the new classification layouts,
and the Widgets-only build passes. See
[the manual checklist and validation report](SEASON_WORKSPACE_2026-10-22.md).
