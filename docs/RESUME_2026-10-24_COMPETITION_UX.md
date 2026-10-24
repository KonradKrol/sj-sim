# Stan końcowy — konkursy i UX, 24.10.2026

Partia zakończona na `feature/qml-competition-config`. Main nadal wskazuje `05f2e48d8e92c7b9b180f18abe250b3c7e61d24c`. Wersja aplikacji pozostaje **1.3.0 beta-3**.

Użytkownik zabronił dalszej pracy subagentów: ich dotychczasowe zmiany zostały przejęte, dalszą integrację i weryfikację wykonano samodzielnie. Nie wznawiać delegowania bez nowego polecenia.

## Wykonane

Zakończone są cztery obszary konkursowe i dziewięć zgłoszonych punktów UX: stany awansu/odpadnięcia i neutralne Q, łagodniejsza zieleń, remisy, belki, przeciąganie z przewijaniem, licznik serii, wpisywanie liczb, scroll kalendarza, aktualizacja konfiguracji, flagi, podpowiedzi, gęstość kart oraz brak wyszukiwania w bieżącej liście i KO.

Ostatnie dopisane zadanie też wykonano: obszar pól edytora konkursu w kalendarzu jest przypięty pod nagłówkiem i wypełnia dostępną wysokość. Poprzedni układ potrafił przydzielić mu wysokość zero, zostawiając pustą kartę. Regresja sprawdza pięć wielkości okna, daty włączone/wyłączone oraz przełączanie konkursów i zakładek.

Szczegółowy opis: [COMPETITION_UX_2026-10-24.md](COMPETITION_UX_2026-10-24.md). Pełny wykaz względem main: [RELEASE_NOTES_VS_MAIN.md](RELEASE_NOTES_VS_MAIN.md).

## Commity kodu

- `257a2dd` — brak wyszukiwania w bieżącej liście startowej i wynikach KO.
- `e580393` — podpowiedzi tylko przy celowym najechaniu/F1.
- `d9bab6f` — wspólna precyzja rankingu i remisów.
- `c68f92b` — rozdzielenie belki trenera, jury i bazy serii.
- `19d5a97` — zachowanie roboczego tekstu w polach liczbowych.
- `8023b76` — wspólny algorytm awansu, osobne kwalifikacje, filtry wejścia, KO/drużyny i role wyników.
- `fa6ea6e` — stabilne przeciąganie zawodników i szybkie przewijanie.
- `5285fd8` — stabilny model kalendarza, zachowanie scrolla, zwarte karty i dni tygodnia.
- `cce62ba` — powiązania wyborów skoczni/zasad, flagi, prawidłowy nagłówek kwalifikacji.
- `2d2cb43` — angielskie tłumaczenia oznaczeń.
- `8359eba` — wysokość inspektora i integracja regresji.

## Weryfikacja

- Pełne `scripts/test-quick-ui.sh`: **kod 0**, 375 renderów, `/tmp/sj-full-final.log`.
- Końcowy `--dates-only` po dodaniu osobnego testu przestrzeni: **kod 0**, `/tmp/sj-inspector-space-final.log`.
- `--ux-only`: **kod 0**, `/tmp/sj-ux-final.log`; rzeczywiste konkursy i dialog belki, neutralne Q, poprawny licznik.
- `--workflow-only` pod gdb: normalne zakończenie, `/tmp/sj-workflow-gdb.log`.
- Długie przeciąganie i undo: `/tmp/sj-rosters-after-2026-10-24.log`, kod 0.
- Wolne wpisywanie/stepping i F1: `/tmp/sj-roster-inputs-only-2026-10-24.log`, kod 0; ponownie objęte pełnym zestawem.
- Wariant Widgets-only: `make -j2` w `build-widgets-compat`, **kod 0**, `/tmp/sj-widgets-final.log`.
- `git diff --check` bez błędów.
- Ręcznie obejrzane rendery Q i końcowego inspektora w rozmiarach 1024 i 360 px szerokości. Zrzuty: `build-quick-ui/ui-renders/calendar-inspector-space-*` oraz `qualification-markers-*`.

Środowisko weryfikacji: Linux, Qt 5.15.3, offscreen/software. Nie wykonano w tej partii natywnego uruchomienia Windows/Qt6 ani testów GPU. Pełny log nadal zawiera wcześniejsze ostrzeżenia HomeTools o implicitHeight i komunikaty offscreen/null pixmap; nie są nowymi poprawkami tej partii. Jedno wcześniejsze uruchomienie workflow zakończyło się komunikatem stack protector bez stosu; kolejny przebieg pod gdb oraz pełny zestaw zakończyły się poprawnie. Przy ewentualnym ponownym wystąpieniu najpierw zebrać stos — przyczyna tego pojedynczego zdarzenia nie została ustalona.

## Kontynuacja

Nie ma pozostawionego niewykonanego punktu z tej partii. Następny krok po nowym poleceniu: zgłoszenia z testów użytkownika oraz natywna weryfikacja Windows/Qt6 przed wydaniem. Nie zmieniać numeru wersji ani main bez polecenia. Nie nadpisywać userData i nie zamykać gry uruchomionej przez użytkownika.
