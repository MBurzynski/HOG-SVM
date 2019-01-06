# README #

### Opis ###

Program służy do obliczania cech HOG zbioru obrazów należących do różnych klas (wszystkie światła, światła czerwone, światła zielone, światła żółte, światła czerwono-żółte, próbki negatywne). Na podstawie obliczonych cech HOG trenowany jest SVM, który służy później do klasyfikacji próbek. Wytrenowany klasyfikator można następnie przetestować i określić jego skuteczność dla różnych próbek.

### Konfiguracja ###

* Język programowania: C++
* Kompilator: Qt Creator 3.6.0
* Wykorzystywane biblioteki: OpenCV v3.1

### Informacje wstępne###

Przed uruchomieniem programu należy się upewnić, czy w folderze projektu znajdują się następujące pliki.

* Plik YAML zawierający wcześniej dokonane zaznaczenia
* Plik wideo o takiej samej nazwie co plik YAML z poprzedniego punktu, z którego pochodzą zaznaczenia

Opcjonalnie do folderu projektu można dodać pliki zawierające obliczone już cechy HOG dla różnych kolorów świateł, jak i obliczone detektory. Poniżej linki do pobrania tych plików.

* Cechy HOG: https://drive.google.com/open?id=0B0yR5Zyj6exBS2x1bFNtcHpzcjA
* Detektory: https://drive.google.com/open?id=0B0yR5Zyj6exBTTJuUzI4VUtsLWM

### Instrukcja użytkowania ###

Po uruchomieniu programu, w zakładce obsługa pliku należy kliknąć przycisk "Otworz", a następnie wybrać plik YAML z zebranymi wcześniej zaznaczeniami. W tym wypadku jest to plik o nazwie "video1". Należy się również upewnić, że w folderze z projektem znajduje się plik wideo o tej samej nazwie, z którego pochodzą zebrane zaznaczenia. Po wykonaniu tej operacji w zakładce "Obsluga pliku" uaktywni się przycisk "Generuj" służący do wygenerowania próbek negatywnych wykorzystywanych w późniejszych krokach. Po kliknięciu tego przycisku wygenerują się próbki negatywne (jeśli próbki są już wygenerowane, to dostaniemy jedynie informację, że próbki zostały wygenerowane). W folderze z projektem próbki negatywne znajdują się w pliku YAML o nazwie "Negatives".
Po wykonaniu tej operacji można przejść do zakładki "HOG i SVM". Klikając przycisk "Klasyfikator wstępny" zostaną obliczone cechy HOG dla wszystkich świateł i wytrenowany zostanie klasyfikator odróżniający światła od nie-świateł. Postępy obliczania będą widoczne w konsoli kompilatora. Po zakończeniu obliczania cech HOG, wyniki zostaną zapisane do pliku YAML, a przy następnej próbie obliczania zostaną już wczytane z pliku. Po zakończeniu treningu w konsoli pojawi się komunikat "Trening ukończony", a powstały w ten sposób detektor zostanie zapisany do pliku YAML.
Klikając przycisk "Klasyfikator szczegółowy" zostaną obliczone cechy HOG dla świateł różnych kolorów i wytrenowany zostanie klasyfikator odróżniający kolor danego światła. Również w tym przypadku klasyfikator zostanie zapisany do pliku.
Po wytrenowaniu obydwu klasyfikatorów można przejść do zakładki "Testowanie". W zakładce tej należy postępować zgodnie z instrukcją widoczną w programie. Po wybraniu klasyfikatora i zestawu próbek należy kliknąć przycisk "Testuj" i obserwować wyniki testu w konsoli.