# Ostatni oddany skok i pewny awans — 24.10.2026

## Poprawione zachowanie

- Ostatnio oddany skok jest rozpoznawany po identyfikatorze przyjętego skoku w wynikach. Nie zależy od aktualnie zaznaczonego zawodnika, miejsca w tabeli ani zawodnika będącego już na belce.
- Identyfikator aktualizuje wspólna ścieżka dodawania wyników, więc dotyczy skoków ręcznych, symulacji N skoków, grupy drużynowej, serii i całego konkursu.
- W tabeli indywidualnej i grupie KO zaznaczony jest właściwy zawodnik. W drużynówce oznaczenie obejmuje podsumowanie właściwej drużyny i — jeśli rozwinięty — wiersz zawodnika, który oddał skok.
- Wynik ma niezależne niebieskie obramowanie i boczny znacznik. Kolor awansu, podium albo zaznaczenie innego wiersza nie usuwa tego oznaczenia.
- Oznaczenie przetrwa sortowanie wyników, przebudowę modelu oraz koniec grupy, serii i konkursu. Podczas animacji na żywo pozostaje zamrożone razem z tabelą i aktualizuje się przy ujawnieniu wyniku.
- Usunięty skok odwołanej serii nie pozostawia fałszywego oznaczenia. Próba dodania kolejnego skoku zdyskwalifikowanej drużyny również nie zmienia ostatniego przyjętego skoku.
- To przejściowa informacja prezentacji; format zapisów wyników pozostaje kompatybilny.

## Pewny awans

- Zielone tło ma pierwszeństwo przed hoverem i zaznaczeniem. Wciąż widać kursor oraz znacznik ostatniego skoku.
- Uwzględniony jest również pewny awans do zależnego konkursu (Q), a nie wyłącznie następnej serii.
- Zieleń jest wyraźniejsza w obu motywach. Pewne odpadnięcie nadal ma neutralne szare tło.
- Sam napis LL nie nadaje zieleni: przed zagwarantowaniem awansu oznacza bieżące premiowane miejsce. Opis dostępności i podpowiedź mówią o aktualnym miejscu lucky losera, nie obiecują pewnego awansu.

## Do ręcznego sprawdzenia

1. Oddaj kilka skoków, kliknij starszy wynik i sprawdź, że oznaczenie ostatniego skoku pozostaje przy właściwym zawodniku.
2. Przesymuluj N skoków, serię i cały konkurs. Sprawdź oznaczenie również po końcu serii.
3. W drużynówce przełącz wszystkie trzy tryby wyników. Rozwinięty widok ma oznaczać właściwego członka drużyny.
4. W KO porównaj oznaczenie ostatniego skoku w tabeli głównej oraz grupie, o ile ten zawodnik jest w aktualnie wyświetlanej grupie.
5. Zaznacz zawodnika z pewnym awansem i najedź na niego myszą: tło ma nadal być zielone. Ostatnio oddany skok może równocześnie mieć niebieski znacznik.
6. Włącz tryb na żywo: tabela nie powinna zdradzać nowego oznaczenia przed ujawnieniem wyniku.

## Testy

- Regresje tożsamości skoku: zmiana kolejności wyników, stary kursor, koniec serii, zamrożenie i ujawnienie, usunięty skok, zawodnicy drużyny i odrzucony skok po DSQ.
- Regresje QML: pewny awans przy zaznaczeniu, hoverze i znaczniku ostatniego skoku, awans z Q, drużyny oraz oba motywy; pewne odpadnięcie pozostaje neutralne.
- Rzeczywiste konkursy sprawdzają jedno oznaczone podsumowanie i obecność wizualnego znacznika, także po automatycznej symulacji.
- Dodatkowe regresje formatu dat: DD.MM.YYYY niezależnie od lokalizacji oraz poprawne przyjmowanie dat przestępnych w tym formacie przy zachowaniu ISO w danych.

Wynik końcowy: pełny zestaw regresji PASS, **391 renderów responsywnych ekranów**; kompilacja wariantu Qt Widgets PASS.
