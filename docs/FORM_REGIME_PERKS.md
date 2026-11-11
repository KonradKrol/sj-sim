# Form regimes and perks — 11 November 2026

Neutral long-run time shares of regimes 0–6: 2%, 10%, 23%, 30%, 23%, 10%, 2%.
These are latent regime occupancy shares, not a histogram of displayed form.

Owner calibration: formDeviation=13, baseDeviation=25, formHalfLife=13,
baseHalfLife=180. Centers and entry deviations remain in the owner's profile.
The complete shipped defaults are recorded in
calibration/form-defaults-2026-11-11-seven-regimes-perks.json.

## Perks

Signed perk: -1…-4 stable, +1…+4 volatile, 0 neutral.
Noise factors: stable .90/.75/.60/.45, volatile 1.10/1.25/1.40/1.55.
Mean period duration factors: stable 1.10/1.20/1.30/1.40,
volatile .90/.80/.70/.60.
These factors change form noise, not base noise.
Neither side has a direct positive or negative form bonus.

Target time shares are proportional to:
share[i] * exp(concentration[perk] * ((i-3)/3)^2).
Concentration magnitude by tier: .45, 1.05, 1.75, 2.50.
Stable uses negative concentration, volatile positive.
Both sides of the regime axis are treated symmetrically.

## Exact CTMC construction

For target shares p and positive off-diagonal transition weights w:
a[i,j] = sqrt(w[i,j] * w[j,i] / (meanDays[i] * meanDays[j]))
q[i,j] = a[i,j] * sqrt(p[j]/p[i]).
Then p[i]*q[i,j] = p[j]*q[j,i], giving exact detailed balance.
There are no self-transitions; exponential holding times implement a CTMC.

All holding times receive one common scale so that the overall mean time
between actual changes equals:
sum_i(p[i]*meanDays[i]) * duration[perk].
With all baseline times 104 days this is 104 * duration[perk].
Individual regimes have different mean holding times; diagnostics display
their actual means. The overall mean is not an arithmetic average of those
state-specific means.

Config::fromJson upgrades old crisis/peak multiplier profiles to this model,
preserving unrelated tuning. Existing form/base values, RNG states and the
remaining time of an already-started regime are retained. Its next transition
uses the new kernel. New saves use the defaults/local lab profile as usual.

## Verification

tests/form-engine.cpp checks normalization, exact detailed balance, symmetry,
aggregate transition intensity, long-run occupancy using 200,000 sampled
holding times per tested perk, RNG persistence and batch determinism.
Run scripts/test-form-engine.sh.
