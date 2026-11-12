# Season performance — 12 November 2026

Simple optimizations, without changing persistence format, introducing a database,
or deferring ownership of loaded save objects.

## Measurements

Release/O2 Linux build, same machine, 17 MB Sezonik save, three runs per main
benchmark. Times include real deserialization and native competition processing.

| Work | Before | After | Reduction |
|---|---:|---:|---:|
| Save JSON parsing + reconstruction | 2.01–2.06 s | 0.59–0.65 s | about 70% |
| Full auto competition with auto gate, 83 entrants / about 113–118 jumps | 18.66–19.51 s | 3.83–3.91 s | about 80% |
| 500 scans of the 71-row archive list | 32.7 ms | 2.9 ms | about 91% |
| Strongest-athlete gate preview, 83 athletes | 64–159 ms | 7–17 ms | about 89% |

The archive measurement isolates list/model work, not total page loading. A cold
archive details construction varied from roughly 23 to 83 ms; no general
60–80% improvement to every screen or first QML frame is claimed. JSON parsing
alone still costs around 130–170 ms on this save. Very different saves, profiles,
weather and hardware will change timings. Auto competitions can have different
jump counts because ties are allowed; timings above are approximate.

## Changes

- Cache successful balance-profile validation: frozen events repeatedly carry
  identical coefficients. A bounded 32-entry thread-local cache compares complete
  JSON objects. Edits are revalidated; failed validations are not cached. Parameter
  definition ranges are constructed once. No validation is removed.
- Classification batch recomputation calculates each event's contributions once,
  retaining row-specific event membership, addition order and eligibility rules.
  Cache lifetime is one recalculation. Used during save reconstruction and after
  season competitions.
- Auto gate evaluates baseline quality for all entrants and flies only the strongest
  candidate per specialization. A flight with the same specialization and weather
  is monotone in starting quality. All specialization groups are still compared.
  The complete first-round field remains the basis for recommendations.
- Hidden to-beat / to-advance distances are not recomputed after each batch jump.
  Normal calculations resume when the batch finishes; rankings and advancement
  decisions continue updating during the batch.
- Archive model indexes played events once on reset. Count and row lookup are
  constant-time. Display labels are cached and rebuilt on reset or language change.
  Existing season/archive synchronization resets the model after event changes.

## Verification and reproduction

Build normally, then on Linux/qmake:

```sh
python3 scripts/benchmark-season-performance.py build-quick-ui/simulationSaves/Sezonik.json
```

The checks do not save the loaded season. They verify 257 real classification
scores and eligibility values against the original per-row calculation, edited
profile revalidation, archive ordering and refresh, and equal maximum-distance
predictions for all 83 athletes versus grouped previews at four gates in calm,
headwind and tailwind scenarios with spatial variation and daily form.

Additional existing UI checks passed: `--race-auto-gate-only` and
`--super-team-only` (completed third automatic Super Team round).
The broader `--bulk-only` suite stops before batch tests at an existing stale
calendar assertion: it expects an invalid date to be rejected, although the
current editor intentionally retains invalid draft dates and blocks confirmation.
That unrelated assertion was not altered to hide the failure.
