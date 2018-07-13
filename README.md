# MatrixMultiplication
Questo programma si occupa di effettuare moltiplicazioni tra matrici in parallelo. È stato sviluppato in C basandosi sul protocollo MPI.

### Requisiti
Ambiente Linux
Libreria MPI
Make tool
### Installazione
Posizionarsi nella directory root del progetto ed invocare il comando
`make`

Nel caso sia necessario cancellare i compilati invocare
`make clean`

### Esecuzione
- processors: è il numero di processori con cui si vuole avviare il programma.
- hosts: è il nome del file contenente gli indirizzi delle macchine (o eventualmente il numero di slots).
- size: è il valore intero che descrive la lunghezza del lato della matrice. 
- testCycles: è il numero di cicli che deve effettuare l'algoritmo di Freivalds.

Per eseguire il programma invocare il comando
``mpirun -np processors -hostfile hosts MatrixMultiplication``


Il programma di default moltiplica delle matrici di size 800x800, se invece si desidera impostare la dimensione della matrice invocare il comando:
``mpirun -np processors -hostfile hosts MatrixMultiplication -s size``

Se si desidera eseguire il calcolo e verificarne il risultato invocare il seguente comando:

``mpirun -np processors -hostfile hosts MatrixMultiplication -s size -t testCycles``


## Il problema
Il problema per cui è stato sviluppato il progetto riguarda la moltiplicazione di matrici in parallelo. L'algoritmo risolutivo doveva essere sviluppato in linguaggio C utilizzando il protocollo MPI.
Inoltre vi erano i seguenti vincoli:
- utilizzare matrici quadrate
- avere una comunicazione Point-to-Point.
- far in modo che il programma funzioni per un qualsiasi numero di processori
- testare il software sulle macchine Aws di tipo m4.large, in particolare utilizzando 1,2,3,4,5,6,7 e 8 istanze. 

## L'algoritmo
Dati i vincoli del problema l'algoritmo sviluppato fa si che il master suddivida le righe della Matrice A fra i vari processori e invii a tutti la matrice B.
I vari processori (master compreso) effettuano la computazione sulla loro porzione di matrici, dopodiché i risultati vengono inviati al master e assemblatati da quest'ultimo.

Si può suddividere l'algoritmo in 4 fasi:
### Fase di Inizializzazione
La prima parte dell'algoritmo si occupa di dichiarare e allocare le variabili che saranno utilizzate successivamente.
È da sottolineare che il master e gli slave inizializzano dinamicamente le matrici.
Il master si occupa di inizializzare le matrici con valori casuali (valori fra 0 e 9).
### Fase di invio
Il master calcola il numero di righe di matrice A e le invia ad ogni slave. Questo numero va a definire quante saranno le righe di matrice C calcolate da ogni processore. In questa parte è stato gestito il **problema del resto** permettendo cosi la risoluzione di matrici quadrate di qualsiasi dimensione.
 ```c
 for (int k = 1; k < p; k++) {
			countRowsSend[k] = 0;
			/*HANDLING OF REMAIN*/
			if (k <= remain) {
				countRowsSend[k] = portionSize + 1;
			} else {
				countRowsSend[k] = portionSize;
			}
			...
```
L'invio del numero di righe,della porzione di matrice A e della matrice B è effettuato tramite send non bloccante(standard).
Si è fatta questa scelta in quanto le send si trovato all'interno di un ciclo for, ed attendere il completamento di una send bloccante al processore k avrebbe ritardato l'invio delle send successive.
```c
MPI_Isend(&countRowsSend[k], 1, MPI_INT, dest, tag,
				MPI_COMM_WORLD, &requestesForCount[k]);
				
MPI_Isend(&matrixA[rowsStart[k]][0], width * countRowsSend[k],
				MPI_INT, dest, tag,MPI_COMM_WORLD, &requestesForA[k]);

MPI_Isend(&matrixB[0][0], width * height, MPI_INT, dest, tag,
				MPI_COMM_WORLD, &requestesForB[k]);
```
Ci si assicura del completamento delle send tramite delle `MPI_Waitall(...)`.
### Fase di computazione
Ogni processore calcola la propria porzione di matrice C tramite l'algoritmo
```c
	for (int a = 0; a < localHeight; a++) {
		for (int q = 0; q < width; q++) {
			matrixC[a][q] = 0;
			for (int u = 0; u < width; u++) {
				matrixC[a][q] += matrixA[a][u] * matrixB[u][q];
			}
		}
	}
```
### Fase di assemblaggio
Ogni processore(tranne il master) invia la porzione di matrice C calcolata al master. L'invio viene effettuato con delle send non bloccanti.
```c
MPI_Send(&matrixC[0][0], width * rowCount, MPI_INT, dest, tag,
		MPI_COMM_WORLD);
```
Il master si assicura  che le receive non bloccanti siano completate tramite una `MPI_Waitall`. L'assemblaggio è implicito in quanto nelle send e receive vengono passati puntatori che indicano indirizzi precisi di parti delle matrici (ad esempio: `&matrixA[rowsStart[k]][0]`) .
## Benchmark
Sono stati effettuati una serie di test utilizzando delle istanze di tipo **m4.large** (2 core) di Amazon Web Services con **StarCluster-Ubuntu_12.04-x86_64-hvm** - ami-52a0c53b. Il tempo di esecuzione è stato considerato dal momento successivo alla generazione e  caricamento delle matrici, i tempi sono in ms ed è stata utilizzata la funzione `MPI_Wtime()`.
### Strong Scaling
I test di tipo Strong Scaling sono stati effettuati stabilendo una dimensione della matrice pari a 2000x2000 celle. Nella tabella seguente sono riportati i tempi.

| N. Processori | Tempo in ms |
|:------:|:------:|
|  2| 43197 |
|  4| 24026|
|  6| 16873|
|  8| 12363|
|  10| 22902|
|  12| 22070|
|  14| 20302 |
|  16| 17875 |

Di seguito vi è riportato il grafico dei tempi
![Grafico Strong Scaling](https://raw.githubusercontent.com/oromis95/MatrixMultiplication/master/doc/Strong%20Scaling.PNG)

Dal grafico si nota un aumento dei tempi quando si utilizzano 10 processori, probabilmente è il punto in cui l'overhead di comunicazione raggiunge valori molto alti da andare ad inficiare sul tempo globale di esecuzione.

### Weak Scaling
Per effettuare i test di tipo Weak Scaling si è deciso di fissare la dimensione del lato della matrice uguale a 200^p dove p è il numero di processori attivi. Nella tabella successiva sono riportati i tempi.

|Dimensione Matrice| N. Processori | Tempo in ms |
|:------:|:------:|:------:|
|400x400|2|214|
|800x800|4|922|
|1200x1200|6|2203|
|1600x1600|8|7571|
|2000x2000|10|23258|
|2400x24000|12|40752|
|2800x2800|14|62326|
|3200x3200|16|86801|

Di seguito è riportato il grafico del dei tempi
![Grafico Weak Scaling](https://raw.githubusercontent.com/oromis95/MatrixMultiplication/master/doc/Weak%20Scaling.PNG)

Dai grafici si nota che i tempi vanno ad aumentare man mano che la size della matrice e numero di processori aumenta. L'aumento di tempo è chiaramente dovuto all'overhead di comunicazione, che però è tollerabile fino alla situazione in cui si hanno 8 processori e una matrice 1600*1600, dopodiché l'overhead inficia completamente le prestazioni. 
### Fattori di scalabilità
Il fatto di strong scalability è stato calcolato tramite la formula:
`t1 / ( N * tN ) * 100%`
Mentr il fattore di weak scalability è stato calcolato tramite la formula:
`(t1/tN) * 100%`

||2|4|6|8|10|12|14|16|
|--------|--------|--------|--------|--------|--------|--------|--------|--------|
|**Strong Scaling**|99,43%|89,38%|84,85%|86,86%|37,51%|32,43%|30,22%|30,03%|
|**Weak Scaling**|19,62%|4,56%|1,91%|0.56%|0,18%|0.10%|0,07%|0,05%|
## Ottimizzazioni e extra
Sono state implementate una serie di ottimizzazioni e di funzioni extra per migliorare le performance e la robustezza del software.
#### Matrici as Array 1D
Dato che il C per le matrici multidimensionali utilizza metodi row-major si è deciso di ottimizzare le performance mappando una matrice (array 2D) in un array mono-dimensionale.
```c
matrixInArrayA = malloc(height * width * sizeof(int));
matrixA = malloc(height * sizeof(int*));
for (int y = 0; y < rowCount; y++) {
		matrixA[y] = &matrixInArrayA[y * width];
}
```
#### Correttezza tramite algoritmo di Freivalds
Per controllare che la matrice prodotto C sia stata calcolata senza errori si è deciso di implementare un metodo di checking basato sull'algoritmo di Freivalds.
È un algoritmo probabilistico randomizzato con il vantaggio di avere complessità pari O(n^2.3729). Se si vuole effettuare il test della matrice prodotta si deve passare da linea di comando il parametro `-t testCycles` dove testCycles è un numero intero>0 che indica il numero di cicli che deve effettuare l'algoritmo di Freivalds. Se l'algoritmo da esito positivo sappiamo che il calcolo è corretto con una probabilità di (1-(1/(2^testCycles)).

#### Salvataggio delle matrici in csv
Sia le matrici generate casualmente (A e B) sia la matrice prodotto C vengono salvate in file csv. I file si trovano all'interno della cartella `data`. 

### Programmazione Concorrente, Parallela e su Cloud
Università degli Studi di Salerno 

Anno accademico  2017/2018

**Prof. Vittorio Scarano**

**Dott. Carmine Spagnuolo**

**Studente: Domenico Antonio Tropeano 0522500513**

