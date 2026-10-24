# Usunięcie rekurencyjnego przeliczania layoutów

## Przyczyna i poprawka

Wspólny komponent `EmptyState.qml` używał wycentrowanego `ColumnLayout`. Jego wysokość zależała od zawijania etykiet i wysokości przycisku. Zmiana wysokości podczas `rearrange()` przesuwała ten sam layout przez `anchors.centerIn`, co ponownie uruchamiało `rearrange()` przed zakończeniem pierwszego przebiegu. Qt dochodził do limitu dwóch zagnieżdżonych iteracji i przerywał obliczenia z ostrzeżeniem.

Diagnostyczna kategoria `qt.quick.layouts` potwierdziła sekwencję `rearrange 0 → 1 → 2` oraz równoczesne zmiany pozycji Y i wysokości tej samej kolumny. Komponent występuje w tabelach, edytorach i pustych ekranach sezonu, dlatego problem ujawniał się w wielu pozornie niezależnych miejscach.

Statyczny układ pustego stanu został zastąpiony zwykłym `Column`. Etykiety mają jednoznaczną szerokość kontenera, a przycisk jest ograniczony do dostępnej szerokości. Zachowano centrowanie, zawijanie tekstu, stany kompaktowe i akcje. Nie wyłączono kategorii logowania i nie zamaskowano ostrzeżeń.

## Wyniki

- Ten sam automatyczny scenariusz przygotowania sezonu, z diagnostyką layoutów: **8 → 0** ostrzeżeń; liczba wejść `QQuickGridLayoutBase::rearrange` **2675 → 2324**.
- Pełny zestaw regresji, **399 renderów**: **162 → 0** ostrzeżeń `Detected recursive rearrange`.
- Testy mają teraz obserwator komunikatów, który zachowuje logowanie i kończy test błędem przy ponownym wystąpieniu rekurencyjnego layoutu.
- Liczby wejść dotyczą wskazanego scenariusza, nie są pomiarem FPS ani procentowym przyspieszeniem całej aplikacji.
- Próba przechwycenia `.qtd` lokalnym profilerem Qt 5 nie dała poprawnego śladu (usługa profilera była wyłączona); diagnoza opiera się na bezpośrednich logach diagnostycznych Qt i powtarzalnych testach. Nie wyciągano wniosków o czasie klatek z uszkodzonego śladu.

## Kolejne zachowane zadania

1. Dyskretny wybór skoku z wcześniejszych serii w karcie, Shift+klik w kolumnę serii oraz rozwijane wyjaśnienie wag.
2. Na końcu: brak komunikatu straty/przewagi dla pierwszego zawodnika serii i przy remisie 0,0 pkt.
