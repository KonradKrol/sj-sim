# LL między seriami, ręczny dobór KO i reset punktów

## Oznaczenia LL

Odtworzono scenariusz z rzeczywistego presetu **SuperKO by fortonse**: 20 → 12 → 6 → 3 zawodników. W pierwszej serii dziesięć grup daje dziesięciu zwycięzców i dwa miejsca LL. W drugiej sześć grup daje sześć miejsc bez LL; w trzeciej trzy grupy dają trzy miejsca bez LL.

Przyczyną dwóch zbędnych oznaczeń było łączenie bieżących lucky loserów z zapamiętanymi zawodnikami, którzy weszli przez LL po poprzedniej serii. Nowa seria KO pokazuje teraz wyłącznie swoich aktualnych LL. Zwykła seria następująca po KO nadal może oznaczać rzeczywiste wejście przez LL, zgodnie z wcześniejszym wymaganiem.

Dodatkowo limit LL uwzględnia faktycznie rozgrywaną liczbę serii, a nie tylko długość presetu. Jednoseryjne kwalifikacje z dłuższymi zasadami nie dostają LL z nieistniejącej następnej serii. Ujemny limit jest normalizowany do zera, a zerowy limit natychmiast czyści bieżącą listę LL. Sprawdzanie pewnego awansu i linii awansu używa tego samego efektywnego limitu.

## Ręczny dobór przed następną serią KO

Po zakończeniu serii, przed kolejną serią KO, obok przejścia dalej pojawia się **Grupy KO…**. Działa również wtedy, gdy preset przewiduje automatyczny dobór; nie zmienia zapisanych zasad. Ustawienie „Ręcznie” w zasadach także otwiera ten ekran przy zwykłym przejściu do następnej serii.

- Edytor używa dokładnie wspólnego obszaru grup z konfiguracji przed konkursem: wyszukiwalnego wyboru zawodnika, zamiany przez drag and drop, podglądu przeciąganej osoby i przewijania przy krawędzi.
- Zaczyna od losowego podziału. Pozwala przełączyć na klasyczny, klasyczny dla większych grup, losowy z koszyków lub losowy i ponowić losowanie. Klasyczny dla par przy większej grupie przełącza na wariant dla większych grup.
- Pula obejmuje wyłącznie zawodników zakwalifikowanych do następnej serii. Wcześniej natywny edytor dostawał też listę uczestników poprzedniej serii.
- **Rozpocznij serię** zatwierdza grupy. Cofnięcie/anulowanie pozostawia poprzednią serię i jej kartę skoku bez zmian.
- Otwarcie z Quick jest kolejkowane, aby nie przełączać widoków w trakcie wykonywania obsługi kliknięcia QML. Zachowano alternatywę Widgets.

## Karta skoku po resecie

Dla wybranej serii z resetem punktów znika przycisk **Wagi** i rozwijane obliczenia. W ich miejscu pozostaje dyskretne **Po resecie**. Historyczny wybór innej serii nadal korzysta z jej zasad.

## Sprawdzenie

- Regresje backendu: zerowy/ujemny limit, faktyczna ostatnia seria przy dłuższym presecie, brak przenoszenia LL do nowego KO i zachowanie poprawnego oznaczenia wejścia przez LL w zwykłej serii.
- `--superko-only`: pełne cztery serie rzeczywistego presetu, dwa miejsca LL w pierwszej, brak nieprawidłowych LL w drugiej/trzeciej — PASS.
- `--manual-ko-only`: wywołanie edytora mimo automatycznego presetu, anulowanie i ponowne otwarcie, losowy stan początkowy, wspólny interfejs, losowanie z koszyków, zatwierdzenie i brak osób spoza zakwalifikowanej puli — PASS.
- `--jump-history-only`: historyczne serie, wagi i ukrycie ich przy resecie — PASS dla konkursu indywidualnego i drużynowego.

Weryfikacja końcowa: pełny zestaw `sj-sim-ui-smoke` — PASS, 399 responsywnych renderów, bez ostrzeżeń o rekurencyjnych layoutach ani pętlach bindingów. Końcowe `make -C build-widgets-compat -j3` — kod 0.
