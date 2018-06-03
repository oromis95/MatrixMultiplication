MatrixMultiplication

	___________m__________
	|
	|
n	|
	|
	|
	|
	
	Considera due matrici quadrate A e B di size n che devono essee moltiplicate:
	1. Partiziona queste matrici in blocchi quadrati di dimensione p, dove p è il numero di processori avviabili.
	2. Crea una matrice di processi di size p^1/2 x p^1/2 cosi che ogni processo possa contenere un blocco di A e un blocco di B
	3. Ogni blocco è inviato ad ogni processo , e i sottoblocchi copiati sono moltiplicati assieme e il risultato viene aggiunto al risultato parziale nel sottoblocco in C
	4. I sottoblocchi di A sono shiftati a sinistra di un passo e i sottoblocchi di B sono shiftati verso l'alto di un passo.
	5. Ripetere step 3 e 4 