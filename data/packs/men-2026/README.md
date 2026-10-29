# Mężczyźni — PŚ 2025/26 i LGP 2026

Paczka do uzupełnienia: **98 zawodników**, posortowanych według kodu kraju,
wewnątrz kraju według nazwiska. Obejmuje sumę zdobywców punktów z końcowych
klasyfikacji Pucharu Świata 2025/26 i Letniego Grand Prix 2026.

W edytorze bazy danych, w sekcji skoczków, wybierz **Wczytaj paczkę** i wskaż
`data/packs/men-2026/pack.json`. Gra zapisze własną kopię paczki. Zmiany w tej
kopii nie zostaną nadpisane przez aktualizację repozytorium. Przed przeniesieniem
zmian na inny komputer wyeksportuj swoją paczkę z edytora.

Wszyscy mają **domyślne OVR 70, formę 70, lądowanie Poprawne i brak perków**.
To wartości robocze, nie ocena rzeczywistych umiejętności. Możesz je uzupełniać.

Ścieżki zdjęć mają postać `POL_KamilStoch.png`, względem folderu paczki.
Nazwy plików nie zawierają spacji, łączników ani znaków diakrytycznych.
Po imporcie użyj przycisku otwierającego folder paczki i umieść tam zdjęcia.
Brak pliku zdjęcia nie przeszkadza w edycji ani grze.

## Dane i źródła

Dla 91 zawodników wpisano rekord życiowy. Pozostałych siedmiu ma prawidłowo
ustawiony brak rekordu (`0`), zgodnie z potwierdzeniem autora projektu: Julijan Smid, Jakob Steinberger, Simon Steinberger, Yuto Nakamura,
Jan Galica, Enej Faletic i Henry Loher. Nie jest to lista zaległych rekordów do wyszukania. Nieustalony rok urodzenia
pozostaje pusty (`0`).

Nazwy i przynależność do rankingów pochodzą z FIS. Zachowano pisownię danych FIS,
która nie zawsze zawiera narodowe znaki. Rekordy i dodatkowe lata urodzenia
pochodzą z profili Skijumping.pl oraz zestawienia Skokinarciarskie.pl; są to
wartości podane przez te źródła, nie pełny audyt wszystkich skoków kariery.
Źródło każdej pozycji jest zapisane w `sources.json`. Identyfikatory profili
są stałe i wynikają z numeru zawodnika w bazie FIS.

- [Puchar Świata 2025/26 — końcowa klasyfikacja FIS](https://www.fis-ski.com/DB/ski-jumping/cup-standings.html?sectorcode=JP&seasoncode=2026&cupcode=WC&disciplinecode=ALL&gendercode=M)
- [Letnie Grand Prix 2026 — końcowa klasyfikacja FIS; sezon FIS 2027](https://www.fis-ski.com/DB/ski-jumping/cup-standings.html?sectorcode=JP&seasoncode=2027&cupcode=GP&disciplinecode=ALL&gendercode=M)
- [Zestawienie rekordów życiowych](https://www.skokinarciarskie.pl/index.php?a=rekordy&b=index&order=nazwisko&typ=pb)
