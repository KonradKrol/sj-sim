# Lista skoczków w nowym edytorze

Nowy edytor otwiera Naszywki z listą skoczków po lewej i formularzem po prawej,
podobnie jak klasyczny widok bazy danych. Lista zastępuje rozwijany wybór zawodnika.

- Wiersze korzystają ze stylu klasycznej listy: flaga, nazwisko, zaznaczone tło,
  lewy akcent i delikatne separatory. Dodatkowo pokazują OVR.
- Jeden klik otwiera zawodnika. Wyszukiwanie obejmuje nazwę i kod kraju.
- Lista zajmuje całą dostępną wysokość, przewija się niezależnie od formularza
  i nie ma limitu wysokości.
- Zmiana nazwy lub parametrów aktualizuje istniejące wiersze, bez resetowania
  modelu i przewinięcia. Dodanie, usunięcie, Cofnij i zmiana paczki synchronizują
  listę oraz zaznaczenie. Zmiana paczki czyści filtr.
- Lista przechowuje indeks źródłowego profilu, dlatego kliknięcie wyniku
  wyszukiwania otwiera właściwego skoczka.
- Przed przełączeniem skoczka kończona jest edycja aktywnego pola, aby wpisany
  tekst nie trafił do następnego profilu.
- Strzałki i Home/End zmieniają zawodnika. Enter otwiera edycję na małym oknie.
- Przy małej szerokości dostępne są zakładki Lista/Edycja, analogicznie do
  klasycznego edytora. Przy większej szerokości oba obszary pozostają widoczne.
  Istniejący układ Naszywek dopasowuje się do węższego formularza.

Nowy komponent `AthleteLibrary.qml` korzysta wyłącznie z osobnego modelu
`AthleteStudioController`. Format paczek, dane klasycznych skoczków i silnik
symulacji pozostają bez zmian.

Weryfikacja obejmuje rzeczywiste kliknięcia i wpisywanie danych, filtrowanie po
nazwie/kraju, 41 profili, zachowanie przewinięcia po zmianie OVR, usunięcie i Cofnij,
klawiaturę oraz przełączanie paczek. Regresje widoku Naszywki obejmują ponadto
jasny/ciemny motyw, szerokości 360–1440, większy tekst i opcjonalne zdjęcie.

`scripts/test-quick-ui.sh --athlete-patches-only` — PASS na Linux/Qt 5.15.3,
łącznie z nowymi regresjami listy i budową aplikacji. Bez błędów ładowania QML
ani ostrzeżeń o rekurencyjnym przeliczaniu layoutu.
