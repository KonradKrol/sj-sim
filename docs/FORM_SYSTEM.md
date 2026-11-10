# Sj.Sim 2.0 — Autonomiczny system formy zawodników

**Specyfikacja implementacyjna v1.0 — 10 listopada 2026**

**Cel:** stworzenie realistycznego, probabilistycznego generatora formy zawodników, opartego na łańcuchu Markova, procesie Ornsteina–Uhlenbecka i rozkładzie Laplace’a.

**Zakres pierwszej implementacji:** autonomiczny generator + integracja z symulacją opartą na datach + interfejs kalibracyjny.

---

## 1. Fundamentalne założenia

### 1.1. Overall — OVR

- Wartość 1–100.
- Określa fundamentalny poziom sportowy zawodnika.
- Ustalany i modyfikowany wyłącznie przez użytkownika.
- Generator formy nie może automatycznie zmieniać OVR.

### 1.2. Aktualna forma — F

- Wartość 1–100.
- Odzwierciedla bieżącą dyspozycję zawodnika względem jego własnego potencjału.
- 50 oznacza dyspozycję neutralną.
- 90–100 oznacza wyjątkową formę życia.
- Nie zależy bezpośrednio od OVR. Zawodnik z OVR 60 może mieć formę 95, podobnie jak zawodnik z OVR 95.
- Jest to wartość wyświetlana użytkownikowi, chyba że ustawienia widoczności stanowią inaczej.

### 1.3. Ukryta baza formy — μ

- Zmienna długoterminowa, wokół której oscyluje aktualna forma F.
- Może być wysoka, nawet gdy F chwilowo spadnie.
- Może się powoli zmieniać bez przechodzenia do nowego reżimu.
- Jej wartość jest w pierwszej wersji zarządzana automatycznie.

### 1.4. Reżim Markova

Pięć stanów opisujących długotrwałe okresy dyspozycji:

| ID | Reżim | Robocze centrum μ |
|---|---|---:|
| 0 | Głęboki kryzys | 20 |
| 1 | Słaby okres | 37 |
| 2 | Typowy okres | 50 |
| 3 | Dobry okres | 70 |
| 4 | Szczytowy okres | 89 |

Reżimy określają charakter długoterminowej dyspozycji, nie sztywne wartości formy.

Wartości centrów są parametrami konfigurowalnymi.

---

## 2. Model matematyczny

System powinien składać się z trzech współpracujących mechanizmów.

### 2.1. Łańcuch Markova — zmiany reżimu

Zastosować model Markova w czasie ciągłym (CTMC).

Każdy reżim posiada:

- Średni czas przebywania w nim.
- Prawdopodobieństwa przejścia do innych reżimów.
- Centrum μ.
- Parametry losowania nowej bazy μ.

Po upływie losowego czasu zawodnik może zmienić reżim.

**Założenia:**

- Wszystkie przejścia są matematycznie możliwe.
- Przejścia do sąsiednich reżimów są zazwyczaj częstsze niż skrajne przeskoki.
- Długotrwałe kryzysy i okresy formy życia muszą być możliwe.
- Żaden reżim nie ma sztywnego maksymalnego czasu trwania.
- Przejścia nie są uzależnione od liczby konkursów.
- Zmiana reżimu nie powoduje bezpośredniego nadpisania aktualnej formy F.

CTMC może zostać zaimplementowany przez losowanie czasu do kolejnego przejścia z rozkładu wykładniczego i następnie losowanie stanu docelowego z odpowiedniego wiersza macierzy.

Średnie czasy trwania i macierz przejść mają być konfigurowalne. Dotychczas omawiana symetryczna macierz stanowi punkt wyjścia do kalibracji, nie ostateczny zestaw parametrów.

### 2.2. Długoterminowa baza μ

Przy wejściu w nowy reżim losowana jest nowa baza μ.

Losowanie powinno:

- Uwzględniać charakter nowego reżimu.
- Preferować wartości w okolicach jego centrum.
- Uwzględniać poprzednią bazę, tak aby mniejsze zmiany były zwykle bardziej prawdopodobne.
- Dopuszczać rzadkie, duże zmiany.

**Dodatkowo μ może dryfować bez zmiany reżimu.**

Proponowany mechanizm:

\[
\mu_{t+\Delta t}=m_r+a_\mu(\mu_t-m_r)+\varepsilon_\mu
\]

gdzie:

- \(m_r\) — centrum aktualnego reżimu;
- \(a_\mu=2^{-\Delta t/h_\mu}\);
- \(h_\mu\) — duży half-life;
- \(\varepsilon_\mu\) — niewielkie losowe zakłócenie.

Dryf powinien być stosunkowo słaby, aby baza pozostawała długoterminowa.

Granice reżimów nie muszą być całkowicie sztywne. Niewielkie nakładanie się zakresów jest dopuszczalne.

### 2.3. Proces OU — zmiany aktualnej formy

Aktualna forma F oscyluje wokół bazy μ.

Roboczy mechanizm:

\[
F_{t+\Delta t}=\mu+a_F(F_t-\mu)+\varepsilon_F
\]

gdzie:

\[
a_F=2^{-\Delta t/h_F}
\]

- \(h_F\) — half-life aktualnej formy.
- \(\varepsilon_F\) — losowy składnik wykorzystujący rozkład Laplace’a.

Rozkład Laplace’a pozwala uzyskać cięższe ogony niż klasyczny rozkład normalny.

Należy zachować jego skalę zmienności w czasie. W pierwszej wersji dopuszcza się **wewnętrzne kroki jednodniowe**, które uproszczą implementację i kalibrację niegaussowskich zakłóceń.

Dla kroku dziennego można zastosować:

\[
\varepsilon_F=b_F\sqrt{1-a_F^2}\,L
\]

gdzie \(L\) jest scentrowaną zmienną Laplace’a o wariancji 1, a \(b_F\) docelową skalą odchylenia standardowego.

Należy pamiętać, że przy ruchomej μ nie jest to dokładne rozwiązanie klasycznego procesu OU, tylko jego dyskretny, niegaussowski odpowiednik.

**Istotne:** aktualizacja przy długiej przerwie musi uwzględniać ewentualne zmiany reżimu i bazy podczas tej przerwy.

---

## 3. Nowy perk — zatwierdzony

### Równa forma ↔ Chwiejna forma

Dziewięć poziomów:

| Wartość | Znaczenie |
|---|---|
| −4 | Równa forma IV |
| −3 | Równa forma III |
| −2 | Równa forma II |
| −1 | Równa forma I |
| 0 | Neutralna |
| +1 | Chwiejna forma I |
| +2 | Chwiejna forma II |
| +3 | Chwiejna forma III |
| +4 | Chwiejna forma IV |

**Pierwszy wpływ — OU:**

- Równa forma zmniejsza losową zmienność F.
- Chwiejna forma zwiększa losową zmienność F.
- Efekt wzrasta stopniowo z poziomem perka.

**Drugi wpływ — Markov:**

- Równa forma istotnie zmniejsza prawdopodobieństwo wejścia w głęboki kryzys.
- Jednocześnie nieco zmniejsza prawdopodobieństwo wejścia w szczytowy reżim.
- Chwiejna forma działa odwrotnie.
- Zmiany prawdopodobieństw należy odpowiednio normalizować.
- Nie modyfikować bezpośrednio średniego czasu trwania reżimów.

**Balans:** równa forma ma być statystycznie trochę korzystniejsza od neutralnej, ponieważ redukcja ryzyka kryzysów powinna przewyższać utratę szans na formę życia.

Perk nie może być utożsamiany z istniejącą cechą „Regularny ↔ Nieregularny”, wpływającą na pojedyncze skoki.

Wszystkie współczynniki poziomów perka są konfigurowalne i wymagają kalibracji.

---

## 4. Inicjalizacja formy zawodników

Zawodnicy posiadają już aktualną formę ustawioną przez gracza w edytorze bazy danych.

**Podczas pierwszego rozpoczęcia symulacji:**

1. Zachować istniejącą formę dokładnie jako F₀.
2. Wylosować bazę μ₀ w pobliżu F₀.
3. Najczęściej różnica między F₀ a μ₀ powinna być niewielka, ale sporadycznie dopuszczalne są większe różnice.
4. Przypisać początkowy reżim na podstawie μ₀.
5. Zainicjalizować stan generatora i jego źródło losowości.

Nie należy resetować formy wszystkich zawodników do 50.

**Ważne:** inicjalizacja nie może sprawić, że zawodnik z formą 90 niemal zawsze natychmiast straci swoją dyspozycję.

Przy wczytywaniu istniejącego zapisu należy odtworzyć poprzedni stan F, μ, reżimu i generatora, a nie ponownie go losować.

---

## 5. Integracja z czasem i kalendarzem — zakres MVP

### 5.1. Generowanie tylko przy aktywnych datach

W pierwszej wersji **automatyczny generator działa wyłącznie w symulacjach wykorzystujących daty wydarzeń**.

Jeżeli symulacja nie wykorzystuje dat:

- Automatyczne generowanie formy jest niedostępne.
- Forma pozostaje niezmieniona przez nowy silnik.
- Nie uruchamia się alternatywny generator oparty na liczbie konkursów.
- Zwykła ręczna edycja danych zawodnika pozostaje możliwa w dotychczasowym edytorze.

**Przyszłość:** wprowadzić osobny tryb manualny lub półautomatyczny dla symulacji bez dat, np. „Zaktualizuj formę po weekendzie”.

Nie implementować tego teraz.

### 5.2. Automatyczne aktualizacje

Przy włączonym generatorze:

- System ustala datę bieżącego wydarzenia.
- Oblicza upływ czasu od ostatniego przetworzonego dnia.
- Aktualizuje stan wszystkich zawodników objętych generatorem.
- Zmiany powinny być wykonane przed symulacją skoków przypisanych do nowej daty.
- Kilka konkursów tego samego dnia nie powinno wielokrotnie naliczać tego samego czasu.
- Przerwy między konkursami należy uwzględniać.
- Stan ostatniej aktualizacji musi być zapisywany.

Nie należy automatycznie traktować długiej przerwy jako końca sezonu ani resetować formy. Zachowanie między formalnymi sezonami pozostaje odłożoną decyzją.

Należy obsłużyć także wczytywanie zapisów, wyłączanie generatora oraz zapobiec podwójnemu naliczaniu tego samego okresu.

---

## 6. UI — wyraźny przełącznik automatycznej formy

**Lokalizacja:** widoczny obszar zarządzania sezonem, w którym gracz rozpoczyna i prowadzi symulację.

Przełącznik:

**Automatyczna forma zawodników** — WŁ. / WYŁ.

Powinien być:

- Łatwy do odnalezienia.
- Czytelny i estetyczny.
- Dostępny bez zagłębiania się w zaawansowane ustawienia.
- Domyślnie wyłączony w istniejących zapisach, aby uniknąć nieoczekiwanej zmiany zachowania.
- Niedostępny lub odpowiednio wyjaśniony w symulacji bez dat.

**Proponowany opis dla użytkownika:**

> Gdy automatyczna forma jest włączona, dyspozycja zawodników zmienia się samodzielnie wraz z upływem czasu. Zawodnicy mogą przeżywać okresy świetnej formy, stopniowe wzrosty i spadki oraz nieoczekiwane kryzysy. System uwzględnia indywidualne cechy każdego skoczka.
>
> Funkcja wymaga korzystania z dat w symulacji.

Bez tłumaczenia Markova, OU, μ, rozkładów czy innych parametrów.

**Zachowanie przełącznika:**

- Włączenie rozpoczyna lub wznawia automatyczne aktualizacje.
- Wyłączenie zatrzymuje generator.
- Wyłączenie nie resetuje dotychczasowej formy.
- Ponowne włączenie powinno wznowić pracę od aktualnej daty, bez automatycznego „nadrabiania” czasu, gdy funkcja była wyłączona.
- Nie wolno ponownie inicjalizować μ i reżimu przy zwykłym przełączeniu.

---

## 7. Form Engine Lab — PRIORYTET IMPLEMENTACYJNY

**Obowiązkowe narzędzie do kalibracji silnika, tworzone od początku prac.**

Może być dostępne w osobnym widoku deweloperskim lub zaawansowanym.

### 7.1. Konfiguracja parametrów

Interfejs powinien umożliwiać edycję:

- Centrów i rozkładów μ dla pięciu reżimów.
- Macierzy przejść Markova.
- Średnich czasów przebywania w reżimach.
- Half-life F i μ.
- Amplitudy losowego dryfu μ.
- Amplitudy zakłóceń Laplace’a.
- Parametrów początkowego losowania μ.
- Współczynników dziewięciu poziomów nowego perka.
- Pozostałych współczynników używanych przez generator.

### 7.2. Symulacja

Użytkownik narzędzia określa:

- Datę początkową i końcową lub długość okresu, np. 150 dni.
- Liczbę zawodników.
- Początkową formę i ewentualne cechy zawodników.
- Seed losowości.
- Konfigurację silnika.

Po uruchomieniu otrzymuje wyniki symulacji.

### 7.3. Wizualizacje

Minimum:

1. Wykres F na przestrzeni czasu.
2. Wykres μ na przestrzeni czasu.
3. Wizualizacja zmian reżimu Markova.
4. Porównanie kilku zawodników.
5. Zbiorcze statystyki populacji.

### 7.4. Cele kalibracyjne

Dla 100 zawodników w okresie 150 dni:

- Około **3–6 zawodników** powinno przynajmniej raz uzyskać F ≥ 90.
- Przeciętnie **1,5–2,5 odwiedzonego reżimu** na zawodnika (licząc stan początkowy).
- Nagłe zmiany mają być rzadkie, ale zauważalne.
- Typowe przebiegi formy powinny być płynne, lecz nie całkowicie przewidywalne.
- Długotrwała forma życia, kryzysy oraz niespodziewane przełomy muszą być możliwe.
- Równa forma powinna nieco poprawiać przeciętne wyniki względem neutralnej.

To cele statystyczne, nie sztywne limity. Należy testować wiele seedów i przedziały niepewności, zamiast oczekiwać identycznych wyników każdej symulacji.

### 7.5. Funkcje dodatkowe

- Możliwość ponowienia symulacji przy identycznym seedzie.
- Porównywanie dwóch zestawów parametrów.
- Zapis i eksport konfiguracji.
- Możliwość wykorzystania skalibrowanej konfiguracji w właściwym symulatorze.
- W miarę możliwości prezentowanie informacji o odchyleniu wyników od założonych celów.

---

## 8. Architektura implementacji

Generator powinien być wydzielony z UI i systemu wyników skoków.

Sugerowany podział:

- **FormEngineConfig** — parametry modelu.
- **AthleteFormState** — F, μ, reżim, informacje potrzebne do dalszej symulacji.
- **FormEngine** — autonomiczne aktualizacje.
- **FormInitializer** — inicjalizacja z danych zawodników.
- **FormSimulationRunner** — uruchamianie eksperymentów kalibracyjnych.
- **FormEngineLab** — interfejs kalibracyjny.
- **FormPersistence** — zapis i odtwarzanie stanów.

Silnik powinien przyjmować czas i stan zawodnika, ale nie powinien wymagać dostępu do klas konkretnego widoku UI.

Należy zachować kompatybilność z istniejącymi strukturami Sj.Sima, unikając zbędnego przepisywania systemu zawodników.

**Wymagania techniczne:**

- Deterministyczna losowość przy identycznym seedzie i stanie.
- Trwałość stanu pomiędzy zapisami gry.
- Wewnętrzna precyzja zmiennoprzecinkowa.
- Ograniczenie prezentowanej formy do zakresu 1–100.
- Brak niekontrolowanych skoków wynikających wyłącznie ze zmiany częstotliwości aktualizacji.
- Testy jednostkowe i symulacyjne.
- Rozdzielenie logiki silnika od wizualizacji.

---

## 9. Poza zakresem obecnej implementacji

**Nie implementować teraz:**

1. Osobnego huba ręcznego zarządzania formą.
2. Masowej zmiany F przez gracza.
3. Przycisków typu „forma życia”, „obniż formę”, „przywróć formę”.
4. Półautomatycznego trybu bez dat.
5. Zaawansowanego systemu przygotowania do sezonu.
6. Automatycznego rozwoju OVR.
7. Rozbudowanych mechanizmów zmęczenia.
8. Dynamicznej stabilizacji pojedynczych skoków.
9. MCP lub sterowania językiem naturalnym.
10. Mechanizmu resetowania formy na początku sezonu.

Te funkcje mogą trafić do późniejszych wersji.

---

## 10. Plan implementacji dla agenta

### Etap 1 — analiza obecnego projektu

- Odnaleźć dotychczasowy generator formy.
- Sprawdzić struktury zawodników, zapisów i kalendarza.
- Sprawdzić istniejącą logikę stosowania formy do wyników.
- Ustalić bezpieczny sposób integracji, bez zmieniania istniejących reguł OVR.
- Zachować zgodność wsteczną z zapisami.

### Etap 2 — niezależny silnik matematyczny

- Zaimplementować Markova.
- Zaimplementować losowanie i dryf μ.
- Zaimplementować proces F.
- Zaimplementować perk.
- Dodać inicjalizację i deterministyczny generator losowy.
- Utworzyć podstawowe testy.

### Etap 3 — Form Engine Lab

- Zbudować konfigurowalny interfejs.
- Dodać symulację na przestrzeni czasu.
- Dodać wykresy i metryki.
- Uruchomić testy populacyjne.
- Skalibrować parametry i wyznaczyć konfigurację domyślną.

### Etap 4 — integracja z Sj.Simem

- Powiązać generator z datami.
- Dodać przełącznik w widoku sezonu.
- Dodać prostą instrukcję.
- Zintegrować zapisywanie stanu.
- Zweryfikować działanie podczas długich przerw i wczytywania zapisów.

### Etap 5 — walidacja

- Symulować wielu zawodników przez długie okresy.
- Przeanalizować nietypowe historie formy.
- Sprawdzić wpływ perka.
- Sprawdzić rozkład formy 90+.
- Sprawdzić, czy działanie z datami jest niezawodne.
- Wyeliminować niezamierzone gwałtowne przeskoki i błędy zapisu.

---

## 11. Kryteria ukończenia MVP

System można uznać za gotowy, gdy:

- Zawodnicy rozpoczynają symulację z formą ustawioną w bazie.
- Generator poprawnie inicjalizuje μ i reżim.
- Forma zmienia się automatycznie wraz z datami.
- Przełącznik w widoku sezonu działa i zachowuje stan.
- Symulacje bez dat nie korzystają z nowego generatora.
- Perk Równa ↔ Chwiejna forma działa na OU i Markova.
- Stan generatora jest poprawnie zapisywany i odtwarzany.
- Form Engine Lab umożliwia kalibrację wszystkich parametrów.
- Testy statystyczne pozwalają zweryfikować ustalone cele.
- Zwykły użytkownik nie musi znać mechanizmów matematycznych.

**Nadrzędny cel:** generator ma w autonomiczny sposób tworzyć wiarygodne, różnorodne i nieprzewidywalne historie dyspozycji zawodników, pozostając prostym w obsłudze.