# Wybory wyników i czytelność składów

## Wyniki konkursu

- Wybór klasyfikacji ma etykiety „Wyniki drużynowe” i „Wyniki indywidualne”.
- Wybór stanu wyników ma „Wyniki na żywo” i „Wyniki z N serii”. Przy jednej dostępnej opcji jest ukryty. Gdy nie ma także ustawień drużynowych, znika cały zbędny przycisk wyboru wyników.
- Widoki drużynowe nazywają się „Tylko wyniki bieżącej drużyny”, „Tylko suma drużyny” i „Pokaż pełne wyniki”. Ustawienia nadal są zapamiętywane.
- Przyciski wyboru w szerokim nagłówku otwierają zwykłą niemodalną listę w dół przy wybranej kontrolce, bez wyszarzania ekranu. Lista jest renderowana w pełnej powierzchni konkursu pod nagłówkiem. Nie próbują już pomieścić popupu w osobnym, niskim QQuickWidget nagłówka. Krótki nagłówek nadal ma wspólne menu „Wyniki”, z którego te same wybory otwierają właściwą listę.
- Wybór wyników indywidualnych w drużynówce również wstrzymuje konkurs, tak samo jak podgląd poprzedniej serii. „Wróć do konkursu” przywraca jednocześnie bieżącą serię i wyniki drużynowe. Nie dodano żadnego wiersza nad tabelą.

## Drużyny przed konkursem

- Minimalna wysokość pozycji zwiększona o 20%, z dodatkowym uwzględnieniem wysokości tekstu przy powiększonych fontach lub większym składzie.
- Nazwiska składu są oddzielnymi, numerowanymi pozycjami w dwóch kolumnach. Nie tworzą jednego ciągu tekstu oddzielonego kropkami.
- Pozostają pełne podsumowania dla dostępności. Wyszukiwanie z listy drużyn przed konkursem zostało usunięte. Wyszukiwanie zawodników wewnątrz edytora składu nadal działa. Skład zmienia się po otwarciu drużyny.
- W edytorze kolejności zawodników cienki separator oddziela ostatnie miejsce podstawowego składu od pierwszego rezerwowego. Przeciąganie i zamiana miejsc działają dotychczasowym mechanizmem.

## Weryfikacja

- `--history-dns-only`: prawdziwe kliknięcie wyboru serii i opcji w powierzchni konkursu, sprawdzenie pełnej widoczności opcji pod nagłówkiem, powrót i ochrona symulacji; trzy rodzaje konkursów.
- `--team-setup-compact-only`: osobne dane nazwisk składu, otwieranie odrębnego edytora, brak sortowania i wyszukiwania, responsywne renderowanie.
- Alternatywa Qt Widgets nadal się buduje.

Weryfikacja końcowa po wszystkich poprawkach: pełny zestaw **399 renderów responsywnych — PASS**, bez ostrzeżeń o rekursywnym układzie, pętlach bindingów ani błędach JavaScript. Testy dedykowane klasyfikacji, zwykłych dropdownów i wstrzymywania podglądu oraz listy drużyn bez wyszukiwania — PASS. Alternatywny wariant Widgets zbudowany poprawnie.
