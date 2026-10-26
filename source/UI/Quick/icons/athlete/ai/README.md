# Piktogramy perków

Sześć oryginalnych obrazów PNG wygenerowanych narzędziem `image_gen.imagegen`
w trybie wbudowanym (26.10.2026). Każdy symbol powstał osobnym wywołaniem,
z przezroczystym tłem. Pliki zachowują oryginalne piksele i kanał alfa.
Pełne prompty i identyfikatory wybranych generacji: [prompts.json](prompts.json).

| Plik | Perk | Symbol |
| --- | --- | --- |
| training.png | Błyszczy na treningach | Sztanga |
| reserve.png | Oszczędza siły | Serce i strzałka regeneracji |
| flight.png | Lotnik | Poziomy lot i szerokie smugi powietrza |
| small.png | Specjalista małych skoczni | Ugięte nogi przy odbiciu, impuls pod nartami i błyskawica |
| steady.png | Regularny | Trzy równe słupki i płaska linia jednakowych wyników |
| volatile.png | Nieprzewidywalny | Kostka do gry |

Lotnika, specjalistę i regularność wygenerowano ponownie po ocenie pierwszego
zestawu: lot ma poziomą, płynącą sylwetkę, a odbicie — zwartą sylwetkę nad progiem
i osobny symbol siły. Metronom zastąpił prosty wykres jednakowych wyników.
Prompty poprzednich wersji pozostają w `previousGenerations` w manifeście;
poprzednie pliki można odtworzyć z historii Git.

`AthletePerkArt.qml` łączy piktogram z dotychczasową oprawą SVG. Oprawy
zawierają teraz wyłącznie ramkę i oznaczenia poziomu. Wspólny renderer obsługuje
wybór perka, przypisane perki i pozostałe warianty podglądu zawodnika.

Oryginały mają granatowy kolor. `PerkIconProvider` przy wczytaniu do silnika QML
skaluje je do 256 × 256 i nadaje jasny kolor za pomocą `CompositionMode_SourceIn`,
zachowując wygenerowaną przezroczystość. Sześć gotowych obrazów jest współdzielonych
przez kontrolki; zmiana poziomu nie uruchamia ponownego dekodowania oryginału.
Nie wymaga to shaderów ani modułów efektów graficznych zależnych od wersji Qt.

Zmiana samych symboli nie wpływa na model zawodnika ani symulację.
