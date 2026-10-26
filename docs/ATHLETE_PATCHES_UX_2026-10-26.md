# Naszywki — kompaktowe perki, poziomy i zdjęcia

Rozwijany jest istniejący widok Naszywki. Dwa kolejne projekty odłożono. Pozostałe cztery prototypy pozostają dostępne; symulacja nie jest zmieniana.

## Interfejs

- Naszywki są domyślne po wejściu do nowego edytora.
- Tła, teksty, pola i kolory działań korzystają z Theme Sj.Sima w obu motywach. Barwy perków pozostają przede wszystkim wewnątrz samych naszywek.
- Zachowano duże OVR, mocne nagłówki i trzy oddzielne kategorie. W każdej są dwie wzajemnie wykluczające się opcje.
- Perki są oznaczone jako opcjonalne. Kliknięcie wybiera lub odznacza naszywkę, a wybór przeciwieństwa zastępuje poprzednią.
- Wzmocnij jest częścią wybranego przycisku perka. Złote Poziom II pokazuje wzmocniony stan i po kliknięciu przywraca poziom I. Nie ma osobnego wiersza ani segmented buttona z poziomami.
- Kliknięcie przypisanego perka albo jego widocznego × usuwa go. Operację można cofnąć jednym Cofnij.
- Opisy są w tooltipie/F1; każdy perk ma wyłącznie symbol, nazwę i stan.
- OVR i forma mają dyskretne +/−, działające od naciśnięcia i powtarzające zmianę po przytrzymaniu. Zakres pozostaje 1–100.

## Piktogramy

Spójny zestaw dla naszywek: stoper z gwiazdą (trening), stoper z pauzą (oszczędzanie wysiłku), skoczek w locie, profil skoczni, równe wyniki i wykres wahań. Poziom II ma podwójne złote oznaczenie. Źródłem jest `scripts/generate-athlete-patches.py`; generuje 18 SVG dla sześciu perków w trzech stanach.

## Zdjęcie względem folderu paczki

Profil zapisuje opcjonalne `photoPath`, np. `zdjecia/skoczek.jpg`. Ścieżka jest względna wobec folderu zawierającego `pack.json`. Ustawianie zdjęcia nie kopiuje ani nie konwertuje pliku. Otwórz folder paczki przyciskiem Folder, umieść tam obraz i wpisz ścieżkę lub wskaż go przez Wybierz…. Plik wskazany przez picker wewnątrz paczki jest zamieniany na ścieżkę względną.

Odwołania poza folder paczki są odrzucane. Brak pliku nie blokuje profilu; po jego pojawieniu się podgląd odświeża się automatycznie. Import i eksport paczki przenoszą istniejące wskazane obrazy z zachowaniem względnej struktury; brakujący obraz nie blokuje przeniesienia danych.

Poprzednie manifesty bez `photoPath` nadal się wczytują. Stare pliki `portraits/...png` pozostają odczytywane i nie są usuwane. Przy kopiowaniu klasycznego skoczka zachowywana jest nazwa pliku zdjęcia, którą można uzupełnić obrazem w folderze nowej paczki.

## Układ i weryfikacja

Długie ścieżki zdjęć ujawniły zależność szerokości pola od treści, prowadzącą do rekurencyjnego przeliczania layoutu. Pola otrzymały stałe wskazówki szerokości; podgląd zdjęcia nie narzuca szerokości przez rozmiar obrazu. Cyfry mają niezależną metrykę, a rozmiar fontu nie zależy zwrotnie od szerokości układu.

Weryfikacja lokalna na Linux/Qt 5.15.3: dedykowany test naszywek i pełne regresje pięciu prototypów zakończyły się PASS, bez ostrzeżeń rekurencyjnego layoutu. Budowa Qt Widgets również przeszła. Zakres:

- `scripts/test-quick-ui.sh --athlete-patches-only` — widok Naszywki i przypadki brzegowe nowego modelu.
- `scripts/test-quick-ui.sh --athlete-studio-only` — regresje pięciu istniejących prototypów, motywy, szerokości 360–1440, większy tekst i opcjonalne zdjęcia.
- Wybór i odznaczanie, osadzony przycisk wzmocnienia, usuwanie przypisanej naszywki, cofnięcie, F1 i brak opisów w układzie.
- OVR 100, wpisywanie wartości, pojedyncze +/− i powtarzanie po przytrzymaniu.
- Ścieżka wewnątrz paczki, odmowa wyjścia poza nią, brak zdjęcia i późniejsze pojawienie się pliku, zachowanie zdjęć przy przenoszeniu paczki, odczyt poprzednich manifestów i rzeczywiste wpisanie ścieżki w UI.

Budowa Qt Widgets pozostaje obsługiwana. Silnik i dane sezonów nie są modyfikowane.
