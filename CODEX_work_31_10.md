## Stan pracy — 1.11 (Codex)

Pracuj sam na `feature/simulation-v2`; każdy ukończony zakres commituj i wypychaj.
Zachowuj hierarchię, kontrast, jasność i brak wizualnego szumu.

- **-3 Listy:** nowy edytor inline ma test 10 list tworzonych przez UI,
  zapisu i odczytu, składu, aktywnej listy oraz cofania po zmianie przypisania
  kalendarza. Sprawdzono 600 zawodników, przeciąganie między panelami, szybkie
  przełączanie list, zmianę nazwy, filtrowanie, zaznaczenie, grupowanie i zmianę
  rozmiaru. Obejrzano oba motywy, 1440×900, 1024×640, 520×600, 360×480 oraz
  większą skalę interfejsu. Naprawiono pętle układu. Szczegóły starszych zadań
  pozostają w `HANDOFF-31-10-LISTY.md`.
- **-2, -1, 0:** walidacja poprawnych, różnych zawodników w składzie; domyślne
  nazwy w języku interfejsu; pojedyncze uzupełnianie i większa tabela drużyn.
- **1:** dodano ochronę pustej i nieaktualnej klasyfikacji oraz test. Oryginalny
  losowy crash użytkownika nie był odtworzony na jego prywatnym zapisie.
- **2, 3, 4, 6:** neutralne tło słabego perka, ukryty ręczny generator formy,
  brak uwag o migracji przed konkursem, paczki w osobnej zwijanej sekcji.
- **7, 8:** wykres edycji stale pokazuje pole wyjściowe; losowanie ustawia nowe
  ziarno, więc te same cztery parametry mogą dać inny układ. Samo wejście w
  edycję nie losuje wiatru. Wyjaśniono to użytkownikowi.
- **9, 10:** rozróżnione sortowanie i grupowanie krajów; specjaliści różnie
  wykorzystują wiatr, a siłę zależności można stroić w labie.
- Wyjaśniono użytkownikowi wykresy wiatru. Niezależny test „O autorze” naprawiono:
  test musi poczekać na zakończenie animacji zamykania powitania.

Testy uruchomione w izolowanych danych: `--simulation-v2-season-only`,
`--simulation-v2-competition-only`, `--roster-workspace-only`, `--roster-only`,
`--choice-popup-only`, `--workflow-only`, `--athlete-studio-only`, `--author-only`,
`--balance-lab-core-only`, `scripts/test-simulation-v2-core.sh` i kontrola tłumaczeń.
Nie oznacza to, że pełny historyczny smoke całej aplikacji został sprawdzony.

---

Zaczynam punktację od minusowych liczb, bo zacząłem dorabiać punkty doraźnie, nie zwracaj na to uwagi.

-3. Ukończ całą zaległą pracę z pliku HANDOFF-31-30-LISTY.md - zadawaj pytania jeśli czegoś nie wiesz.

-2. Nie wiem jakim cudem, ale powstają z losowania niekompletne drużyny. Jak drużyna nie ma 4 zawodników to nie może być drużyną... Chyba że tam się znaleźli zawodnicy bez imienia i nazwiska, przypadkiem dodani w bazie danych. Ale takich zawodników chcemy zapisać domyślną nazwą, zależną od wybranego aktualnie przy dodawaniu języka interfejsu. Nawet jeśli są po 4 zawodników, to coś uniemożiwia rozgrywanie konkursu bo jest komunkat że nie każda drużyna jest kompletna.

-1. Są dwa przyciski "uzupełnij drużyny" przed konkursem, tam w ogóle są niepotrzebnie aż 3 przyciski które zabierają mnóstwo miejsca w pionie.

0. Tamta tabelka składów drużyn mogłaby mieć dużo więcej pionowego miejsca.

1. Kliknąłem nową nierozegraną klasyfikację chcąc sortowac wyniki przed konkursem w trybie sezonów - aplikacja zcrashowała.

2.  perk "lekko" nie ma mieć zmienianego backgroundu tak jak reszta. dzięki temu też perk nr II tez ma byc slabszy niz III. Lekko prawie niewidoczne, potem troche bardziej, bardziej aż jest skrajnie bardzo widoczne z tą czaderską animacją.

3. usuń generator formy z widoku. będzie auto-generator na bazie dat.

4. Słabe UI: jakies bledy migracji i wczytywania paczek, są wyświetlane na ekranie pojedynczego konkursu. ten ekran nie jest do tego odpowiednim miejscem, a ekran bazy danych.

6. W ekranie pojedynczego konkursu, wybór paczki do wczytania nie powinien być razem z ustawieniami typowo konkursowymi, np. zasady czy skocznia konkretna. To daj gdzieś w mniej widocznym, mniej ważnym miejscu.

7. Jest to totalnie nieprawidłowe, że klikam na jeden z sektorów wiatru w edytorze, i zmienia się nagle cały wiatr na zeskoku, potem odklikuję, to wraca do poprzedniego stanu.

8. Ważne pytanie, muszę mieć na nie odpowiedź - co decyduję o konkretnym rozstawieniu wiatru na zeskoku? Czy jeśli wejdę dwa razy z tym samym setem 4 parametrow (kierunek, sila, zmiennosc i różnice) to dostanę takie same sekcje wiatrowe?

9. rozdziel obie opcje sortowania list startowych, bo są dwie opcje sortowania krajem. rozdziel je jasno, w punkt opisując różnicę między nimi.

10. perk lotnik / specjalista od małych skoczni - ten perk ma też wpływać na wykorzystanie wiatru. lotnik lepiej wykorzystuje wiatr pod narty ale gorzej sobie radzi z wiatrem w plecy, niż specjalista od skoczni małych, który aż tak dobrze nie wykorzysta wiatru z przodu. To nie jest game-changer, ale ma realny wpływ.


# dowiedz się
1. jak interpretować wykresy wiatrowe w balance labie
