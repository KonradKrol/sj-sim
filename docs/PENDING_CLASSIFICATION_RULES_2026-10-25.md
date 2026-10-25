# Oczekujące decyzje dotyczące reguł klasyfikacji

Użytkownik zapowiedział odpowiedzi później i polecił pracować nad rzeczami niezależnymi od odpowiedzi. Poniższych zmian logiki nie należy wdrażać przez domysł. Formularz, kompaktowa siatka punktacji, edycja sposobów w miejscu, tooltipy, granice pól wieku 1–100, dropdowny wyników, wstrzymywanie podglądu indywidualnego i usunięcie wyszukiwania drużyn są już wykonane.

1. **Rok używany do wieku:** stały rok rozpoczęcia sezonu, wiek przy pierwszych zdobytych punktach albo aktualny rok z zachowaniem uprawnienia zdobywających już punkty. Propozycja: stały rok rozpoczęcia sezonu. Wymaganie użytkownika: zawodnik zdobywający punkty w roku X może kontynuować w tej samej klasyfikacji tego samego sezonu w roku X+1. Po rozstrzygnięciu usunąć pole roku odniesienia z formularza i uwzględnić presety, zapisy, przejścia sezonów oraz parametry konkursów.
2. **Zakres filtra wieku:** wyłącznie punkty do danej klasyfikacji czy również prawo startu. Propozycja: wyłącznie punkty, bo konkurs może należeć do kilku różnych klasyfikacji.
3. **Podstawa „wyklucz TOP N kraju”:** zachować dotychczasową zasadę (indywidualna: cały ranking; krajów: wyniki pojedynczego konkursu), zawsze używać pojedynczego konkursu albo dodać wybór podstawy.
4. **Dorobek wykluczonych:** czy zawodnik wypadający z wykluczanego TOP N wraca do indywidualnego rankingu z pełnym dorobkiem, czy wykluczenie blokuje naliczanie w danym konkursie. Propozycja: zachować surowy dorobek i filtrować ranking.

Po odpowiedziach: dodać przełącznik uwzględniania/wykluczania najlepszych albo najsłabszych z kraju oraz implementację, zapis i testy odpowiadające uzgodnionej semantyce. Sprawdzić granice roku, awans zawodnika poza wiek w trakcie sezonu, nowe wejście do klasyfikacji, dokładne N przy remisach, N większe od liczby zawodników i cofanie/odtwarzanie ustawień. Nie usuwać wcześniej zebranych punktów ani powiązań kalendarza bez uzgodnionej reguły.
