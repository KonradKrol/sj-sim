# Season-wide chronology

When dates are enabled, play is restricted to the earliest pending date
across every calendar of the current season. All event types participate.
Calendars can be browsed and activated freely; activation never bypasses the
start guard. On the same date any calendar may be chosen, but its internal
order and qualification dependencies remain mandatory.

The last resolved date is the maximum date of played or cancelled events
across the current season. A pending event before it is legacy backlog:
play remains blocked until its date is repaired or it is cancelled. Finished
historical results are not modified. Missing dates and malformed pending
calendar order also block play. Turning dates off retains the previous flow.

SeasonChronology.h is the shared pure policy. The manager guards normal and
quick simulation; the overview shows the blocker and offers explicit
navigation. Stale/invalid events lead to their calendar editor. The season
bar date follows the global pending date, or the reached date for backlog.
Cancellation cannot jump over earlier events in another calendar and advance
form into the future. Cancelling stale events never rewinds form.

No save schema change is required. Autosave retains work in progress even
when manual confirmation is blocked. tests/season-chronology-regressions.h
covers calendar switching, same-day choice, local order, qualification
sources, cancellation, legacy backlog, missing dates and undated seasons.
