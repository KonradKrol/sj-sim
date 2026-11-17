# Jak tworzyć i publikować paczki w Sj.Sim 2

## 1. Przygotuj zawartość

W edytorze bazy danych utwórz lub zmodyfikuj paczkę skoczków, skoczni, zasad albo punktacji. Nadaj jej rozpoznawalną nazwę i zapisz zmiany.

**Kalendarz:** przygotuj konkursy i klasyfikacje w sezonie, a następnie zapisz kalendarz jako preset. Do udostępnienia samego terminarza nie wysyłaj zapisu całej symulacji.

**Gdzie jest folder paczki?** W **Ustawienia → Biblioteka zawartości** kliknij **Otwórz folder**, następnie wejdź do `Packs/Athletes`. Każdy podfolder o długiej nazwie jest jedną paczką. W jego pliku `pack.json` pole `name` podaje nazwę paczki. Domyślna lokalizacja to `Dokumenty/SjSimData/Packs/Athletes/<identyfikator-paczki>/` na Windowsie i Linuxie.

**Zdjęcia:** umieść je w tym samym folderze co `pack.json` lub utwórz w nim podfolder `photos`. W edytorze wpisz względną ścieżkę, np. `photos/POL_JanKowalski.png`. Podczas eksportu istniejące pliki zdjęć zostaną dołączone do archiwum; sama wpisana ścieżka nie wystarczy.

## 2. Wyeksportuj paczkę

1. Otwórz **Ustawienia → Biblioteka zawartości**.
2. Zaznacz zasoby, które chcesz udostępnić — możesz wybrać kilka paczek i kalendarzy naraz.
3. Kliknij **Eksportuj** i zapisz plik `.sjsim-pack`, np. `Sezony-2023-2025.sjsim-pack`.

Eksport kalendarza dołącza wymagane skocznie, zasady i punktacje. Jeśli chcesz udostępnić również zawodników, zaznacz ich paczkę osobno. Nie trzeba ręcznie pakować folderów do ZIP-a.

## 3. Sprawdź i opublikuj

Sprawdź import w osobnej, pustej bibliotece: **Wczytaj paczkę**. Przy kalendarzu sprawdź także jego wczytanie do sezonu, skocznie, zasady, daty i powiązania kwalifikacji. Przed zmianą biblioteki zachowaj ścieżkę do swojej zwykłej biblioteki.

Wrzuć plik `.sjsim-pack` na kanał **#📦pobierz-paczki**. W opisie podaj:

- nazwę i zawartość, np. „Kalendarze LGP 2023 i 2024 oraz PŚ 2023/24 i 2024/25”;
- wymaganą wersję Sj.Sima i wersję swojej paczki;
- autora, źródła danych i ewentualne uproszczenia;
- przy aktualizacji — krótko, co zmieniłeś.

**Odbiorca:** pobiera plik, otwiera **Ustawienia → Biblioteka zawartości → Wczytaj paczkę** i wybiera pobrany plik. Preset kalendarza wczytuje następnie w edytorze kalendarzy.

Paczki ze starego Sj.Sima nie muszą być w pełni zgodne z 2.0.0. Najbezpieczniej udostępniać zasoby wyeksportowane i sprawdzone w aktualnej wersji gry.
