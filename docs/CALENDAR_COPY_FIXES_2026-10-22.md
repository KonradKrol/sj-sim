# Dodatkowe poprawki kopiowania kalendarzy

Gałąź: `feature/qml-competition-config`. Każdy błąd ma osobny commit i regresję.

## 1. Wyniki kopii konkursu wskazywały na konkurs źródłowy

Kopiowanie `CompetitionInfo` kopiuje również `CompetitionResults`, którego pole `competition` jest surowym wskaźnikiem. Zarówno tworzenie presetu, jak i import presetu nadawały nową tożsamość konkursowi i wynikom, ale pozostawiały wskaźnik wyników na konkurs źródłowy. Późniejsze `addJump` przypisywało nowym wynikom i skokom niewłaściwy konkurs, potencjalnie ze skocznią `nullptr` w presecie.

Po utworzeniu kopii pojemnik wyników jest przypinany do nowego konkursu. Dane źródłowego kalendarza i presetu pozostają niezależne.

Test przed poprawką wykazał niepoprawnego właściciela wyników kopii presetu. Po poprawce `scripts/test-quick-ui.sh --workflow-only` przechodzi; sprawdza zarówno konstrukcję presetu, jak i import, a także przypisanie nowo dodanego skoku do żywego konkursu kalendarza.
