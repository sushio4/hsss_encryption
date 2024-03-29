# HSSS - Hash Salt Shift by Suski encryption algorithm

This acronym should be pronounced like a hiss of a cat.

## Usage

You can encrypt files like this  
``./hsss file1 file2 -e password``  
And decrypt like this  
``./hsss file1.hsss file2.hsss -d password``  

Also, you can give input as an argument, use `-t` for this  
Encryption:  
``./hsss -t "secret text" -e password``  
Decryption:   
``./hsss -t 98557d3663a7cd24ed97e7e44a851a334fc17887590fba5041d489 -d password``  

## How it works

This algorithm is based on the caesar cipher but the offset of each number is different. The procedure is as follows:

- add a salt to the password
- hash it to an 8 bit number
- add that value to the currently encrypted byte (like caesar)
- shift the password one character to the left (rotate)

Each character being offseted by a different amount greatly reduces the risk of a successful frequency analysis of a ciphertext, even though this is cyclic (cycle has the length of a password), as long as the attacker has no information on the password used.

### Salt

It has a constant length of 16 bytes and it's generated pseudo-randomly. It sits at the beginning of encrypted data and is appended to the password before hashing. Salt prevents attacks based on precomputed hash tables such as rainbow tables.

``[ password ][ salt ]``

### Hashing

It is a Merkle–Damgård construction with my own one way compression function

![There should be diagram](/img/HSSS_hash.drawio.png)

### One way compression function

It's a function that takes two 8 bit numbers and outputs one, its safety is based on how hard it is to solve higher degree polynomials. It works as follows, subscripts indicate singular bits:

a - input 1  
b - input 2  
c - output  

c = a<sub>7</sub>b<sup>8</sup> + a<sub>6</sub>b<sup>7</sup> + a<sub>5</sub>b<sup>6</sup> + a<sub>4</sub>b<sup>5</sup> + a<sub>3</sub>b<sup>4</sup> + a<sub>2</sub>b<sup>3</sup> + a<sub>1</sub>b<sup>2</sup> + a<sub>0</sub>b + a + b (mod 256)

The output of previous compression is input a, and the next byte to hash is input b.

### Encryption

To encrypt one byte of data, you hash salted password and add it modulo 256 to the data byte. After that, you shift the password and go to the next character.  

![There should be diagram](/img/HSSS_encrypt.drawio.png)

Decryption is the same but you subtract the hash.

![There should be diagram](/img/HSSS_decrypt.drawio.png)

### Shifting

When you shift the password, all its characters go one to left, except for the first one, which goes on the last place.  
Example:  
```
password = admin
1st iteration:
    admin (salt)
2nd iteration:
    dmina (salt)
3rd iteration
    minad (salt)
```
