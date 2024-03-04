# HSSS - szyfrowanie Hash Salt Shift by Suski 

Ten akronim powinien być wymawiany jak syczenie kota.

## Używanie

Szyfrowanie plików wygląda w ten sposób  
``./hsss file1 file2 -e password``  
A odszyfrowanie w ten  
``./hsss file1.hsss file2.hsss -d password``  

Można również zaszyfrować tekst podając go przez argument z opcją `-t`  
Szyfrowanie:  
``./hsss -t "secret text" -e password``  
Odszyfrowanie:   
``./hsss -t 98557d3663a7cd24ed97e7e44a851a334fc17887590fba5041d489 -d password``  

## Zasada działania

Ten algorytm jest bazowany na szyfrze cezara, ale przesunięcie każdego bajtu jest inne. Procedura wygląda następująco:

- dodaj sól do hasła
- zahaszuj do 8 bitowej liczby
- dodaj tą wartość do aktualnie szyfrowanego bajtu (jak cezar)
- przesuń hasło o jeden znak w lewo (rotate)

Dzięki temu, że każdy znak jest przesuwany o inną ilość, analiza częstotliwości jest znacznie utrudniona, mimo, iż wartości przesunięć są cykliczne (długość równa długości hasła), tak długo, jak atakujący nie ma żadnej informacji o użytym haśle.

### Sól

Ma stałą długość 16 bajtów i jest generowana pseudo-losowo. Jest na samym początku szyfrogramu i jest dodawana do hasła przed haszowaniem. Sól powstrzymuje ataki bazujące na gotowych tabelkach haszy (np rainbow tables).

``[ hasło ][ sól ]``

### Haszowanie

Jest zrobiony na podstawie konstrukcji Merkle–Damgård z moją własną jednostronną funkcją kompresji

![Tu powinien być diagram](/img/HSSS_hash_pl.drawio.png)

### Jednostronna funkcja kompresji

To funkcja która bierze za argumenty dwie 8 bitowe liczby i zwraca jedną, a jej bezpieczeństwo gwarantuje trudność w rozwiązywaniu wielomianów wyższych stopni. Działa następująco, indeksy wskazują konkretne bity liczby:

a - argument 1  
b - argument 2  
c - zwracana wartość  

c = a<sub>7</sub>b<sup>8</sup> + a<sub>6</sub>b<sup>7</sup> + a<sub>5</sub>b<sup>6</sup> + a<sub>4</sub>b<sup>5</sup> + a<sub>3</sub>b<sup>4</sup> + a<sub>2</sub>b<sup>3</sup> + a<sub>1</sub>b<sup>2</sup> + a<sub>0</sub>b + a + b (mod 256)

Wartość zwrócona przez poprzednią iterację staje się argumentem a, a kolejny bajt do haszowania jest argumentem b.

### Szyfrowanie

Żeby zaszyfrować jeden bajt danych, trzeba zahaszować posolone hasło i dodać hasz modulo 256 do bajtu danych. Po tym trzeba przesunąć hasło o jeden znak w lewo.

![Tu powinien być diagram](/img/HSSS_encrypt_pl.drawio.png)

Odszyfrowanie wygląda analogicznie, z tym, że hasz się odejmuje.

![Tu powinien być diagram](/img/HSSS_decrypt_pl.drawio.png)

### Przesuwanie

Gdy przesuwamy hasło, wszystkie znaki zmieniają miejsce o jeden w lewo, oprócz pierwszego znaku, który staje się ostatnim  
Przykład:   
```
hasło = admin
1 iteracja:
    admin (sól)
2 iteracja:
    dmina (salt)
3 iteracja
    minad (salt)
```
