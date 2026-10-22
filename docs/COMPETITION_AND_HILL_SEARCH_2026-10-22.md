# Wyszukiwanie skoczni i dopracowanie kart konkursu

- **Wybór skoczni:** dyskretne wyszukiwanie w rozwijanej liście, wyłącznie w edytorze kalendarzy i przed pojedynczym konkursem. Wyszukiwanie ignoruje wielkość liter, obsługuje strzałki/Enter/Escape i pusty wynik, zachowuje oryginalny indeks skoczni oraz współpracuje z kalendarzowym Cofnij. Pozostałe listy nie pokazują wyszukiwarki. Regresje obejmują oba miejsca i konfigurację przy 360×480.
- **Opcje konkursu:** po zmniejszeniu treści do mieszczącej się w panelu pozycja przewinięcia wraca do początku. Samodzielny Flickable nie jest już nadpisywany przez otaczający ScrollView. Kółko i przeciąganie nie uruchamiają przewijania mieszczącej się treści; prawdziwe przepełnienie pozostaje dostępne. Testuje się rzeczywiste zdarzenia wejściowe i zmianę wysokości panelu.
- **Największy awans:** od poprzedniego miejsca odejmuje się bieżące miejsce oraz liczbę zawodników oczekujących na skok. Oczekujący pochodzą z aktywnej listy serii/grupy, bez DNS i DSQ; odpadli nie zwiększają korekty. Podpowiedź pokazuje skorygowane miejsce. Podczas prezentacji na żywo wskaźnik czeka na ujawnienie wyniku. W drużynówce podstawą pozostają wszystkie skoki poprzedniej serii.
- **Zawodnik na belce:** od drugiej serii obok poprzedniego miejsca widać różnicę punktów po zakończonych seriach. Skoki oddane już w bieżącej serii nie zmieniają tej wartości. Strata jest dodatnia, przewaga nad najlepszym rywalem ujemna, remis wynosi zero. Pierwsza seria nie ma tej informacji.
- **Główna karta skoku:** pole „W serii” pokazuje miejsce za oglądany skok w jego serii, niezależnie od wcześniejszego wyłączenia tej informacji w ustawieniach szczegółów Widgets. W trybie na żywo pozostaje niewyznaczone do ujawnienia wyniku. Pozostałe dane i kompaktowa geometria karty są zachowane.
- Nowe teksty mają angielskie tłumaczenia.

Commity kluczowe: wyszukiwanie `d89b6f4` / `da2fcd0`, przewijanie `a894d4b`, awans `eef3818`, poprzednia strata i miejsce w serii `f95bcfa`, tłumaczenia `4b8322a`.

Test użytkownika: wyszukać i wybrać skocznię w obu wskazanych miejscach, anulować inny wybór, cofnąć zmianę skoczni w kalendarzu; w drugiej serii konkursu porównać poprzednią stratę i pole „W serii” z wynikami, także podczas Na żywo. Sprawdzić opcje konkursu po przejściu z dłuższej zawartości do krótszej.
