# Athlete statistics

Season athlete library exposes a Statistics action for the selected athlete.
The old backend filters remain available through the collapsed filter/export
area: competition range, calendar merging, event types, hill types, specific
hill, classification and DSQ inclusion.

The Qt Quick view uses the shared Shape-based BalanceLabPlot with inverse
ranking axis, a summary of best/average/worst position and standard deviation,
point context, country flag and optional pack portrait. Plot presentation
options default to their previous behavior for existing lab callers.

Legacy results retain takeoff/flight rating plots. New-engine results expose
actual recorded quality and distance instead of obsolete zero ratings.
CSV exports contain the visible data and context, with UTF-8, quoted context
fields and atomic replacement. Empty athletes are supported without requiring
any previously played contest. No save format changes are needed.
