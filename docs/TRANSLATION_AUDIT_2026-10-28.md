# Kontrola tłumaczeń — 28.10.2026

## Naprawione i sprawdzone

„Zakończ konkurs” już miał wpisy EN/CS w katalogach TS. Brakowało jednak
odświeżania dynamicznego tekstu przy zmianie języka w otwartych zawodach.
`CompetitionManagerWindow::changeEvent` ponownie ustawia odpowiedni przycisk
końca konkursu / następnej serii / następnej grupy. Nie uruchamia pełnego
`retranslateUi`, który mógłby wyzerować wyświetlane bieżące wartości.

Regresja kończy rzeczywisty konkurs i sprawdza kolejno „Finish competition”,
„Ukončit závod”, „Zakończ konkurs”, z niezmienionym zapisem wyników. PASS Qt5.
Dodano po 40 brakujących tłumaczeń EN/CS dla nowych metod pomiaru, ich pomocy,
podglądu przed skokiem i zapisanych faz wiatru oraz przywracania profilu.
Skrypt budowy generuje nowe QM; Windowsowy skrypt pakowania i WINDOWS_SETUP.MD
również zawierają `lrelease` — same pliki TS obok starego EXE nie wystarczą.

Test zmiany języka ujawnił też pętlę wysokości stopki. Jej wysokość wynika teraz
z metryk czcionek, bez zależności od geometrii zagnieżdżonego layoutu naszywki
wersji. Dopasowanie wysokości chrome jest kolejkowane i chronione QPointer.
`--translation-only` PASS bez ostrzeżenia o pętli bindingów.

## Nadal do sprawdzenia — pełny audyt odłożony

Użytkownik pozwolił odłożyć szerszą kontrolę, aby nie zatrzymywać integracji silnika.
Nie deklarować, że cała aplikacja ma już kompletne tłumaczenia. Do sprawdzenia:

- synchronizacja `lupdate` z najnowszym QML/C++, brakujące i unfinished EN/CS;
- napisy i menu tworzone jednorazowo w C++, ich odświeżanie przy otwartym ekranie;
- nowe edytory profili, pogoda, nazwy scenariuszy, walidacja i komunikaty błędów;
- formatowanie liczb/daty, atrybuty dostępności i tooltipy po zmianie języka;
- świeży start EN/CS oraz przełączanie w trakcie całego podstawowego workflow.

Nie zastępować tego kontrolą samych katalogów TS: zgłoszony błąd miał tłumaczenie
w pliku, ale nie odświeżał wartości ustawionej w runtime.
