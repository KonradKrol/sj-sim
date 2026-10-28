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
- deszcz: osobny `rainLossPerLevel × max(0, poziom − rainLossThreshold)`;
- domyślnie odpowiednio 0,008, 0,003 i próg 2/10. To **założenia gry do strojenia**,
  a nie współczynniki potwierdzone badaniami. Lekki deszcz nie dostaje kary;
  większy deszcz ma mniejszy efekt niż ten sam poziom śniegu;
- nie dodajemy twierdzenia, że każdy deszcz pogarsza lot lub podnosi ryzyko upadku.
  Nie mamy wystarczającej podstawy do konkretnej zależności. Nie modelujemy
  obecnie czyszczenia torów, temperatury, drożności odpływu ani retencji wody.

Uproszczone opisy UI dotyczą tego modelu. Losowanie według daty i kraju jest
heurystyką, a nie prognozą ani odtworzeniem klimatu konkretnej skoczni.
