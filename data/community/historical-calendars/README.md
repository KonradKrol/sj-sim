# Kalendarze FIS 2023–2025 do Sj.Sim 2

Gotowa paczka: **Kalendarze-FIS-2023-2025.sjsim-pack**.

Powstała z paczki użytkownika `KalendarzeStarySjSim.sjsim-pack`. Oryginał pozostał bez zmian. Nowa paczka zachowuje dołączone skocznie, zasady i punktacje, a cztery presety kalendarzy odbudowuje według archiwum FIS.

| Preset | Konkursy indywidualne | Drużynowe / duety | Uwagi |
| --- | ---: | ---: | --- |
| Lato2023 | 9 | 0 | Usunięte dodatkowe Zakopane i Hinterzarten. |
| Lato2024 | 9 | 0 | Wisła w terminie zastępczym 14–15 września. |
| Zima-2023-24 | 33 | 7 | 32 indywidualne PŚ + MŚ w lotach; 6 drużynowych/duetów PŚ + MŚ. |
| Zima-2024-25 | 31 | 5 | 29 indywidualnych PŚ + 2 MŚ; 4 drużynowe/duety PŚ + MŚ. |

Daty są historyczne, a nie przesunięte na bieżący rok. Przy tworzeniu rozgrywki wybierz odpowiedni rok rozpoczęcia sezonu: 2023 albo 2024. Do gry wczytaj paczkę przez Bibliotekę zawartości, a następnie preset w edytorze kalendarza.

## Co poprawiono

- Rzeczywiste miejsca, kolejność i daty odbytych konkursów, także terminów zastępczych; nie dodawano fikcyjnych dat dla Predazzo, Kulm czy Râșnova w PŚ 2024/25.
- Dodano brakujący konkurs w Lahti 1 marca 2024, przeniesiony po odwołaniu Szczyrku. Całkowicie odwołane konkursy pominięto.
- Oba indywidualne konkursy w Vikersund 2024 są 17 marca: rano przeniesiony konkurs jednoseryjny, po południu finał 30 → 20 → 10.
- Kwalifikacje, treningi i próbne mają daty z osobnych kart wyników FIS. Nie dopisywano kwalifikacji, które nie zostały rozegrane. Próby i treningi bez potwierdzonej karty wyników pominięto zamiast zgadywać.
- Liczby treningów wynikają z kart oficjalnego treningu. Liczba serii głównych konkursów uwzględnia faktyczne skrócenia zawodów, nie tylko pierwotny plan.
- Mistrzostwa na Kulm i w Trondheim pozostają poza Pucharem Świata i Pucharem Narodów. Trondheim korzysta z właściwego HS138.
- Odbudowano powiązania kwalifikacji, próbnych i treningów. Kwalifikacje prowadzą do właściwej liczby uczestników pierwszej serii, także na mamutach.
- Odbudowano przypisania PŚ, Pucharu Narodów, TCS, Raw Air, lotów i Planicy; skorygowano błędny indywidualny typ dwóch klasyfikacji Pucharu Narodów.
- Presety nie zawierają wyników, rozegranych konkursów, startujących zawodników ani wybranych drużyn.

## Granice odwzorowania

- **To kalendarze męskie. Konkursy mieszane pominięto**, ponieważ Sj.Sim nie odwzorowuje wymaganego składu kobiet i mężczyzn. Nie zastępujemy ich fikcyjnymi męskimi duetami lub drużynówkami.
- Sj.Sim przypisuje jedną datę całemu konkursowi. Indywidualne MŚ na Kulm 26–27 stycznia 2024 mają więc datę zakończenia, **27 stycznia**, zgodną z wpisem wynikowym FIS. Trzy rzeczywiście rozegrane serie pozostają jednym konkursem; silnik nie przesuwa daty między seriami na następny dzień.
- Historyczne punkty turniejów korzystają z mechanizmów klasyfikacji Sj.Sima; ta paczka nie dodaje nowej logiki specjalnych rankingów FIS.
- Zachowano identyfikatory/nazwy czterech presetów. Przy imporcie sprawdź komunikat gry o istniejących zasobach i ewentualnych kopiach.

## Źródła i kontrola

Podstawą jest [archiwum kalendarzy i wyników FIS](https://www.fis-ski.com/DB/ski-jumping/calendar-results.html), dla LGP 2023/2024 oraz PŚ 2023/24 i 2024/25, a także MŚ w lotach 2024 i MŚ 2025. Pliki `*-FIS.json` zawierają odnośniki do konkretnych imprez, wyników i kart treningów/próbnych. Pliki `*-terminarz.csv` przedstawiają każdy wygenerowany wpis oraz jego źródło.

Dodatkowa weryfikacja: [finałowy kalendarz LGP 2023](https://www.fis-ski.com/ski-jumping/news/2023-24/new-date-for-summer-grand-prix-in-courchevel), [PŚ 2023/24](https://assets.fis-ski.com/f/252177/1be4cf4ad0/wc_calendar_jp23-24_men_h.pdf), [Vikersund 16 marca 2025 — jedna seria](https://www.fis-ski.com/ski-jumping/news/2024-25/-unreal-domen-prevc-soars-to-win-in-vikersund-while-wellinger-and-nika-prevc-secure-overall-raw-air-victory).

Walidacja używa rzeczywistego `SeasonCalendarPreset::validateJson`, sprawdza chronologię i odwołania do wcześniejszych wydarzeń oraz importuje całą paczkę do izolowanej biblioteki. Nie zmienia biblioteki użytkownika.

Odtworzenie pliku:

```sh
python3 scripts/create-historical-calendar-bundle.py /ścieżka/KalendarzeStarySjSim.sjsim-pack /ścieżka/wynik.sjsim-pack
python3 scripts/test-resource-library.py historical-calendar-bundle.cpp
```
