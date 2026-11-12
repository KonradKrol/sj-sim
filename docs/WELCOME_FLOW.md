# Welcome flow

Four-step welcome replaces the original example-season popup. It is shown on
first launch and can be reopened under Settings → Database and tools → Show welcome.
The first step opens the existing author page; closing it returns to the welcome.
Next/back preserve navigation, while Go to menu dismisses the flow.

Screenshots are bundled as optimized lossless PNGs in
`source/UI/Quick/assets/welcome/`. The temporary third/fourth filenames were swapped
as requested: fourth → competition, third → calendar. Images preserve their whole
frame at desktop and laptop sizes. Text scrolls independently when necessary.

The final action creates a uniquely named, translated Example simulation with
Summer (LGP26 / Official-LGP) and Winter (WC26-27 / Official-WC) calendars, using
active packs. Summer is active initially. Hills are matched to the selected pack
by name and HS, with nearest-HS fallback. The existing date-disabled example
behavior is preserved. The new save is persisted before opening.

Verified four steps at 1050×720 and 640×480, and real creation/save of the example
with 83 athletes and both calendars (19 summer and 108 winter events in the local
presets). English and Czech welcome, settings action and example names translated.
