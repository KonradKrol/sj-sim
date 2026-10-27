# Uruchomienie Qt Quick na Linuksie z Qt 5

Po zmianach przygotowanych z Qt 6 ekran startowy nie ładował się w Qt 5.15:
nowe komponenty zawierały importy modułów bez numerów wersji. Taka sama sytuacja
dotyczyła również komponentów nowego edytora skoczni i wyboru siły perków.

Importy są zgodne z Qt 5.15 oraz używanym w projekcie Qt 6. Import stylu
`QtQuick.Controls.Basic` zastąpiono wspólnym importem Controls; na Qt 6 aplikacja
nadal wybiera styl Basic w `QuickUi`. Usunięto też właściwości siatki dostępne
dopiero w Qt 6: dwie karty menu mają jednakową preferowaną szerokość i wspólną
wysokość wyznaczaną na podstawie zawartości.

Wysokość kafelków jest obliczana z niezależnych pomiarów tekstu, a szerokość
pomiaru wynika z dostępnej szerokości siatki. Dzięki temu przeliczanie układu
nie odczytuje z powrotem własnej wysokości. Analogicznie odłączono wysokość
animowanych przypisanych perków od wysokości ich wewnętrznego layoutu.

Przebudowanie i uruchomienie z katalogu repozytorium:

```sh
./scripts/build-quick-ui.sh
./scripts/run-quick-ui.sh
```

Istniejące dane zapisane w katalogu kompilacji nie są kopiowane ponownie ani
nadpisywane przez skrypt budowania.

Weryfikacja na Linux/Qt 5.15.3: build, uruchomienie właściwego programu przez
8 sekund w izolowanym katalogu i regresje `--motion-feedback-only`,
`--appearance-only`, `--hill-studio-only` oraz `--athlete-patches-only` przeszły.
Sprawdzono równe rozmiary kafelków menu w jasnym i ciemnym motywie na różnych
rozmiarach okna. Logi końcowych uruchomień nie zawierały błędów importu ani
ostrzeżeń o pętlach QML. Fixture anulowanego przeciągnięcia jawnie wysyła ruch
myszy z wciśniętym lewym przyciskiem, co działa także w Qt 5.

Środowisko Linux używane do weryfikacji nie ma lokalnego Qt 6; wersji dla Windows
nie budowano w tym przebiegu.
