# Wznowienie — audyt silnika, 24.10.2026

Gałąź feature/qml-competition-config. Nie ruszać main, pracować bez subagentów.

## Bieżące zadanie

Użytkownik zlecił znaleźć i naprawić 10 błędów nie-UI oraz wdrożyć do 3 sensownych optymalizacji async/multithread (można mniej lub zero). Osobne commity. **10 poprawek już wykonano i zapisano**, testy każdego problemu odtworzyły błąd przed zmianami; cały nowy zestaw przeszedł po zmianach.

- f1b9c33 — niezależne regresje: tests/engine-audit-regressions.h, flaga --engine-audit-only i --engine-case=N.
- 7cfef95 — inicjalizacja/reset JumpData: DNS, belka trenera, forma, wskaźniki i wyniki.
- 87b5792 — inicjalizacja/ochrona kontekstu JumpSimulator, czyszczenie skills po odpięciu skoczka, domyślny manipulator tymczasowy; zły kontekst rzuca invalid_argument.
- 9d6c768 — dzielenie przez zero i niewłaściwe konwersje prawdopodobieństwa DSQ przy cesze -7.
- 6e50e1c — nawias we wpływie wysokości lotu: kara od różnicy od ideału, nie od przeskalowanej samej cechy.
- 9b8644b — krótkie wektory wymuszonych not i wartości poza zakresem: dopełnienie -1, limit20, NaN pomijane.
- 6266cb8 — wyższa belka trenera musi dawać ujemną rekompensatę także poniżej95% HS.
- 111d6d0 — zapis skoczni używa właściwej flagi autoPointsForMeter.
- b48fa0d — K169,5 daje1,8 pkt/m zamiast0.
- 090bd61 — normalizacja kątów wiatru modulo360, także ujemnych i wielu obrotów.
- 53a9960 — zerowe odchylenie uciętego normalnego nie zapętla się; daleki ogon próbkowany przez propozycję wykładniczą zamiast praktycznie nieskończonej pętli odrzucania.

Logi: /tmp/sj-engine-before-1.log ... -10.log (1–9 kod1 z oczekiwanym FAIL,10 timeout4s), /tmp/sj-engine-fixed.log (wszystkie10 PASS). Tymczasowy skrypt /tmp/sj-engine-fixes.py tylko do częściowego stagingu — wszystkie poprawki już są w Git, czysty working tree w chwili zapisania checkpointu. Nie trzeba go wznawiać.

## Do zrobienia, kolejność obowiązkowa

1. Ocenić/wdrożyć sensowną równoległość. Kandydat: duże niezależne partie SingleJumpsManager, nie sekwencyjne konkursy. Potrzebne: lokalny RNG (obecny MyRandom ma globalny mt19937), kopie skoczka/skoczni/reguł per worker, snapshot ustawień globalnych (JumpSimulator/Hill odczytują singleton), brak QWidget w workerze, bezpieczne anulowanie i usunięcie wskaźników do lokalnych workerów. Zmierzyć przed/po i zachować tylko przy sensownej korzyści. Nie ma obowiązku robić3. Kod optymalizacji ukończono później w a99df69; patrz aktualizacja na końcu.
2. Uzupełnić test losowania dalekiego ogona (średnia z phi(alpha)/Q(alpha)-alpha i terminacja), dodać engine audit do normalnego pełnego zestawu, uruchomić pełne regresje/Widgets oraz testy właściwe optymalizacji.
3. Zrobić szczegółowy raport10 błędów, dowody i decyzje o optymalizacji, release notes vsmain.
4. **Na końcu zmienić wersję na dokładnie 2.0.0-beta**, osobny commit (użytkownik to dopisał). Źródło source/UI/mainwindow.cpp const appVersion; zaktualizować związane testy i dokumentację. Nie zmieniać jeszcze przed zakończeniem punktów1–3.
5. **Po znaczniku wersji**: przycisk „Dodaj konkurs w kalendarzu” z Przeglądu ma naprawdę dodawać konkurs, zaznaczyć go i przejść do edycji, a nie tylko przechodzić do sekcji. Osobny commit.
6. **Absolutnie na końcu, po powyższym przycisku**: poprawić straty przed skokiem, przynajmniej w KO, również zwykły konkurs. Użytkownik odwołał wcześniejszą decyzję o porównywaniu do aktualnej sumy lidera. Teraz porównywać punkty po wagach NA TYM SAMYM ETAPIE: bez dodatkowego skoku lidera z bieżącej serii. Dotyczy obu strat (cały konkurs i grupa). Obecny QuickRaceStats::currentGap używa pointsForJumps(jumps.size(),round) i daje np180 zamiast10; trzeba uciąć do skoków z poprzednich serii, z bieżącymi wagami. Zachować ukrywanie po pełnym resecie, minus jako przewagę, frozen live context. Dostosować tests/simulation-onboarding-regressions.h i multiKO, które obecnie testują dawną decyzję.
7. Commity, inventory scripts/update-release-inventory.py, czysty workingtree, finalny raport. User przy poprzednich wiadomościach pytał o render — wyjaśniono, że to zrzuty testowe, a nie FPS.

Środowisko Qt5.15.3, scripts/test-quick-ui.sh (pełny~4min), --engine-audit-only (szybki), --single-jumps-only, --single-jumps-benchmark, --ko-multi-round-only, --round-points-bulk-only. W build-quick-ui nie uruchamiać równoległych buildów. Widgets: make-j2 w build-widgets-compat. Testy bez buildu przez env LD_LIBRARY_PATH=.../build-qml-preview/qt-runtime/lib QML2_IMPORT_PATH=.../build-qml-preview/qt-runtime/qml QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software build-quick-ui/sj-sim-ui-smoke FLAG. Nie nadpisywać danych użytkownika ani nie zamykać jego gry.


## Aktualizacja po zakończeniu części silnika

Optymalizacja `a99df69` ukończona: duże partie >=4096 skoków, do4 pracowników, snapshot globalnych ustawień w TLS, lokalne RNG, anulowanie i odłączone wyniki. Izolowany benchmark20k:359ms1worker/236ms4workers, ok34% krócej. Nie wdrażano pozostałych2 kandydatów, ponieważ konkurs jest sekwencyjny, a pojedyncze sektory/nota to zbyt małe zadania.

Przeszły pełne regresje391renderów (/tmp/sj-engine-full.log), single-jumps-only, async-benchmark-only z testem zniszczenia właściciela i izolacji ustawień oraz Widgets. Raport: ENGINE_AUDIT_2026-10-24.md.

**Następna czynność: wersja2.0.0-beta**, potem realne dodawanie konkursu z Przeglądu, a na samym końcu poprawka strat na równym etapie. Te trzy dopisane zadania jeszcze nie są wykonane w tym checkpointcie. Po nich finalne testy i aktualizacja release inventory. Pozostać na feature, bez subagentów.
