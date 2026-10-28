# Śnieg i deszcz — podstawy modelu Sj.Sim

Research 28.10.2026. Materiał dla developmentu; UI ma zawierać jedną krótką uwagę,
nie instrukcję meteorologii. Liczby w grze są współczynnikami do strojenia.

## Ustalenia ze źródeł pierwotnych

- [FIS ICR, §417.1 i §401.2.4](https://assets.fis-ski.com/f/252177/x/c67426c343/icr-ski-jumping-2024_e_clean.pdf):
  tory zimowe wymagają przygotowania/chłodzenia, odpływu wody z deszczu lub
  roztopów, a śnieg w trakcie zawodów wymaga utrzymywania czystego rozbiegu.
  Nie wynika stąd uniwersalna procentowa kara za każdy poziom opadów.
- [FIS: How Ski Jumping changes with the seasons](https://www.fis-ski.com/nordic-combined/news/2025-26/nordic-combined-101-how-ski-jumping-changes-with-the-season):
  letnie tory ceramiczne i maty są celowo nawadniane, aby umożliwić dobry ślizg.
  Dlatego sam deszcz lub wilgoć nie powinny zawsze działać jak zasypany tor.
- [FIS: konstrukcja skoczni igelitowych, s. 3](https://assets.fis-ski.com/f/252177/c0c6541e55/instructions-for-the-construction-of-plastic-covered-jumping-hills-edition-2022_english.pdf):
  woda jest używana na letnim rozbiegu, ale potrzebny jest sprawny odpływ.
- [WSL/SLF: Improving safety at snow parks](https://www.slf.ch/en/news/improving-safety-at-snow-parks/):
  pomiary wiążą tarcie ze stanem, temperaturą, ziarnem i wilgotnością śniegu;
  mokry świeży śnieg może hamować znacznie mocniej niż suchy przemrożony.
  To badanie freestyle na śniegu, nie kalibracja toru skoczni. Nie przenosimy
  jego mnożników bezpośrednio na metry w skokach.

## Decyzja implementacyjna i ograniczenia

Oddzielny `PrecipitationKind` przechodzi przez pole pogody, preview, pełny skok,
snapshoty batcha i metadane wyniku. Rodzaj opadów nie przełącza nawierzchni.
Przy dokładnym wymuszeniu wiatru rodzaj i natężenie opadów pozostają zachowane.

Model przybliża **warunki rozbiegu**, nie symuluje fizycznej prędkości, masy
namokniętego kombinezonu czy zmiany jego współczynników aerodynamicznych:

- śnieg: dotychczasowy regulowany spadek zapasu `precipitationLossPerLevel × poziom`;
- deszcz: osobny `rainLossPerLevel × max(0, poziom − rainLossThreshold)`
  oraz dodatkowy nieliniowy koszt ulewy opisany poniżej;
- domyślnie odpowiednio 0,008, 0,003 i próg 2/10. To **założenia gry do strojenia**,
  a nie współczynniki potwierdzone badaniami. Lekki deszcz nie dostaje kary;
  ulewa ma teraz dodatkowy koszt i może działać mocniej niż śnieg;
- nie dodajemy twierdzenia, że każdy deszcz pogarsza lot lub podnosi ryzyko upadku.
  Nie mamy wystarczającej podstawy do konkretnej zależności. Nie modelujemy
  obecnie czyszczenia torów, temperatury, drożności odpływu ani retencji wody.

Uproszczone opisy UI dotyczą tego modelu. Losowanie według daty i kraju jest
heurystyką, a nie prognozą ani odtworzeniem klimatu konkretnej skoczni.

## Ponowne sprawdzenie deszczu — badanie i kalibracja 28.10

Użytkownik uważa, że mocny deszcz może działać dużo bardziej destrukcyjnie.
Obecnego `rainLossPerLevel=0,003` nie uznawać za zatwierdzoną kalibrację.
Przy natężeniu 10 daje zaledwie 2,4% straty zapasu; trzeba zbadać ciężki deszcz
osobno, zamiast wnioskować o ulewie z korzystnego nawadniania letniego rozbiegu.

Dodatkowe źródła do dalszej analizy:

- [Hasegawa i in., 2026, badanie w tunelu aerodynamicznym](https://www.jstage.jst.go.jp/article/tvsj/46/7/46_10/_article/-char/en):
  przepuszczalność tkaniny ma znaczenie, lecz wynik zależy od części ciała;
  większa przepuszczalność na tułowiu poprawiła stosunek siły nośnej do oporu
  w tym modelu. **To nie badanie mokrego kombinezonu**, więc nie wyciągać
  automatycznie wniosku „mokry = gorszy/lepszy lot” z samej przepuszczalności.
- [Hasegawa i in., 2018](https://www.jstage.jst.go.jp/article/aem/3/0/3_118/_pdf/-char/en):
  badanie tkanin na walcu eliptycznym; pomocne do zrozumienia mechanizmu,
  bez kalibracji opadów na metry skoku.
- [FIS o kombinezonach](https://www.fis-ski.com/ski-jumping/news/2024-25/microchips-for-ski-jumping-suits-levelling-the-playing-field-and-reducing-waste):
  potwierdza znaczenie materiału i przepuszczalności, bez liczbowego efektu deszczu.

Następny krok: znaleźć bezpośrednie obserwacje mocnego deszczu/zalanych torów,
odróżnić je od mokrego śniegu i ślizgu na przygotowanym igelicie. Potem kontrolowane
serie silnika dla 0/2/5/8/10 przy stałych profilach, wietrze i seedach. Ewentualny
silny efekt ulewy powinien narastać płynnie i być osobnym jawnym współczynnikiem
gry; nie przedstawiać arbitralnej funkcji jako pomiarów naukowych.

### Bezpośrednie źródła dotyczące deszczu

- [FIS: Guideline for Inrun track profile](https://assets.fis-ski.com/f/252177/x/d3dc008ef1/guideline-inrun-track-structure.pdf)
  wymaga rozwiązań umożliwiających odpływ deszczówki z torów. Potwierdza znaczenie
  wody i przygotowania nawierzchni; nie podaje uniwersalnej straty w metrach.
- [FIS: relacja z Oberstdorfu](https://www.fis-ski.com/ski-jumping/news/2018-19/articlekamil-stoch-takes-the-win-oberstdorf)
  zawiera wypowiedź Tande o mokrym kombinezonie oraz zazwyczaj szybkim rozbiegu
  przy niewielkim deszczu. To relacja zawodnika, a nie kontrolowany pomiar.
- [FIS: Klingenthal 2022](https://www.fis-ski.com/ski-jumping/news/2022-23/dawid-kubacki-takes-it-all-in-klingenthal)
  opisuje bardzo trudną fazę konkursu podczas mocnego deszczu i zmieniającego się
  wiatru. **Nie izoluje wpływu deszczu**. Nie przypisywać wszystkich różnic w wynikach
  samym opadom ani nie odczytywać z tej relacji dokładnych współczynników.

Wniosek implementacyjny: odróżniamy mały deszcz od ulewy i umożliwiamy duży wpływ
mocnych opadów. Nie udajemy, że mamy empiryczny wzór na namoknięty kombinezon.
Nie dodajemy osobnych, niepotwierdzonych współczynników siły nośnej czy oporu.

### Przyjęta kalibracja gry

Do poprzedniego łagodnego kosztu dodano:

```
ulewa = max(0, (natężenie − 5) / (10 − 5))
dodatkowa strata zapasu = 0,15 × ulewa²
```

Wszystkie trzy liczby to jawne `FlightParameters`: `rainHeavyThreshold`,
`rainHeavyLossAtMaximum`, `rainHeavyExponent`. Domyślna łączna strata zapasu:
0% przy 0–2, 0,9% przy 5, 7,2% przy 8 i 17,4% przy 10. Krzywa jest płynna;
nie włącza nagłej kary przy przekroczeniu progu. Śnieg pozostaje osobnym modelem.
**To propozycja balansu Sj.Sima, nie wartości zmierzone przez FIS.**

Kontrolowane porównanie 300 000 skoków: 3 skocznie × 5 natężeń × 2 modele ×
10 000 powtórzeń, OVR 80, forma 70, brak perków, stała belka 10 i brak wiatru.
Każdy wariant używa identycznych seedów jakości. Dla K120/HS134 średnia odległość:

| Deszcz | Poprzednio | Po kalibracji |
| --- | ---: | ---: |
| 0/10 | 120,849 m | 120,849 m |
| 2/10 | 120,849 m | 120,849 m |
| 5/10 | 119,759 m | 119,759 m |
| 8/10 | 118,671 m | 112,146 m |
| 10/10 | 117,950 m | 99,822 m |

Maksymalna ulewa skraca w tym porównaniu średnią o 16,65 m na K95, 21,03 m na
K120 i 35,05 m na K200 względem braku opadów. Brak dodatkowych losowych kar
zależnych od gracza; zmienność pochodzi z istniejącej jakości i czasowego pola.

Odtworzenie: `scripts/analyze-rain-v2.sh 10000`. Program wypisuje CSV także
z punktami i częstością upadków. Spadek liczby upadków przy krótszych skokach
jest skutkiem mniejszej odległości — nie dodano niezweryfikowanego wzrostu ryzyka
upadku tylko dlatego, że pada. Nadal nie ma retencji wody/czyszczenia torów.
