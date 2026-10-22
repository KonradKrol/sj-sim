# Autor, dostępność i pojedyncze skoki — 22.10.2026

Gałąź `feature/qml-competition-config`; `main` pozostaje przy `05f2e48`. Numer w aplikacji nadal **1.3.0 beta-3**.

## Zmiany do sprawdzenia przez użytkownika

1. **Menu główne → O autorze.** Nowa strona we wspólnej nawigacji: zdjęcie Konrada, biografia, zaproszenie do współpracy i dwa projekty. Predazzo Edition jest pierwsze i wyróżnione; Ski Jump Draft drugie, z opisem drużyny gracza i testów beta w 2025 roku. Linki prowadzą do wskazanych repozytoriów, GitHuba i LinkedIna. Ikony SVG mają wariant jasny i ciemny. Nie ma ostatniego zdania o SJ Manager/SJ Draft.
2. **Małe okno autora.** Całe zdjęcie bez przycinania jest obok powitania, w ramce 84×112 px albo 120×160 px. Biografia i projekty poniżej korzystają z pełnej szerokości. Na szerokim ekranie zachowany jest większy portret obok treści. Dostarczono zoptymalizowaną kopię zdjęcia; oryginał użytkownika pozostaje bez zmian.
3. **Podziękowania.** Widoczny przycisk pod informacjami autora otwiera osobny dialog z zachowaną listą współtwórców. Zamknięcie wraca do strony autora; lista nie rozwija się już wewnątrz biografii.
4. **Menu główne.** Usunięto niezrozumiałe oznaczenie „01 — 03”.
5. **Wygląd i dostępność.** Dodano opcję „Kompaktowy interfejs”, która zmniejsza kontrolki, wiersze i odstępy niezależnie od rozmiaru tekstu. Dostępne wielkości tekstu: 75%, 85%, 100%, 115%, 130%. Oba ustawienia są zapamiętywane. Panel ustawień mieści się w małym oknie i przewija własną zawartość, kiedy jest to potrzebne.
6. **Zasady → dodawanie serii.** Każda nowa seria ma domyślnie zaznaczone sortowanie listy startowej. Wcześniejsze serie zachowują własny wybór. Usunięto błędne przypisywanie ustawień KO i rozmiaru grupy do flagi sortowania.
7. **Konkurs → opcje.** Po przejściu ze „Skoku” do mieszczących się zakładek „Przebieg” i „Warunki” znika przewijanie i zeruje się przesunięcie. Nieaktywny Loader nie zachowuje już wysokości poprzedniej zawartości.
8. **Automatyczna symulacja konkursu.** Trzy osobne przyciski „N skoków”, „Seria”, „Konkurs” zastępują nadrzędne menu. W drużynówce pozostaje „Grupa”. Wybór zamyka panel opcji, aby odsłonić przebieg konkursu. Anulowanie pytania lub wyboru liczby skoków nie wykonuje symulacji.
9. **Pojedyncze skoki → konfiguracja.** Przycisk „Dobierz belkę” wylicza konkretną liczbę na podstawie aktualnego zawodnika, skoczni i bazowego wiatru. Używa deterministycznego podglądu odległości z celem 97% HS. Nie zastępuje ręcznej belki przed kliknięciem. Niekompletne dane zachowują poprzednią wartość. Działa także w Widgets.
10. **Pojedyncze skoki → wyniki.** Tabela wypełnia dostępną wysokość, bez wcześniejszego ogólnego limitu około 540 px i bez przewijania całego ekranu. Szerokie okno pokazuje szczegóły/statystyki obok wyników; wąskie oferuje „Wyniki” i „Szczegóły i statystyki”. Wszystkie sześć kategorii szczegółów/wykresów jest dostępnych z selektora. Zachowano wybór skoku, sortowanie, wyszukiwanie i dotychczasowe akcje. Uporządkowano szczegóły, usunięto nieadekwatne zerowe miejsca z trybu bez konkursu i zrównoważono szerokości kolumn liczbowych.
11. **Wydajność pojedynczych skoków.** Trzy optymalizacje opisano poniżej. Postęp i przerwanie nadal działają, z zachowaniem częściowych wyników.
12. **Domyślna ważność skoku.** `SingleJumpsManager` inicjalizuje ważność na 5. Wcześniej użycie menedżera bez jawnego ustawienia tego pola odczytywało niezainicjalizowaną liczbę. Jawne ustawienia z konfiguratora zachowują pierwszeństwo.

## Trzy optymalizacje

- `a52c96f`: aktualizacja etykiety i wartości postępu jest ograniczona do około 30 razy na sekundę, zamiast wykonywania jej przy każdym skoku. Modalny `QProgressDialog::setValue()` sam obsługuje zdarzenia; usunięto dodatkowe pompowanie kolejki w każdej iteracji. Sprawdzenie anulowania pozostaje w pętli.
- `c9b2682`: ponowne użycie standardowego rozkładu normalnego zachowuje jego drugi, już wylosowany wynik. Dla rozkładu normalnego o średniej zero losowanie połówkowe korzysta z odbicia próbki zamiast odrzucania około połowy losowań. Dla średniej różnej od zera zachowano odrzucanie, żeby nie zmienić rozkładu. Nie zmieniono współczynników silnika; kolejność zużycia losowych próbek jest inna, więc nie jest to gwarancja identycznej sekwencji wyników.
- `bb4f7a6`: konfiguracja generatora wiatru, stałe wiatry i pojemność buforów są przygotowywane raz na partię. Odczyt niezmiennych ustawień wiatru nie odłącza współdzielonego `QVector`. Zmienny wiatr nadal powstaje osobno dla każdego skoku, a zapisane wiatry wcześniejszych skoków pozostają niezależne. Manipulator jest obiektem lokalnym, a bazowa belka symulatora jest jawnie inicjalizowana.

Największy zmierzony zysk daje ograniczenie aktualizacji postępu. Pozostałe dwie zmiany zmniejszają pracę silnika i alokacje; nie przedstawiamy ich jako trzech osobnych wielokrotnych przyspieszeń.

## Pomiary

Qt 5.15.3, Release `-O2`, Linux, `QT_QPA_PLATFORM=offscreen`, `QT_QUICK_BACKEND=software`. Test obejmuje rzeczywisty `SingleJumpsManager::simulate()` z dialogiem postępu, 1000 skoków tego samego zawodnika, sześć sektorów, oraz osobno budowę i pokazanie wyników.

| Etap | Stały wiatr przed | Stały wiatr po | Zmienny wiatr przed | Zmienny wiatr po |
| --- | ---: | ---: | ---: | ---: |
| Sama symulacja z postępem | 170 ms | **36 ms** | 173 ms | **38 ms** |
| Przygotowanie wyników | 49 ms | 49 ms | 20 ms | 20 ms |
| Pokazanie wyników | 168 ms | 126 ms | 135 ms | 99 ms |

Końcowe wartości to mediany trzech prób. Sama symulacja przyspieszyła około **4,6–4,7×**. Po samej zmianie postępu wcześniejszy pomiar wynosił 39/41 ms; odrębnego zysku dwóch pozostałych optymalizacji nie izolowano. Przygotowanie i pokazanie wyników są dodatkowymi kosztami. Log bazowy: `/tmp/sj-single-before.log`; końcowe próby: `/tmp/sj-single-final-median-0.log` do `-2.log`.

Nie odtworzono lokalnie zgłoszonych około 13 sekund na 1000 skoków. Środowisko offscreen nie odtwarza kosztu sterownika/GPU i prezentacji okien na komputerze użytkownika. Pomiar nie jest gwarancją takiego samego czasu na każdym komputerze.

## Weryfikacja

- `--author-only`: nawigacja, pełny portret na małym ekranie, oba motywy, duży tekst, kliknięcia projektów/profili i dialog podziękowań. Linki przechwytuje test bez uruchamiania przeglądarki.
- `--appearance-only`: niezależne zmniejszenie kontrolek, zapis ustawień oraz panel dostępności w 360×480.
- `--backend-only`: nowe serie, zachowanie poprzednich ustawień, domyślna ważność, rozkłady normalne i wcześniejsze regresje.
- `--ux-only`: rzeczywisty konkurs, zmiana zakładek po skoku, trzy bezpośrednie operacje i anulowanie, ukończenie zawodów.
- `--single-jumps-only`: przerwanie dużej partii z częściowymi wynikami, poprawność wiatrów i ich historii, dobór belki, wybór skoku, układ wyników do 360×480 w obu motywach, większy tekst, wzrost tabeli przy wysokościach 700/1000/1400.
- `--single-jumps-benchmark`: powtarzalny pomiar bez twardego limitu czasowego w teście.
- Pełny `scripts/test-quick-ui.sh`: kod 0, **367 responsywnych renderów**, `/tmp/sj-author-single-final-full.log`. Po ostatnim przestawieniu portretu ponowiono `--author-only`: kod 0, `/tmp/sj-author-header-final.log`.
- Widgets-only: `make -j2` w `build-widgets-compat`, kod 0, `/tmp/sj-author-single-final-widgets.log`.
- Brak `FAIL`, `TypeError`, `ReferenceError` i pętli powiązań w tych końcowych testach. Pozostają wcześniejsze ostrzeżenia kodu i wtyczki offscreen.

## Powiązane commity

- Autor: `6910a70`, `04f78b6`; pełny mały portret `4c4ba9c`; końcowy układ portretu obok powitania `7eaf15d`.
- Utrzymujące się przewijanie: `bf523d6`.
- Domyślne sortowanie nowych serii: `4490b95`.
- Usunięcie numeracji menu: `208fce0`.
- Mniejszy interfejs: `131f87e`.
- Trzy bezpośrednie operacje symulacji: `f945bd2`.
- Belka w pojedynczych skokach: `4cae942`.
- Pełna wysokość wyników: `1ee819b`.
- Trzy optymalizacje: `a52c96f`, `c9b2682`, `bb4f7a6`.
- Inicjalizacja ważności: `c4b9212`.
- Nowe tłumaczenia: `0b81ec2`.
