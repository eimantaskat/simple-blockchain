# Simple-Blockchain

## v0.2

### Atsisiuntimas ir paleidimas
   #### Kompiliujant source kodą
   1. Atsisiųskite ir sukompiluokite suorce kodą
      ```console
      > git clone https://github.com/eimantaskat/simple-blockchain.git
      > cd simple-blockchain
      > make
      ```
   1. Paleiskite programą
      ```console
      > cd <path to executable>
      > ./blockchain
      ```
   #### Atsisiunčiant sukompiluotą programą
   1. Atsisiųskite blockchain'o exe failą paspaudę [čia](https://objects.githubusercontent.com/github-production-release-asset-2e65be/553382685/4e05bea3-ce77-42c0-a95d-b5d0aac36731?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20221102%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20221102T113802Z&X-Amz-Expires=300&X-Amz-Signature=0296e53b08170507331190ee075210437a882ad10ea736ec3b2ebba84cefa2f6&X-Amz-SignedHeaders=host&actor_id=80033246&key_id=0&repo_id=553382685&response-content-disposition=attachment%3B%20filename%3Dblockchain.exe&response-content-type=application%2Foctet-stream)
   1. Paleiskite programą
      ```console
      > cd <path to executable>
      > ./blockchain
      ```

### Naudojimas
   Paleidus programą įveskite norimą komandą:
   ```console
      Commands:
            help                                    Show this view
            generateData                            Generate users and transactions
            showBlockchainInfo                      Show information about blockchain
            decentralizedMining                     Start decentralized mining
            mineBlock                               Mine one block
            mineAllBlocks                           Mine blocks until there are no transactions left
            outputBlock <block height>              Output block info to HTML file
            outputTransaction <transaction hash>    Output transaction info to HTML file
            showBlock <block height>                Get information about block
            showTransaction <transaction hash>      Get information about transaction
            quit                                    Close blockchain
   ```

   * help - parodomos visos galimos komandos su trumpais aprašymais
   * generateData - sugeneruojami duomenys testavimui: 1,000 naudotojų, 10,000 tranzakcijų. Jei dar grandinėje nėra nei vieno bloko, yra sukuriamas pirmasis blokas su tranzakcijomis, duodančiomis visiems naudotojams atsitiktinį valiutos balancą
   * showBlockchainInfo - į konsolę atspausdinama blockchain'o informacija: blokų skaičius, paskutinio bloko hash'as, versija, naudotojų ir tranzakcijų kiekis
   * decentralizedMining - "decentralizuotai" iškasti bloką. Yra sugeneruojami 5 miner'iai ir jie pamainomis bando iškasti bloką, kol kažkuriam pavyksta
   * mineBlock - iškasti vieną bloką
   * mineAllBlocks - kasti blokus tol, kol yra tranzakcijų
   * outputBlock \<block height> - išvesti bloko duomenis į HTML failą
   * outputTransaction \<transaction hash> - išvesti tranzakcijos duomenis į HTML failą
   * showBlock \<block height> - atspausdinti bloko duomenis į konsolę
   * showTransaction \<transaction hash> - atspausdinti tranzakcijos duomenis į konsolę
   * quit - uždaryti programą

## v0.1

### Atsisiuntimas ir build'inimas
   ```console
   > git clone https://github.com/eimantaskat/simple-blockchain.git
   > cd simple-blockchain
   > make
   ```

### Naudojimas
1. Palieskite programą:
```console
   > ./blockchain
```
2. Įveskite norimą komandą:  
```console
      Commands:
              help                    Show this view
              generateData            Generate users and transactions
              mineBlock               Mine one block
              mineAllBlocks           Mine blocks until there are no transactions left
              showBlock               Get information about block
              showTransaction         Get information about transaction
              quit                    Close blockchain
```