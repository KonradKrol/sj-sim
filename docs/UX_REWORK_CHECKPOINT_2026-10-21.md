# Stan prac: pięć zmian UX — 2026-10-21

**Aktualizacja po wznowieniu:** pięć zmian UX ukończono; dwa końcowe pełne przebiegi przeszły z 316 renderami. [Raport końcowy](UX_IMPROVEMENTS_2026-10-21.md) opisuje poprawkę geometrii KO, wyniki i ograniczenie diagnostyki jednorazowej awarii. Poniżej zachowano historyczny stan sprzed wznowienia.

Przerwano na prośbę użytkownika przed wyłączeniem komputera. Zadanie **nie jest jeszcze zamknięte**.
Gałąź: `feature/qml-competition-config`. Nie zmieniano `main` (05f2e48d8e92c7b9b180f18abe250b3c7e61d24c).

## Zaimplementowane zmiany

1. **Generator formy:** edycja parametrów i wczytanie presetu tworzą szkic, zamiast natychmiast nadpisywać wszystkich zawodników. Jawne „Wszyscy / Zaznaczeni” i „Zastosuj parametry”; generowanie zablokowane przy niezastosowanym szkicu. Przycisk zastosowania przypięty także w małych oknach. Zakres i licznik wszystkich zawodników niezależne od wyszukiwania. Commity: e577e73, f9a0b90.
2. **Tabele danych:** widoczne wyszukiwanie, Ctrl+F, licznik zaznaczenia, czyszczenie zaznaczenia i pusty stan z przyciskiem usunięcia filtra. Zmiana filtra usuwa ukryte cele operacji zbiorczej. Podświetlenie faktycznego zaznaczenia oddzielone od kursora klawiatury; zaznaczona komórka liczy i oznacza zawodnika. Commity: 1611e61, 8708819.
3. **Zapisane listy startowe:** bezpośredni wybór listy i oddzielne „Dodaj / Usuń / Kolejność”, zamiast zagnieżdżonych menu. Na małych ekranach jeden krótki panel; mniej zbędnych nagłówków zostawia miejsce na zawodników. Commity: 0679252, 597e1dd.
4. **Ekran konkursu:** podrzędne opcje w zakładkach „Przebieg / Warunki / Skok”; tryb na żywo dostępny w głównym pasku, odczyty wiatru i śniegu otwierają odpowiednie warunki. Główne wyniki, lista startowa i ostatni skok pozostają poza tymi zakładkami. Commit: 191138e.
5. **Kalendarz:** kliknięcie zaznacza bez przechodzenia do edytora. Jawne „Edytuj”, strzałka, Enter lub dwuklik otwierają edycję. Ctrl/Shift umożliwia zbiorczy wybór, widoczny licznik i czyszczenie zaznaczenia. Zachowanie fokusu po przebudowaniu modelu. Commity: 4e1c614, 77811ae.

## Weryfikacja i pozostała praca

- Testy kierowane `scripts/test-quick-ui.sh --ux-only` przeszły przed ostatnimi dopracowaniami wyszukiwania/podświetlenia i licznika pełnego zakresu. Log: `/tmp/sj-ux-final-focused.log`, kod wyjścia 0.
- Ostatni pełny test (`/tmp/sj-ux-final-full.log`) przeszedł regresje backendu, poprzedni audyt 13 błędów, nowe przepływy UX, nawigację, sezon, listy startowe i geometrię generatora/presetów. **Zakończył się błędem**: `critical tables remain visible together without page scrolling on laptops`. Należy ustalić scenariusz i naprawić regresję układu konkursu. Nie przedstawiać całego zestawu jako zaliczonego.
- Ostatnia drobna zmiana `participantCount` w QuickBridge/FormGeneratorWorkspace oraz jej test nie były jeszcze przebudowane w tym pełnym przebiegu. Potrzebny nowy build i test.
- Kompilacja Widgets-only (`make -j4` w `build-widgets-compat`) przeszła, kod wyjścia 0; log `/tmp/sj-ux-widgets-final.log`.
- Sprawdzono zrzuty generatora parametrów i biblioteki zawodników przy 360 × 480. Zrzuty w `build-quick-ui/ui-renders/ux-*`.
- Nowe testy są w `tests/quick-ui-smoke.cpp`, funkcja `uxRegressions`, dodatkowe asercje renderowania i kontrole opcji w `runCompetition`. Flaga `--ux-only` uruchamia nowe przepływy i konkurs nr 0.
- Pełny zestaw powinien obecnie liczyć 316 renderów zamiast 292, ale **nie jest to jeszcze potwierdzone zaliczonym końcowym przebiegiem**.
- Po wznowieniu: naprawić widoczność tabel konkursu, uruchomić pełny zestaw, przejrzeć zrzuty laptop/desktop, zaktualizować `QUICK_UI_MIGRATION.md` (opis ładowania presetów formy nadal opisuje stare natychmiastowe zastosowanie), dopisać końcowy raport wyników i commity. Sprawdzić tłumaczenia po ostatecznych zmianach.
- Angielskie tłumaczenia nowych kontrolek są zapisane w `translations/translation_en.ts`; aktualizowano tylko konteksty Quick. Pomocniczy skrypt `/tmp/sj-update-ux-translations.py` korzysta z `/tmp/sj-quick-refresh.ts`; przy nowych ciągach najpierw ponownie uruchomić lupdate dla plików Quick.

Qt 5.15.3, qmake, C++17. Użyte skille `.agents/skills/qt-qml/SKILL.md` i `qt-ui-design/SKILL.md`. Bez subagentów. Zachować oryginalną wersję Widgets i eksperymentalny katalog `source/UI/Competition/Quick`. Nowa domyślna implementacja jest w `source/UI/Quick`.
