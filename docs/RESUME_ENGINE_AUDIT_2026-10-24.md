# Stan końcowy — audyt silnika i 2.0.0-beta

Zlecenie zakończone. Gałąź **feature/qml-competition-config**, wersja **2.0.0-beta**. Main niezmieniony: `05f2e48d8e92c7b9b180f18abe250b3c7e61d24c`. Pracowano bez subagentów.

## Zapisane zmiany

- Dziesięć błędów silnika, każdy z odtworzeniem i osobnym commitem: `7cfef95`, `87b5792`, `9d6c768`, `6e50e1c`, `9b8644b`, `6266cb8`, `111d6d0`, `b48fa0d`, `090bd61`, `53a9960`.
- `a99df69`: jedna uzasadniona optymalizacja async dużych niezależnych partii od4096 skoków, do4 pracowników. Snapshoty ustawień, własne RNG, anulowanie, odłączone wyniki i bezpieczny cykl życia. Izolowany pomiar20k:359ms1worker/236ms4workers (~34% krócej). Pozostałych2 kandydatów świadomie nie wdrażano.
- `b04f612`: znacznik2.0.0-beta.
- `0fc093d`: przycisk przeglądu faktycznie dodaje konkurs, zaznacza i otwiera edycję.
- `a72a6b5`: dodatkowy błąd wykryty przez ASAN — tłumaczenie ukrytych list odczytywało zastąpione dane skoczków. Powiadomienie tłumaczeń korzysta teraz z map wartości.
- `5af8f1b`: ostatnia korekta strat — punkty zakończonych serii po wagach bieżącej, bez dodatkowego skoku lidera. Zwykły konkurs, KO, drużyny i trzecia seria objęte regresjami. To zastępuje wcześniejszą decyzję o użyciu aktualnej sumy lidera.

Szczegóły: [ENGINE_AUDIT_2026-10-24.md](ENGINE_AUDIT_2026-10-24.md). Cały release względem main: [RELEASE_NOTES_VS_MAIN.md](RELEASE_NOTES_VS_MAIN.md).

## Końcowe testy

- Pełny zwykły smoke: kod0,391zrzutów, `/tmp/sj-final-normal-fixed.log`.
- Pełny ASAN pod gdb: normalne zakończenie,391zrzutów, brak raportu pamięci, `/tmp/sj-final-asan-fixed-gdb.log`, `/tmp/sj-asan-fixed-gdb.65769`.
- Async i benchmark: `/tmp/sj-engine-async-isolated.log`.
- Nowa regresja tłumaczeń i onboarding/dodawanie: `/tmp/sj-final-cache-and-gaps.log`.
- WieloseryjneKO: `/tmp/sj-final-gaps-multiko.log`.
- Single jumps/cancel/history: `/tmp/sj-engine-single-final.log`.
- Widgets2.0.0-beta: `/tmp/sj-final-2-beta-widgets.log`.
- Wszystkie10reproduktorów przed zmianą: `/tmp/sj-engine-before-1.log` ... `-10.log`;1–9oczekiwanyFAIL,10timeout4s.

ASAN: build-quick-asan, osobny ignorowany katalog. Leak checker wyłączony; ThreadSanitizera i natywnego Windows/Qt6 nie uruchamiano. Sporadyczny zwykły SIGSEGV doprowadził do znalezienia powyższego błędu list; po poprawce końcowe zwykłe i ASAN przeszły. Start ASAN czasami kończył się samym DEADLYSIGNAL przed testami; skuteczny pełny przebieg wykonano pod gdb.

## Remote i ewentualna dalsza praca

Wszystkie zmiany wypchnięto na **origin/feature/qml-competition-config**. Push HTTPS nie miał danych logowania; SSH z istniejącym kluczem działa. Użyto tymczasowego `git -c remote.origin.pushurl=git@github.com:KonradKrol/sj-sim.git push origin HEAD:feature/qml-competition-config`, bez zmiany fetch URL. Użytkownik autoryzował commit i push.

Nie ma pozostałego otwartego punktu tego zlecenia. Kontynuować dopiero nowe zadania/zgłoszenia użytkownika, nadal bez delegowania i bez ruszania main. Nie zamykać gry użytkownika ani nie nadpisywać jego userData. Zrzuty testowe nie są miarą FPS.
