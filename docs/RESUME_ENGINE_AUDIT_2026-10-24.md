# Punkt kontrolny — silnik, 2.0.0-beta i kontrola pamięci

Gałąź **feature/qml-competition-config**. Nie modyfikować main. Pracować bez subagentów. Użytkownik polecił teraz zrobić commit i push wszystkich zmian, a potem kontynuować diagnostykę.

## Ukończony kod

- Dziesięć błędów nie-UI odtworzonych przed zmianami i naprawionych w dziesięciu osobnych commitach: 7cfef95,87b5792,9d6c768,6e50e1c,9b8644b,6266cb8,111d6d0,b48fa0d,090bd61,53a9960. Szczegóły w ENGINE_AUDIT_2026-10-24.md.
- Jedna uzasadniona optymalizacja async: a99df69, duże niezależne partie >=4096 skoków, do4 pracowników. Snapshot ustawień w TLS, własne RNG, bezpieczne anulowanie i odłączone wyniki. Mediana20k:359ms1worker/236ms4workers (~34% krócej). Pozostałych2 kandydatów świadomie nie wdrażano.
- b04f612: dokładny znacznik **2.0.0-beta**. VersionBadge rozdziela numer i beta; wspólna wartość jest w source/UI/mainwindow.cpp.
- 0fc093d: przycisk „Dodaj konkurs w kalendarzu” z przeglądu faktycznie dodaje, zaznacza i otwiera edycję konkursu.
- Ostatnia korekta strat: QuickRaceStats::roundStartGap porównuje punkty tylko po zakończonych seriach, z wagami bieżącej, bez dodatkowego skoku lidera. Dotyczy zwykłego konkursu, KO i drużyn; regresje obejmują również trzecią serię. To nadpisuje wcześniejszą decyzję o uwzględnianiu aktualnego skoku lidera.

## Weryfikacja i dodatkowa znaleziona awaria

- /tmp/sj-engine-before-1.log ... -10.log: przypadki1–9 FAIL przed poprawką,10 timeout4s.
- /tmp/sj-engine-fixed.log: wszystkie10 PASS.
- /tmp/sj-engine-async-isolated.log: PASS dla snapshotów ustawień, anulowania, życia właściciela, serializacji i unikalnych ID.
- /tmp/sj-engine-full.log: pełny zestaw391renderów, kod0 przed ostatnimi dopiskami.
- /tmp/sj-final-full-gdb.log: pełny końcowy zestaw391renderów zakończony normalnie pod gdb.
- /tmp/sj-final-gaps-multiko.log: rzeczywiste wieloseryjneKO z obiema stratami, kod0.
- /tmp/sj-final-2-beta-widgets.log: aktualny build Widgets, kod0.
- /tmp/sj-final-cache-and-gaps.log: engine audit, nowa regresja tłumaczeń i onboarding/dodawanie konkursu PASS po ostatniej poprawce.

Pełny zwykły smoke raz zakończył się SIGSEGV po authorRegressions; powtórzenie pod gdb przeszło. Uruchomiono ASAN w osobnym ignorowanym **build-quick-asan** i on znalazł konkretny heap-buffer-overflow:

`QuickUi.cpp:212 -> QuickRoster::invalidateText -> refresh -> visiblePool -> Jumper::getCountryCode`.

Zmiana języka odświeżała ukryte adaptery mające wskaźniki do zastąpionej bazy skoczków. Błąd zapisany w /tmp/sj-asan-full.64349, linia2806 i dalej, pełny log aplikacji /tmp/sj-final-asan.log. Naprawa **a72a6b5**: invalidateText emituje changed bez dereferencji danych zawodników (gotowe wiersze są mapami wartości; zmianę danych nadal wykonuje sync). Regresja tests/roster-translation-regressions.h usuwa źródłowego zawodnika przed tłumaczeniem i przeszła.

## Pozostałe — kontynuować po pushu

1. Ponowić **pełny ASAN po a72a6b5**. Build jest już gotowy i aktualny, ale pierwsza próba startu po poprawce zakończyła się od razu samym DEADLYSIGNAL bez raportu, zanim zaczęły się testy. Takie starty występowały też przed poprawką; nie mylić z raportem właściwego heap-buffer-overflow. Próba z verbosity=1 wcześniej pozwoliła przejść cały przebieg aż do rzeczywistego błędu. Można użyć gdb z ASAN_OPTIONS=detect_leaks=0:handle_segv=0, jeśli startup znowu się wywróci.
2. Udany start: env ASAN_OPTIONS=detect_leaks=0:halt_on_error=1:verbosity=1:log_path=/tmp/sj-asan-fixed LD_LIBRARY_PATH=$PWD/build-qml-preview/qt-runtime/lib QML2_IMPORT_PATH=$PWD/build-qml-preview/qt-runtime/qml QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software $PWD/build-quick-asan/sj-sim-ui-smoke > /tmp/sj-final-asan-fixed.log 2>&1. Właściwy raport ASAN jest w pliku log_path.PID.
3. Katalog ASAN ma symlinki userData/flags/translations do build-quick-ui, a sam test kopiuje je do QTemporaryDir. Nie nadpisuje danych użytkownika.
4. Jeśli potrzebny rebuild ASAN: LD_LIBRARY_PATH=$PWD/build-qml-preview/qt-runtime/lib make -C build-quick-asan -j3. To ważne również przy linkowaniu; bez LD_LIBRARY_PATH mieszał systemowe QuickTemplates z runtime i link nie przechodził. Nie uruchamiać równoczesnych buildów w jednym katalogu.
5. Po potwierdzeniu braku zgłoszenia: końcowy zwykły smoke albo przynajmniej istotne regresje, dokumentacja wyniku ASAN, aktualizacja release inventory, commit i push dalszych zmian na tę samą gałąź. Nie ogłaszać jeszcze zakończenia diagnostyki.

MCP QML Preview brak. Lokalnie Linux/Qt5.15.3, offscreen/software, Windows/Qt6 i ThreadSanitizer nie były sprawdzane. Nie zamykać gry użytkownika ani nie ruszać jego userData. Wszystkie wymagane zmiany funkcjonalne są już gotowe; otwarta jest końcowa weryfikacja znalezionej awarii.
