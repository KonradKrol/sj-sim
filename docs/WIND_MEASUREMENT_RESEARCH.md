# Pomiar wiatru — badanie przed wyborem metod

Stan: **wdrożono sześć metod**, w tym „FIS — przybliżenie”. Użytkownik jawnie
zaakceptował przybliżenie po informacji o brakujących dokładnych wagach i oknach.
Nie jest to deklaracja dokładnej zgodności technicznej. Wariant dotychczasowego
Sj.Sima pozostaje dostępny i zachowuje wcześniejsze ustawienia.
Wzory, identyfikatory i parametry: `SIMULATION_V2_ENGINE.md`, sekcja o metodach.

Użytkownik chce **3–7 nazwanych metod**, różne stopnie realizmu, także opcję/opcje
oficjalne. Pomiar ma pozostać niedoskonały tak jak rzeczywisty system, bez osobnej
losowej kary i bez wyzerowania wpływu wiatru przez odwracanie wyniku symulacji.
Trzeba zachować wybór w zasadach konkursu i przyszłym eksperymencie; UI proste,
wyjaśnienia w pomocy. Nie mnożyć 30 technicznych inputów.

## Ustalone ze źródeł

- [FIS ICR, art. 415.3](https://assets.fis-ski.com/f/252177/x/c67426c343/icr-ski-jumping-2024_e_clean.pdf)
  opisuje ogólne trzy stanowiska i **osobne zasady** dla wind/gate. Nie wolno
  uznać ogólnych trzech stanowisk za pełną recepturę pomiaru punktowanego.
- [FIS / Swiss Timing, katalog usług 2025/26](https://assets.fis-ski.com/f/252177/x/f6ff2a73a4/fis-service_catalogue_sjp_men_wc_2022-2026_season_2025_v1-0.pdf)
  wymienia osiem urządzeń, dwanaście na lotach. To lista dostarczanego sprzętu,
  **nie algorytm** ani dowód, że wszystkie urządzenia mają równe wagi.
- [Swiss Timing, opis systemu](https://www.swisstiming.com/fileadmin/Resources/Data/Datasheets/DOCM_SJ_Competition-System_1215_EN.pdf)
  potwierdza pomiar wektorów, monitoring i zgodność wejścia z systemem FIS,
  bez publikacji kompletu wag/okien.
- [Pietschnig i in., 2020, oryginalna praca](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0238101)
  opisuje historyczne 5/7/10 stanowisk zależnie od rozmiaru skoczni, uśrednianie
  z wagami i zmianę z 2016. Omawia utrzymujące się niedoskonałości rekompensaty.
  Nie traktować historycznych liczb jako dowodu dokładnego algorytmu 2026.
  Referencja 4 to **FIS Fact Sheet. Wind and in-run compensation (2016)**;
  dokładny opis ma być też w dodatku pracy Jung/Müller/Staat 2018.
- [Jung, Müller, Staat, 2018](https://doi.org/10.1016/j.jbiomech.2018.05.001)
  bada zróżnicowane efekty wiatru w fazach lotu. Dostępny odczyt wydawcy zawiera
  abstrakt i fragmenty, nie komplet załącznika technicznego. To nadal trop do
  pozyskania dokładnej oficjalnej receptury.
- [Dokumenty FIS](https://www.fis-ski.com/ski-jumping/documents) są bieżącym
  indeksem dalszych wytycznych. Stary link do fact sheet 2009 w pracy 2020:
  `http://www.fis-ski.com/mm/Document/document/General/04/23/02/July292009NewrulesatSkiJumpingandNordicCombinedSummerGrandPrix2009_Neutral.pdf`.

## Dalsze badanie i świadomie zachowane ograniczenia

1. Dokładne, wersjonowane pozycje/wagi czujników, okna czasowe, wpływ krótkiego
   skoku, projekcja kierunku, kolejność zaokrągleń w wariancie oficjalnym.
2. Oznaczenia w grze: oficjalny tylko gdy potwierdzony; model przybliżony musi
   mieć jawne oznaczenie. Nie używać „FIS” jako marketingowej etykiety arbitralnych wag.
3. Sensowne dodatkowe warianty do porównania: pojedynczy odczyt przed wybiciem;
   średnia z równoważnych czujników przez czas lotu; ważona sieć pomiarowa;
   gęstsza sieć pomiarowa; średnia rzeczywiście doświadczana na przebytej trasie
   (symulacyjny punkt odniesienia, **nie** idealne wyrównanie odległości).
   Wdrożono pięć nowych modeli i zgodność (sześć opcji), zgodnie z tym podziałem.
4. Testy: stałe jednolite pole, czysty boczny wiatr, lokalny podmuch między
   czujnikami, zmiana po mijaniu sektora, krótki skok, brak ekspozycji (DNS/DSQ),
   zgodność zapisu i deterministyczna praca wielowątkowa. Lot nie zmienia się
   wskutek samego przełączenia metody pomiaru — zmienia się pomiar/punktacja.


## Zatwierdzone przybliżenie

Model 3 zachowuje zasadę projekcji wzdłuż zeskoku, stałych czujników, okna czasowego
i wag większych w środkowej/dalszej części. **Własny dobór gry:** progi K dla
liczby czujników, ich rozmieszczenie, zasięg 1,1K, 17 próbek czasowych i trójkątne
wagi ze szczytem 0,65. Nie przypisywać tych liczb FIS. Dokładniejsze odtworzenie
może powstać później po uzyskaniu technicznej specyfikacji; nie blokuje już
zaakceptowanego wdrożenia.
