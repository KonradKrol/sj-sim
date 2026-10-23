# Czas otwierania ekranów na Windows

Otwieranie nowego ekranu tworzyło oddzielny silnik QML dla strony i ponownie kompilowało wspólne komponenty. Pomiar natywnego buildu Release wykazał około 1,8 s do pierwszej klatki edytora bazy. Profil QML wskazał powtarzaną kompilację `Workspace.qml`, `Node.qml` i `SeasonBar.qml` oraz tworzenie ich drzew obiektów.

Ekrany, nagłówek i stopka danej powłoki aplikacji używają teraz jednego silnika i jego pamięci skompilowanych komponentów. Każdy widok ma własny `QQmlContext` z własnym kontrolerem `ui`, więc otwieranie kolejnej strony nie podmienia kontrolera poprzedniej. Powrót do zawieszonej sceny odtwarza ją w tym samym kontekście. Usunięto też nadmiarowe odświeżenie kontrolera na końcu dołączania strony. Zmiana języka odświeża wspólny silnik tylko raz.

Widoki są usuwane przed silnikiem, a silnik przed końcowym zamykaniem `QApplication`. To istotne na Windows: usuwanie silnika dopiero jako dziecka aplikacji prowadziło w próbnym wariancie do oczekiwania na wątek wczytywania obrazów podczas zamykania. Taki wariant został odrzucony; gotowy plik EXE uruchamia się i zamyka kodem 0.

## Natywny benchmark

Qt 6.8.3, MSVC 2022, Release, 23.10.2026. Każdy ekran otworzono trzy razy w tej samej sekwencji; tabela podaje medianę czasu od rozpoczęcia konstruktora kontrolera do gotowej pierwszej klatki. Pomiar obejmuje krótki odstęp na przetworzenie zdarzeń oraz wymuszenie renderowania. Nie jest to obietnica identycznych czasów dla każdego komputera i rozmiaru bazy.

| Ekran | Przed | Po | Skrócenie |
| --- | ---: | ---: | ---: |
| Konfiguracja konkursu | 1318 ms | 864 ms | 34,4% |
| Edytor bazy | 1842 ms | 1112 ms | 39,6% |
| Lista zapisów sezonów | 472 ms | 162 ms | 65,7% |
| Konfiguracja pojedynczych skoków | 1128 ms | 646 ms | 42,7% |

Pozostaje koszt tworzenia istniejących kontrolerów Widgets i ich modeli; pierwsze wejście do komponentu nadal wymaga jego załadowania. Zmiana nie dodaje sztucznego opóźnienia ani animacji przejścia.

Walidacja na Windows Release: testy backendu, nawigacji i przycisków strony głównej, opcji konkursu, klasyfikacji, dat, paczek (import/eksport, zapis/anulowanie), wyglądu, resetowania punktów i interakcji. Osobno sprawdzono uruchomienie i normalne zamknięcie wdrożonego EXE bez ścieżki deweloperskiej Qt. Test paczek po wykonaniu zrzutów przy różnych rozmiarach przywraca okno do rozmiaru roboczego przed dalszym sprawdzaniem zapisu.

Zbudowany wariant `CONFIG+=quick_ui_smoke` obsługuje `--navigation-benchmark`. Otwiera cztery rodzaje ekranów, sprawdza współdzielenie silnika i odrębność kontekstów, a wyniki zapisuje w `release/ui-renders/navigation-benchmark.json`. Flaga `--qml-profile` włącza profilowanie wyłącznie w programie testowym; zwykła aplikacja nie włącza debugowania QML.

## Profil QML

Zarejestrowano scenariusz w trybie logicznym: 94 842 zdarzenia. Suma zakresów kompilacji wyniosła 16 078 ms, tworzenia 9808 ms, powiązań 2591 ms, JavaScript 1239 ms i obsługi sygnałów 468 ms. Zakresy zagnieżdżają się: **te sumy nie są czasem rzeczywistym ani całkowitym zużyciem CPU**. Profil logiczny nie obejmował metryk klatek, pamięci ani bufora obrazów. Szczegółowy lokalny raport i ślad znajdują się w ignorowanym katalogu `profiler/`.

Użyte mechanizmy: [silnik QQuickWidget i jego własność](https://doc.qt.io/qt-6.8/qquickwidget.html), [hierarchia kontekstów QML](https://doc.qt.io/qt-6.8/qqmlcontext.html), [tworzenie komponentu w kontekście](https://doc.qt.io/qt-6.8/qqmlcomponent.html#create). Ładowanie gotowego korzenia używa publicznego slotu `setContent`, którego zachowanie sprawdzono w [źródłach Qt 6.8.3](https://github.com/qt/qtdeclarative/blob/v6.8.3/src/quickwidgets/qquickwidget.cpp#L656).
