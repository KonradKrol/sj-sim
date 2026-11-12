# Poprawki interfejsu — 12.11.2026

## Drużynówki

- Przygotowanie zachowuje dotychczasowy widok. Nowa zakładka **Składy** pokazuje
  wybór dostępnych zawodników po lewej i tę samą listę drużyn po prawej.
- Wybór dotyczy wyłącznie przygotowywanego konkursu. Można korzystać z list
  zawodników, wybrać wszystkich albo odznaczać pojedyncze osoby.
- Odznaczony zawodnik znika również z rezerwy; po usuniętym starterze pozostaje
  wolne miejsce. Pozostali starterzy nie przesuwają się automatycznie.
- Dobór automatyczny korzysta wyłącznie z dostępnych zawodników. Kraj z za małą
  liczbą dostępnych osób nie tworzy drużyny. Cofnięcie wyboru odtwarza także skład.
- Rezerwowego można przeciągnąć na wolne miejsce. Składy zablokowane po
  wcześniejszym wydarzeniu pozostają tylko do odczytu.
- Ukryta informacja o wcześniejszych wydarzeniach nie zajmuje miejsca w karcie.

## Edytor sezonowy i ustawienia

Zaznaczenie skoczka lub skoczni otwiera właściwy edytor także po przebudowaniu
modelu listy. Ładowanie profilu nie zapisuje poprzedniego profilu do nowej osoby.
Panel siedmiu ustawień symulacji wylicza wysokość bez pustego kontenera i nie
rozciąga przycisków w pionie. W interfejsie widnieje **Sj.Sim 2.0.0**, bez beta.

## Powitanie

Mniejszy dialog (maksymalnie 752 × 384 jednostek skali interfejsu), mniejszy
kadr zdjęcia i więcej szerokości na tekst. Zdjęcia wypełniają kadr z przycięciem.
Główne akcje są po prawej, „O autorze” po lewej. Na końcu główną akcją jest
„Ok”, a drugą „Rozpocznij przykładowy sezon”. Link Discord zmienia kursor.

## Weryfikacja

Build Linux/Qt Quick zakończony poprawnie. Izolowane testy (bez danych gracza):

```sh
python3 scripts/test-resource-library.py team-eligibility.cpp
python3 scripts/test-resource-library.py welcome-layout.cpp
```

Oba zestawy przeszły. Obejmują dobór i wykluczanie zawodników, wolne miejsca,
zamianę przez przeciąganie, dokładne cofnięcie składu, wybór profilu po
przebudowie listy, obecność zakładki, cztery kroki powitania i końcowe akcje.
Widoki sprawdzono także przy małym rozmiarze okna. W tych testach nie wystąpiły
ostrzeżenia o zapętlonych bindingach ani rekurencyjnym przeliczaniu layoutów.

## Dalsza naprawa list skoczków i skoczni w sezonie

Wybór z nowej listy trafia bezpośrednio do obiektu o wskazanym identyfikatorze,
a nie do wiersza pomocniczej tabeli, który mógł oznaczać inny obiekt po filtrowaniu.
Zmiana wyboru odświeża ekran od razu, bez oczekiwania na okresowe odpytywanie.
Nowa lista jest dostępna już przed pierwszym wyborem; nie zastępuje starej listy
w połowie interakcji. W małym układzie pozostaje załadowana podczas edycji.

Zaznaczenie jest niezależne od przewinięcia. Edycja danych nie przewija listy do
zaznaczonej osoby. Filtr i pozycja są przechowywane osobno dla skoczków i skoczni,
w ramach otwartego sezonu. Przy powrocie pozycja odtwarza się po ustaleniu wymiarów
listy. Ukrywany lub usuwany widok nie może nadpisać zapamiętanej pozycji zerem.
Usunięto też zapętlony binding dekoracji kart perków skoczni.

```sh
python3 scripts/test-resource-library.py season-library-sync.cpp
```

Test korzysta z rzeczywistego Qt Quick: wybiera skocznie mimo przefiltrowanej
pomocniczej tabeli, edytuje nazwę, przełącza skocznię, wybiera i edytuje zawodnika
oraz sprawdza przewinięcie po edycji i po zmianie zakładek. Dane są izolowane od
biblioteki gracza. Build i test zakończone poprawnie.
