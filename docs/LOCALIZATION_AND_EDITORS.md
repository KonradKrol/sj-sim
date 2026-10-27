# Języki i nowe edytory

Interfejs używa polskich tekstów źródłowych oraz katalogów `translations/translation_en.ts` i `translation_cs.ts`. Angielski glosariusz: **jumper**, **competition**, **round**, **trial round**, **start gate**, **take-off**, **flight**, **landing**, **judges’ marks**. Czeskie odpowiedniki: **skokan**, **závod**, **kolo**, **zkušební kolo**, **nájezdové okno**, **odraz**, **let**, **doskok**, **známky rozhodčích**. Nazwy danych użytkownika pozostają treścią zapisów.

Po zmianie tekstów uruchom `lupdate source -no-obsolete -ts translations/translation_en.ts translations/translation_cs.ts`, uzupełnij oba katalogi w Qt Linguist i wykonaj `python scripts/check-translations.py`. Kontrola odrzuca puste i nieukończone tłumaczenia oraz niezgodne parametry `%1`, `%2` itd. Pakowanie Windows i Linux kompiluje oba katalogi i dołącza polskie oraz czeskie tłumaczenia standardowych kontrolek Qt. Test `--translation-only` sprawdza przełączanie języka w otwartym widoku.

## Nowe profile

Skoczkowie i skocznie mają osobne modele autorskie. Bieżąca symulacja ich jeszcze nie odczytuje. Klasyczny edytor pozostaje dostępny; kopiowanie do nowego modelu nie zmienia oryginału.

`AthleteProfile.perks` i `HillProfile.perks` przechowują trzy liczby całkowite z zakresu −3…3. Zero oznacza brak perka; znak wybiera stronę pary; wartość bezwzględna oznacza poziom I, II lub III. Dotychczasowe profile skoczków pozostają zgodne. Poziom II jest wzmocniony, a III potężny i ma odrębną fioletowo-złotą oprawę.

Skocznie są zapisywane atomowo w katalogu `hill-studio` wewnątrz `QStandardPaths::AppDataLocation`, w osobnych plikach paczek JSON. Import tworzy nowe identyfikatory paczki i profili. Edycja, usuwanie i zamiana perka obsługują cofanie i ponawianie.

Pary perków skoczni:

| Para | Strona dodatnia | Strona ujemna |
| --- | --- | --- |
| Charakter | Katapulta | Królestwo lotników |
| Lądowanie | Przyjazny zeskok | Bez marginesu |
| Wiatr | Wietrzna ruletka | Spokojne niebo |

Model domenowy udostępnia preferencję wybicia/lotu (±0,15 / 0,30 / 0,55), próg trudnego lądowania i względną zmienność wiatru. Przyjazny zeskok podnosi próg o 3/6/10% HS, a Bez marginesu obniża go o 1,5/3/5% HS. Zmienność wynosi 1,35/1,8/2,5 dla ruletki oraz 0,75/0,5/0,25 dla stabilności. To parametry przygotowane dla przyszłej symulacji, a nie zmiany bieżącego silnika.

Test `--hill-studio-only` obejmuje walidację, poziomy perków, niezależność od danych symulacji, zapis, import/eksport, cofanie oraz widok przy małym oknie. `--athlete-patches-only` sprawdza edytor skoczka i przechodzenie przez wszystkie trzy poziomy.
