# Nowe laboratorium — wdrożenie 29.10.2026

Użytkownik zatwierdził wykonanie całego projektu obok starego playgroundu.
Projekt: [BALANCE_LAB_DESIGN.md](BALANCE_LAB_DESIGN.md).

Dodatkowe odpowiedzi z wdrożenia:

1. Zastosowanie profilu od następnego konkursu/sesji, także w istniejącym sezonie;
   rozpoczęty konkurs zachowuje kopię parametrów.
2. Nowe laboratorium obejmuje również KO i drużyny.
3. Mapy: gotowe kombinacje i dowolne liczbowe osie; kliknięcie wybiera dane.
4. Szybki mały podgląd; dokładna duża próba osobno.
5. Reprezentacje z paczki oraz dowolne drużyny testowe.
6. Domyślnie środkowe 80% wyników, osobno przełączalny przedział średniej 95%.
7. Przywracanie warsztatu, osobno nazwane profile i historia.

## Zrealizowany zakres

Dodano moduły source/balance/BalanceLabModel, BalanceLabController i BalanceRuntime
oraz dziesięć komponentów BalanceLab*.qml. Stary PlaygroundScreen zostaje.
Wejście: menu główne oraz Ustawienia → Symulacja obok starego playgroundu.

Obejmuje: 15 pytań/obszarów, trzy warianty, dane z katalogu, edycję grup, mapy,
lądowanie, wiatr, generator, punkty, profile, cele, historię, odtwarzanie i CLI.
Wspólny rejestr ma 607 ustawień. Domyślnie panel pokazuje parametry związane
z pytaniem, a pełny rejestr jest dostępny osobnym przełącznikiem i wyszukiwarką.
Instrukcja i przykłady: [BALANCE_LAB.md](BALANCE_LAB.md).
Generator presetów ma edytowalne zakresy; automatyczna propozycja ma edytowalne
wagi i tablice. Wspólne źródło w WeatherCalibration, bez osobnego silnika UI.
Game profile jest kopiowany do JumpSimulator i CompetitionInfo; kopia jest
zapisywana w konkursie. Nowe kopie kalendarzy nie dziedziczą balansu starego eventu.

## Właściwości i granice

- Profile gry zawierają współczynniki silnika, pomiaru i generatora. Zmiany danych
  skoczków, skoczni, stawki, belki i zasad pozostają danymi eksperymentu.
- Cele mogą zamrażać dane osobnych scenariuszy. Przy kolejnej kontroli podstawiane
  są aktualne współczynniki każdego wariantu, bez podmieniania skoczków i skoczni.
- Ocena średnich wymaga co najmniej 30 powtórzeń; przedział 95% przecinający granicę
  celu jest nierozstrzygnięty. Dokładne prawdopodobieństwo upadku nie udaje próby.
- Szybki podgląd jest ograniczony kosztem. Pełna mapa ma jawny budżet, a liczbę
  konkursów na komórkę pokazuje UI i raport. Rozdzielczość diagnostyki podglądu
  jest ograniczana; pełna analiza respektuje ustawienia użytkownika.
- Odtwarzanie pokazuje zapisane odcinki ekspozycji na wiatr, a nie animowany tor
  lotu. Raporty z innej kompilacji mają ostrzeżenie przy nowym odtwarzaniu.
- Indywidualne, KO i drużyny korzystają z menedżerów gry. Naprawiono dodawanie
  wyników drużynowych w runnerze; nie tworzy się wyników z pustym wskaźnikiem drużyny.
- Warsztat, profile i historia są osobno w AppData/balance-lab. Testy mają prywatny
  katalog przez właściwość aplikacji; nie zmieniają aktywnego balansu użytkownika.
- PL/EN/CS: 373 nowe komunikaty katalogów, brak unfinished i błędów placeholderów.
- Zgodność źródeł Qt5/Qt6 zachowana. Ten etap zbudowano i testowano na Windows,
  Qt 6.8.3/MSVC; nie deklarujemy ponownej walidacji na Linux/Qt5.

Automatyczne strojenie, 1–5 prostych ustawień dla gracza oraz publiczny system
modowania silnika pozostają poza tym etapem. Sam balans wymaga eksperymentów
i decyzji użytkownika; gotowy warsztat nie oznacza skalibrowanej symulacji.

## Weryfikacja

- Cztery zestawy rdzenia MSVC: random, quality, weather i engine — PASS.
- CLI nowego laboratorium: trzy warianty, 607 ustawień, zgodność próbek i rozkładów
  na 1/4 wątkach, mapa, diagnostyka, odtwarzanie i odrzucanie błędnych zakresów — PASS.
- CLI dotychczasowego playgroundu: powtarzalność, niezależny pomiar i cel OVR90–70
  — PASS, przewaga 15,3574 m w istniejącym scenariuszu kontrolnym.
- Natywne regresje: dotychczasowy playground, adapter/katalog v2, pomiar, zegar
  konkursu, sezon, konfiguracja, zapis, seryjna symulacja i języki — PASS.
- Backend laboratorium: mapy, grupy, cele i zamrożone scenariusze, generator,
  profile, kopie dla rozpoczętych sesji, KO/drużyny i odtworzenie warsztatu — PASS.
- Rozszerzony test UI — PASS: nawigacja, podgląd, silnik lądowania, wiatr,
  generator, mapa, odtwarzanie, wypełniony cel i profile. Zrzuty dla 1440×900,
  1024×640, 520×600 i 360×480, oba motywy i tekst 130%; sprawdzone wizualnie.
  Test kontroluje również ostrzeżenia QML i powiązanie formularza celów z backendem.
- Końcowa kompilacja aplikacji i testów Windows/Qt 6.8.3/MSVC — PASS.
  Skrót `Sj.Sim QML.lnk` wskazuje aktualny `build-win/release/sj-sim.exe`.
