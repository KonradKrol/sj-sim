# Jump regularity calibration — 12 November 2026

Normal noise, competition deviation 4, negative influence 0.65, neutral app settings.
Thresholds are relative to deterministic potential: weak <= -5 quality,
large mistake <= -10, excellent >= +5. No wind or other perk effects included.

| Perk | Weak % | Large mistake % | Excellent % |
|---|---:|---:|---:|
| Regular I | 14.5 | 2.1 | 3.2 |
| Regular II | 12 | 1.6 | 2.6 |
| Regular III | 8.75 | 1.35 | 2.1 |
| Regular IV | 5 | 0.9 | 1.5 |
| None | 16.814 | 2.724 | 3.708 |
| Uneven I | unchanged | 3.8 | 4.1 |
| Uneven II | unchanged | 5.6 | 4.5 |
| Uneven III | unchanged | 8 | 5.1 |
| Uneven IV | unchanged | 12 | 5.9 |

For a negative standardized sample z, loss = deviation * 2 * negativeInfluence
* Negative[level] * abs(z)^NegativePower[level]. Positive samples retain their
linear multiplier. Both arrays start with 1 at level zero; no-perk behavior and
RNG draw count are unchanged. Power is applied before the sign multipliers.
Coefficients are fitted using inverse normal tail probabilities. Changing
base deviation, noise distribution or app scales changes threshold rates.
The new power parameters are exposed in the engine/lab parameter registry.
Only these perk coefficients were changed in the owner's active local profile;
other owner tuning was preserved. Existing saved experiment profiles may still
override the multiplier values and need these coefficients reapplied.
