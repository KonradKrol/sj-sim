# Tworzenie symulacji, puste ekrany i prezentacja konkursu — 24.10.2026

Gałąź `feature/qml-competition-config`, bez zmiany main i bez zmiany numeru **1.3.0 beta-3**. Użytkownik wycofał nowe zgłoszenie o ograniczonej wysokości konfiguracji konkursu jako nieaktualne. Pozostałe punkty są zakresem tej partii.

## Tworzenie nowej symulacji

- Jeden dedykowany ekran Qt Quick: nazwa symulacji, pierwszy sezon oraz paczki skoczków, skoczni i zasad.
- Trzy karty wyboru są obok siebie na laptopie/desktopie, a na wąskim oknie układają się pionowo. Przycisk tworzenia pozostaje poza przewijanym obszarem.
- Zniknął dodatkowy etap edycji skoczków, skoczni i zasad w konfiguratorze sezonu. Sam konfigurator i komponenty Widgets pozostają w projekcie, m.in. do konfiguracji kolejnego sezonu.
- Wybrana paczka jest kopiowana do nowego zapisu. Wybór nie przełącza globalnie aktywnej bazy ani nie edytuje źródłowej paczki.
- Przy każdym wyborze widać liczebność paczki. Puste paczki blokują tworzenie niekompletnej symulacji; zachowana jest walidacja nazwy.
- Dostępny jest przełącznik dat zawodów i opcja zapamiętania trzech wyborów paczek. Preferencje zapisują się dopiero po zatwierdzeniu tworzenia; anulowanie ich nie zmienia. Wyłączenie zapamiętywania usuwa zapisane wybory.
- Po utworzeniu zapis startuje bez ukrytego kalendarza, z wyraźnym kolejnym krokiem w przeglądzie.

## Puste ekrany i przejście do kalendarza

Wspólny komponent pustego stanu pokazuje krótki tytuł, powód braku danych i konkretną następną czynność. Przy bardzo niskich listach priorytet ma przycisk działania, bez obcinania go pod opisem. Stany list są nałożone na widoczny viewport, a nie na pusty obszar przewijanej zawartości.

Objęte miejsca:

1. Przegląd symulacji bez kalendarzy — utworzenie pierwszego kalendarza.
2. Przegląd z kalendarzami, ale bez aktywnego — wskazanie wyboru i aktywowania.
3. Przegląd z pustym aktywnym kalendarzem — dodanie konkursów lub presetu.
4. Sekcja kalendarzy bez kalendarza — wyjaśnienie jego roli i bezpośrednie tworzenie.
5. Kalendarz bez konkursów — bezpośrednie dodanie pierwszego wydarzenia. Zbędne narzędzia zaznaczania, przenoszenia, kopiowania i dat nie zajmują wtedy miejsca.
6. Pusta lista klasyfikacji — wyjaśnienie przypisywania konkursów i przycisk dodania.
7. Wyniki konkursu przed pierwszym skokiem — informacja, kiedy się pojawią.
8. Tabela po wyszukiwaniu bez dopasowań — wyjaśnienie i czyszczenie filtra.
9. Pusta lista startowa — wskazanie wyboru zawodników lub zapisanego składu.
10. Brak dopasowań w bibliotece zawodników — zmiana lub czyszczenie filtrów.
11. Pozostałe puste tabele — informacja o danych/wynikach, które pojawią się po działaniu użytkownika.
12. Przegląd bez wyników klasyfikacji — wskazanie dodania klasyfikacji i przypisania wydarzeń.

Przycisk „Utwórz kalendarz” w przeglądzie od razu tworzy kalendarz, zaznacza go i otwiera sekcję kalendarza z edycją nazwy. Przejście działa również wtedy, gdy ukryta karta Qt Widgets już była ustawiona na Kalendarz — dawniej nie emitowała ponownie zmiany indeksu, więc Quick pozostawał w Przeglądzie. Ta sama droga działa przy przejściu z przeglądu do dodawania konkursów w istniejącym pustym kalendarzu.

Przycisk dodawania kalendarza jest opisany również w wąskim układzie. Przy jednym pustym aktywnym kalendarzu nie zajmują miejsca zbędny dropdown wyboru ani wyłączony przycisk jego aktywowania.

## Karta zawodnika i KO

- Strata/przewaga przed skokiem jest liczona z punktów przeliczonych według wag bieżącej serii.
- Porównanie dotyczy aktualnego wyniku najlepszego rywala, w tym skoków oddanych już w tej serii. Przewaga ma znak minus.
- W KO osobno pokazany jest wynik względem lidera całego konkursu i najlepszego rywala w grupie.
- Pierwsza seria i seria z całkowitym resetem wcześniejszych punktów nie pokazują tych odziedziczonych strat. Miejsce po poprzedniej serii może nadal być widoczne; nie jest mylone ze stratą po przeliczeniu.
- Podczas animacji na żywo wartości sprzed skoku pozostają zamrożone do ujawnienia wyniku.
- Kliknięcie wyniku w tabeli KO otwiera kartę właściwego zawodnika. Wiersz grupy jest mapowany przez jej posortowanych uczestników, a nie błędnie używany jako indeks tabeli całego konkursu. Bez wyniku nie otwiera pustej karty; podczas ujawniania na żywo nie omija blokady.
- Pojedyncze kliknięcie w tabeli głównej korzysta z tego samego zachowania podglądu.

## Oznaczenia i podium

- Usunięto znacznik ✓ awansu. Łagodna zieleń i tekst dostępności nadal przekazują stan.
- Q w kalendarzu przesunięto na lewo, obok flagi i nazwy wydarzenia.
- Podium pojawia się dopiero po zakończeniu całego konkursu. Zakończenie pośredniej serii nie uruchamia kolorów podium.
- Kolor zależy od miejsca w łącznej klasyfikacji, nie indeksu wiersza. Przy ex aequo 1,1,3 obaj pierwsi mają złoto, a trzeci brąz; nie tworzy się sztucznego srebrnego miejsca.
- Odwołanie serii kończące zawody uznawane jest za koniec konkursu i zachowuje podium po poprzedniej zakończonej serii. Odwołanie pierwszej serii nie tworzy podium z pustych wyników.
- Licznik serii po odwołaniu nie pokazuje niemożliwego „2 / 1”.

## Regresje

`tests/simulation-onboarding-regressions.h` sprawdza wagi, bieżące wyniki rywali, niezależną przewagę KO, reset, brak podium w trakcie i po pośredniej serii, ex aequo oraz odwołaną serię kończącą konkurs. Sprawdza też trzy paczki, liczebność, preferencje, niezależność globalnej bazy, daty, utworzenie kalendarza z przeglądu i widoczność działania pustego kalendarza.

Pełny smoke obejmuje ponadto kliknięcie wyniku KO, zamrożenie strat podczas skoku na żywo, wcześniejsze regresje kalendarza i zestaw renderów. Wyniki:

- Pełny `scripts/test-quick-ui.sh`: **kod 0**, 391 testowych zrzutów interfejsu, `/tmp/sj-onboarding-final.log`.
- Końcowy `--onboarding-only`: **kod 0**, `/tmp/sj-onboarding-last.log`.
- Końcowy `--workflow-only`: **kod 0**, `/tmp/sj-onboarding-workflow.log`; obejmuje również wcześniejsze narzędzia sezonowe i edycję kalendarza.
- `--ko-multi-round-only`: **kod 0**, `/tmp/sj-multiko.log`; rzeczywisty dwuseryjny konkurs KO z wagą 0,5, obiema stratami oraz podglądem skoku z grupy.
- `--round-points-bulk-only`: **kod 0**, `/tmp/sj-onboarding-weighted.log`; konkurs indywidualny, KO i drużynowy, wagi/reset i odwołanie serii.
- Aktualny wariant Widgets-only: `make -j2`, **kod 0**, `/tmp/sj-widgets-onboarding-last.log`.
- `make -q` dla Widgets oraz `make -q -f Makefile.smoke` dla Quick: **kod 0**, binaria aktualne.
- Angielskie tłumaczenia nowych tekstów skompilowane przez `lrelease`; `git diff --check` bez błędów.

Weryfikacja lokalna: Linux, Qt 5.15.3, offscreen/software. Zrzuty nie są pomiarem FPS. Natywne Windows/Qt6 i GPU nie były sprawdzane w tej partii. Przy szybkich zmianach testowych rozmiarów nadal pojawiają się komunikaty Qt5 o negocjacji układu, a w starszych widokach komunikaty null pixmap; końcowe testy wymiarów i interakcji przeszły. Nie jest to deklaracja usunięcia wszystkich ostrzeżeń aplikacji.

Commity implementacji: `3295a84` (prezentacja konkursu), `63c7662` (tworzenie symulacji), `faeba46` (puste ekrany i nawigacja), `17bd91b` (regresje).
