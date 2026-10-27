# Poprawki edytorów i tłumaczeń

1. Katalogi tłumaczeń są ładowane względem pliku wykonywalnego. Uruchomienie aplikacji z innym katalogiem roboczym nie blokuje języków EN/CS ani tłumaczeń kontrolek Qt.
2. Anulowane kliknięcie perka lub jego poziomu nie zmienia modelu. Zmiana następuje po zaakceptowanym kliknięciu.
3. Wyszukiwany dropdown respektuje wyłączone opcje także przy zatwierdzaniu klawiaturą.
4. Zatwierdzenie wyniku wyszukiwania aktualizuje wybraną pozycję dropdownu.
5. Wspólny dropdown używa ogólnych komunikatów wyszukiwania; wybór kraju i innych danych nie opisuje ich jako skoczni.
6. Pole punktu K ma taki sam górny limit 400 m jak model domenowy.
7. Niezatwierdzona wartość liczbowa poprzedniej skoczni jest odrzucana przy zmianie rekordu.
8. Lista skoczni zeruje indeks klawiatury, gdy wybrany rekord znika po filtrowaniu.
9. Zapis paczki skoczni respektuje limit 20 MiB używany podczas odczytu. Odrzucony zapis zachowuje poprzedni stan.
10. Niezatwierdzona ścieżka zdjęcia nie przechodzi do formularza innego skoczka.

Regresje: `--translation-only`, `--hill-studio-only`, `--athlete-patches-only`. Ogólne kontrole domeny: `--backend-only`.
