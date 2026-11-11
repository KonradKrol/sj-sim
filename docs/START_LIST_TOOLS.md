# Start list tools — November 2026

Individual non-KO contests: participant list selection on the left; grouped
ordering tools above the start list on the right. KO: participant selection
only; groups stay in their dedicated editor. Team tools never import another
contest's squads. Full copying preserves existing squads and constructs any
missing countries from the current candidate pool.

Source selection uses a compact anchored QML popup, defaults to the current
season and can browse earlier seasons. Lists of results and classifications
sort stronger competitors last; absent entries and ties are shuffled within
their cohort. Country ordering is stable within each country.

Potential is computed through SimulationV2::jumperPotential and shared by
start-list ordering and KO group balancing. It uses the engine baseline,
current form, daily form where available, athlete perks, hill, gate and
weather; it does not randomly sample a jump. Pre-contest grouping receives
the same potential callback as start-list ordering. The in-contest grouping
dialog uses the current contest clock and weather. Back-end automatic
grouping without a supplied callback uses the stored hill/balance and calm
weather as a fallback.

## Additional KO methods

- Interleaved (5): alternate the direction of every rank basket.
- Baskets without compatriots (6): proper edge coloring of the bipartite
  basket/country graph, with groups as colors. Each group gets at most one
  athlete per basket and country whenever country counts make that possible.
  An overrepresented country is split into quota-sized virtual nodes; only
  then are repeated countries unavoidable.
- Balanced potential (7): descending greedy allocation followed by up to
  30 best pair swaps. The objective penalizes group mean differences with
  weight 25 and within-group standard-deviation differences with weight 1.
  This is an optimization heuristic, not a guarantee of the global optimum.
  Roster sizes and every athlete's uniqueness are preserved.

Existing enum identifiers (including Manual=4) remain unchanged.
