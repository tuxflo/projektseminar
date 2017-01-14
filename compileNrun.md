#Kompilieren

Mit dem Befehl
     
    mpicc -o ga_test ga_test.c ga_create.c ga_mutex1.c mutex-create.c ga_put.c mutex-acquire.c mutex-release.c ga_get.c mcs-lock.c
     
Kann das erste Beispiel aus dem Buch erstellt werden. Damit wird das ausführbare Binary `ga_test` erstellt was

* ein globales Array für 4 Ranks mit Einträgen der Größe 10 anlegt
* per `ga_put` den Text "This is just a Test" in den Speicher des Ranks Nummer 1 schreibt
* per `ga_get` den eben geschriebenen Speicher wieder ausliest und mit `printf` ausgibt

Die im Beispiel verwendeten `MPI_Send` bzw. `MPI_Recv` Funktionen dienen lediglich der Synchronisierung der Ranks (Sicherstellen, dass erst geschrieben und dann gelesen wird)
Beim Kompilieren kommt es noch zu zahlreichen Warnungen a la "Implicit declaration of" was daran liegt, dass für die verwendeten Funktionen keine Funktionssignatur in einem Headerfile existiert.

#Ausführen
Per `mpirun -n 4 ./ga_test` kann das Beispiel gestartet werden. Neben den Hostnames und Ranks sollte der Text `This is just a test` ausgegeben werden.
