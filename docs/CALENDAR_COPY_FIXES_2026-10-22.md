# Dodatkowe poprawki kopiowania kalendarzy

Gałąź: `feature/qml-competition-config`. Każdy błąd ma osobny commit i regresję.

## 1. Wyniki kopii konkursu wskazywały na konkurs źródłowy

Kopiowanie `CompetitionInfo` kopiuje również `CompetitionResults`, którego pole `competition` jest surowym wskaźnikiem. Zarówno tworzenie presetu, jak i import presetu nadawały nową tożsamość konkursowi i wynikom, ale pozostawiały wskaźnik wyników na konkurs źródłowy. Późniejsze `addJump` przypisywało nowym wynikom i skokom niewłaściwy konkurs, potencjalnie ze skocznią `nullptr` w presecie.

Po utworzeniu kopii pojemnik wyników jest przypinany do nowego konkursu. Dane źródłowego kalendarza i presetu pozostają niezależne.

Test przed poprawką wykazał niepoprawnego właściciela wyników kopii presetu. Po poprawce `scripts/test-quick-ui.sh --workflow-only` przechodzi; sprawdza zarówno konstrukcję presetu, jak i import, a także przypisanie nowo dodanego skoku do żywego konkursu kalendarza.

## 2. Duplikowanie bloku gubiło ustawienia treningów i serii próbnej

Quick kopiował główne zawody z ich ważnością skoków, ale treningi i serię próbną budował osobno tylko ze skocznią i zasadami. Ich niezależna ważność wracała do domyślnej wartości 5. Pomijane były też pozostałe ustawienia wydarzenia, np. wyjątkowa liczba serii, przypisania klasyfikacji i powiązania kwalifikacji; skocznia serii próbnej była brana z głównego konkursu.

Wspólne kopiowanie ustawień jest teraz stosowane do głównego wydarzenia i jego załączonych wydarzeń. Każde otrzymuje własny obiekt i tożsamość, bez rozegranych wyników, zachowując konfigurację źródła.

Regresja przed poprawką wykazała utratę ważności treningów 3,5 i serii próbnej 4,0. Po poprawce `scripts/test-quick-ui.sh --workflow-only` przechodzi; sprawdza także kompletność bloku, nowe tożsamości, niezależność edycji kopii i cofanie operacji.
