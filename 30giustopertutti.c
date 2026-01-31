
//idea di miglioramento: il peso lo calcolo alla prima volta che controllo l'ordine e poi gli ordini in sospeso avranno una nuova istanza che tiene in memoria il peso
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLEN 20  
#define TABLESIZE 65536
#define SIZEHASHMAG 256
#define LENHEAP 16
#define FNV_OFFSET_BASIS 2166136261U
#define FNV_PRIME 16777619U
#define LENVECT 10
#define POINTERSIZE 65536
typedef struct lott{//nodo contenuto nell'heap
    int scadenza;
    int quant;
}lotto;
//idea: nella ricetta al posto di salvarmi il nome dell'ingrediente mi posso salvare direttamente il puntatore al magazzino e la sua quantità così posso rimuovere il puntatore al magazzino dagli ordini in

typedef struct magazz{//scatola che contiene tutte le occorrenze di un ingr
    char ing[MAXLEN];
    int totale;
    int heapsize;
    int lenarr;
    lotto * dati;// punta all'ing che ha scadenza minima 
    struct magazz * next;
}magaz;
typedef struct ingri{
    magaz * pointer;
    unsigned short quant;
    struct ingri * nexting;
}ingric; //struttura dati che contiene un ingrediente della ricetta con la sua quantità

typedef struct rice{
    char nome[MAXLEN];  
    ingric * ingredienti;
    struct rice * nextric;
}ricetta; //struttura dati che contiene il nome di una ricetta e un puntatore a una lista che elenca tutti gli ingredienti


typedef struct pront{
    char nome[MAXLEN];
    unsigned short quant;
    int tempo;
    int peso;
    struct pront * next; 
}pronti;

typedef struct poi{
    char nome[MAXLEN];
    struct poi * next;
    magaz * posizioni[LENVECT];
    unsigned short occorrenze;
}pointeringred;

typedef struct ordiso{
    //char nome[MAXLEN];
    unsigned short quant;
    int tempo;
    struct ordiso * next; 
    unsigned short k;
    //pointeringred * posizioni;
    ricetta * pointrecipe;//puntatore alla ricetta
}ordsosp; //struttura che mi serve per immagazzinare gli ordini in sospeso


pointeringred * testapointer=NULL;
pointeringred * hashpointer[POINTERSIZE];

magaz * hashmag[SIZEHASHMAG];//hash table che contiene tutte le scorte
ricetta * hash_table[TABLESIZE]; //variabile globale hash table che contiene le ricette disponibili
int time=0;
int periodicita, capienza; //configurazione del corriere
ordsosp * head=NULL; //testa della lista degli ordini in sospeso
ordsosp * queue=NULL; //coda della lista degli ordini in sospeso
pronti * hep=NULL;//testa della lista degli ordini pronti
pronti * camioncino=NULL; //testa della lista degli ordini che inserisco sul camioncino
void inizializza_hash_table(int dim);
void inizializza_hash_table2(int dim);
void inizializza_hash_table3(int dim);
unsigned short hash(char * nome, int dim);
ricetta * findrecipe(char * nuovaricetta, int k);
void aggiungi_ingrediente(ricetta * ricetta,char * nome, unsigned short quantita);
void insertinorder(char * nomelotto);
void rifornimento();
unsigned int hash2(char *str, int table_size);
void insertheap(magaz * ingrediente, int quant, int scadenza);
void swap(lotto *, lotto*);
int ordiniinsospeso(char * nomeric);
void deleterecipe(char * dacanc);
void sistemaheap(magaz * curr);
void minheapify(magaz * curr, int n);
void decrementaheap(magaz * curr, int decr);
void rimuovi_ricetta();
void checkordini();
void ordine();
pointeringred * insosppointer(char * nomericetta);
void print_hash();
void caricaordini();
void aggiungi_ricetta();
void aggiungiordiniinattesa(char *nomericetta, unsigned short quant, unsigned short k);
void inseriscipronti(char * nome, unsigned short quant, int tempo, int peso);

int main(){
    char comando[MAXLEN];
    if(scanf("%d %d", &periodicita, &capienza)!=2){
        printf("errore lettura scanf\n");
        return 0;
    }
    inizializza_hash_table(TABLESIZE);
    inizializza_hash_table2(SIZEHASHMAG);
    inizializza_hash_table3(POINTERSIZE);
    while(scanf("%s", comando)==1){
        if(strcmp(comando,"aggiungi_ricetta")==0){
            aggiungi_ricetta();
        }
        else if(strcmp(comando,"rimuovi_ricetta")==0)
            rimuovi_ricetta();
        else if(strcmp(comando, "rifornimento")==0)
            rifornimento();
        else if (strcmp(comando, "ordine")==0)
            ordine();
        else{
            printf("errore di lettura input non è un comando \n");
            printf("questa stringa ha creato errore\n%s", comando);
            break;
        }
    }
    /*for(int i=0; i < POINTERSIZE ; i++){
        printf("\nindice tabella :  %d  ", i);
        pointeringred * curr=hashpointer[i];
        while(curr!=NULL){
            printf("%s ----> ", curr->nome);
            curr=curr->next;
        }
    }*/
    return 0;   
}
void inizializza_hash_table(int dim){ //inizializzo hashtable ricette
    for(int i=0;i<dim;i++){
        hash_table[i]=NULL;
    }
    return;
}
void inizializza_hash_table2(int dim){
    for(int i=0;i<dim;i++)
        hashmag[i]=NULL;
    return;
}
void inizializza_hash_table3(int dim){
    for(int i=0;i<dim;i++){
        hashpointer[i]=NULL;
    }
    return;
}
/*int hash(char * nome, int dim){   //funzione di hash per le ricette
    int len=strlen(nome);
    int hash_value=0;
    for(int i=0;i<len;i++){ 
        hash_value+=*(nome+i); //sommo tutti i valori in ascii della stringa
    }
    int k=(int) (dim*(ALFA*hash_value-(int)(ALFA*hash_value)));
    return k;
}*/
/*int hash( char *str,  int table_size) {
    unsigned long hash=5381;
    int c;

    while ((c=*str++)) {
        hash=((hash << 5)+hash)+c; // hash * 33 + c
    }
    hash=hash%table_size;
    return (int) hash;
}*/

unsigned short hash(char *str, int table_size) {
    unsigned int hash = FNV_OFFSET_BASIS;
    while (*str) {
        hash ^= (int)*str++;    
        hash *= FNV_PRIME;
    }
    return hash % table_size; // Restituisce l'indice nella tabella
}

unsigned int hash2(char *str, int table_size) {
    unsigned int hash = FNV_OFFSET_BASIS;
    while (*str) {
        hash ^= (int)*str++;    
        hash *= FNV_PRIME;
    }
    return hash % table_size; // Restituisce l'indice nella tabella
}

void caricaordini(){
    pronti * curr=hep;//uso per scorrere ordini pronti
    //pronti * pre=NULL;//uso per scorrere ordini pronti
    pronti * camcurr;//uso per scorrere ordini sul camion
    pronti * campre=NULL;//uso per scorrere ordini sul camion
    pronti * inscam;//nuovo nodo che metto sul camion
    pronti *camioncino=NULL;
    int conta=0;
    if(curr==NULL){
        printf("camioncino vuoto\n");
        return;
    }
    while(curr!=NULL && capienza>conta ){
        camcurr=camioncino;
        campre=NULL;
        if(curr->peso+conta<=capienza){
            conta+=curr->peso;
            inscam=curr;
            if(hep==curr){//cambio la testa
                hep=curr->next;
                curr=hep;
            }
            /*else{ // posso togliere questa parte in quanto il processo si ferma appena viene trovato un ordine che va oltre la capienza massima
                pre->next=curr->next;
                curr=pre->next;
            }*/
            while(camcurr!=NULL && camcurr->peso>=inscam->peso){
                campre=camcurr;
                camcurr=camcurr->next;
            }
            if(campre==NULL){
                inscam->next=camioncino;
                camioncino=inscam;
            }
            else{
                inscam->next=campre->next;
                campre->next=inscam;
            }
        }
        else{
            break;
        }
    }
    //ora mi resta da stampare e deallocare camioncino
    camcurr=camioncino;
    while(camcurr!=NULL){
        printf("%d %s %d\n", camcurr->tempo, camcurr->nome, camcurr->quant);
        campre=camcurr->next;
        free(camcurr);  
        camcurr=campre;
    }
    return;
}



ricetta * findrecipe(char *nuovaricetta, int k ){ 
    ricetta *curr=hash_table[k];
    while (curr!=NULL) {
        if (strcmp(curr->nome, nuovaricetta)==0) {
            return curr;;
        }
        curr=curr->nextric;
    }
    return NULL;
}

void aggiungi_ingrediente(ricetta * ricetta,char * nome, unsigned short quantita){
    ingric * nuovo= (ingric*)malloc(sizeof(ingric ));
    magaz *curr;
    magaz * pre=NULL;
    nuovo->quant=quantita;
    nuovo->nexting=ricetta->ingredienti;
    ricetta->ingredienti=nuovo;
    int j=hash(nome, SIZEHASHMAG);
    curr=hashmag[j];
    while(curr!=NULL && strcmp(nome,curr->ing)>0){
        pre=curr;
        curr=curr->next;
    }
    if(curr!=NULL && strcmp(nome, curr->ing)==0){
        nuovo->pointer=curr;
        
    }
    else{
        magaz * new=(magaz*)malloc(sizeof(magaz));
        strcpy(new->ing,nome);
        new->dati=(lotto *)malloc(LENHEAP*sizeof(lotto));
        new->lenarr=LENHEAP;
        new->heapsize=0;
        new->next=curr;
        new->totale=0;
        if(pre==NULL){
            hashmag[j]=new;
        }
        else{
            pre->next=new;
        }
        nuovo->pointer=new;
    }

    return ;
}


void minheapify(magaz * curr, int n){
    lotto * start= curr->dati;
    int left= 2*n+1;
    int right=2*n+2;
    int posmin=n;
    if( left<curr->heapsize && start[left].scadenza<start[n].scadenza){
        posmin=left;
    }
    if(right<curr->heapsize&& start[right].scadenza<start[posmin].scadenza){
        posmin=right;
    }
    if(posmin!=n){
        lotto tmp=start[n];
        start[n]=start[posmin];
        start[posmin]=tmp;

        //swap(&start[n], &start[posmin]);
        minheapify(curr,posmin);
    }
    return;
}

void sistemaheap(magaz * curr){//aggiusta heap se è scaduto
    while(curr->heapsize>0 && curr->dati[0].scadenza<=time){
        curr->totale-=curr->dati[0].quant;
        curr->dati[0]=curr->dati[curr->heapsize-1];
        curr->heapsize--;
        minheapify(curr,0);
    }     
    return;     
}


void decrementaheap(magaz * curr, int decr){
    curr->totale=curr->totale-decr;
    while(decr>0 && curr->heapsize>0){
        if(curr->dati[0].quant<=decr){
            decr-=curr->dati[0].quant;
            curr->heapsize--;
            curr->dati[0]=curr->dati[curr->heapsize];
            minheapify(curr,0);
        }
        else{
            curr->dati[0].quant-=decr;
            decr=0;
        }
    }
    return;
}

void checkordini(){//funzione chiamata da rifornimento che controlla se posso preparare qualche ordine che era rimasto in attesa
    ordsosp * ordinesospeso=head;
    ordsosp * precedente=NULL;
    magaz * curr;
    //magaz * pre=NULL;
    unsigned short quant;
    int peso=0, ris;
    ricetta * daprep=NULL;
    ingric * ing;
    int number;
    ingric * ut;
    while(ordinesospeso!=NULL){
        number=0;
        quant=ordinesospeso->quant;
        daprep= ordinesospeso->pointrecipe;
        ing = daprep->ingredienti;
        ris=1;
        ut=daprep->ingredienti;

        //pointeringred * c=ordinesospeso->posizioni;
        //ut=c->posizioni;
        while(ing!=NULL){//ciclo che verifica che ci siano tutti gli ingredienti per evadere il suddetto ordine, non decrementa nulla verifica solo il magazzino
            curr=ut->pointer;
            if(curr->totale<ing->quant*quant){
                ris=0;
                break;
            }   
            sistemaheap(curr);
            if(curr->totale<ing->quant*quant){
                ris=0;
                break;
            }  
            number++;
            ut=ut->nexting;
            ing=ing->nexting;
        }
        if(!ris){//se non ho abbastanza  di un ingrediente provo con l'ordine successivo 
            precedente=ordinesospeso;
            ordinesospeso=ordinesospeso->next;
        }
        else{
            ing=daprep->ingredienti;
            peso=0;
            ut=daprep->ingredienti;
            for(int i=0;i<number;i++){
                curr=ut->pointer;
                peso+=quant*ing->quant;
                decrementaheap(curr,ing->quant*quant);
                ing=ing->nexting;  
                ut=ut->nexting;              
            }
            inseriscipronti(daprep->nome, quant, ordinesospeso->tempo, peso);
             
            /*pointeringred * ferma=ordinesospeso->posizioni;
            ferma->occorrenze--;
            if(ferma->occorrenze==0){
                unsigned int k3=hash2(ferma->nome, POINTERSIZE);
                pointeringred* current=hashpointer[k3];
                while(current!=NULL){
                    if(strcmp(current->nome, ferma->nome)==0){
                        free(current);
                    }
                    current=current->next;
                }
            }  */
            if(precedente==NULL){//vuol dire che la testa cambia
                if(ordinesospeso->next==NULL){
                    queue=NULL;
                }
                head=ordinesospeso->next;
                free(ordinesospeso);
                ordinesospeso=head;
            }
            else{
                if(ordinesospeso->next==NULL){
                    queue=precedente;
                }
                precedente->next=ordinesospeso->next;
                free(ordinesospeso);
                ordinesospeso=precedente->next;
            }
        }
    }
    return;
}


void swap(lotto * x, lotto * y){
    lotto tmp=*(x);
    *(x)=*(y);
    *(y)=tmp;
    return;
}

void insertheap(magaz * ingrediente, int quant, int scadenza){
    if(ingrediente->heapsize==ingrediente->lenarr){//devo allocare un blocco + grande
        ingrediente->lenarr=ingrediente->lenarr*2;
        ingrediente->dati=(lotto *)realloc(ingrediente->dati,ingrediente->lenarr*sizeof(lotto));
    }
    ingrediente->totale+=quant;
    ingrediente->heapsize++;
    int index=ingrediente->heapsize-1;
    ingrediente->dati[index].scadenza=scadenza;
    ingrediente->dati[index].quant=quant;
    int parentindex;
    while(index>0){
        parentindex=(index-1)/2;
        if(ingrediente->dati[parentindex].scadenza<=ingrediente->dati[index].scadenza){
            break;
        }
        lotto tmp=ingrediente->dati[parentindex];
        ingrediente->dati[parentindex]=ingrediente->dati[index];
        ingrediente->dati[index]=tmp;


        //swap(&ingrediente->dati[parentindex],&ingrediente->dati[index]);
        index=parentindex;
    }
    return;
}

void insertinorder(char * nomelotto){ //funzione chiamata da rifornimento che aggiunge un lotto in magazzino
    magaz * curr;
    magaz * pre=NULL;
    int quant, scadenza;
    if(scanf("%d %d", &quant, &scadenza)!=2){
        printf("errore lettura scanf\n");
        return;
    }
    int k=hash(nomelotto, SIZEHASHMAG);
    if(hashmag[k]==NULL){
        magaz * nuovo=(magaz *) malloc(sizeof(magaz));
        strcpy(nuovo->ing, nomelotto);
        nuovo->dati=(lotto *)malloc(LENHEAP*sizeof(lotto));
        nuovo->lenarr=LENHEAP;
        nuovo->heapsize=0;
        nuovo->totale=0;
        nuovo->next=NULL;
        insertheap(nuovo,quant,scadenza);
        hashmag[k]=nuovo;
    }
    else{
        curr=hashmag[k];
        while(curr!=NULL && strcmp(nomelotto,curr->ing)>0){
            pre=curr;
            curr=curr->next;
        }
        if(curr!=NULL && strcmp(nomelotto,curr->ing)==0){
            insertheap(curr,quant,scadenza);
        }
        else{
            magaz * nuovo=(magaz *)malloc(sizeof(magaz));
            strcpy(nuovo->ing,nomelotto);
            nuovo->dati=(lotto *)malloc(LENHEAP*sizeof(lotto));
            nuovo->lenarr=LENHEAP;
            nuovo->heapsize=0;
            nuovo->next=curr;
            nuovo->totale=0;
            insertheap(nuovo,quant,scadenza);
            if(pre==NULL){
                hashmag[k]=nuovo;
            }
            else{
                pre->next=nuovo;
            }
        }
    }
    return;
}

void rifornimento(){
        if(time%periodicita==0 && time!=0){
            caricaordini();
        }
        char lotto[MAXLEN];
        while(scanf("%s", lotto)==1 && strcmp(lotto, "ordine") != 0 && strcmp(lotto, "rimuovi_ricetta") != 0 && strcmp(lotto, "rifornimento") != 0 && strcmp(lotto,"aggiungi_ricetta")!=0 ){
            insertinorder(lotto);
        }
        printf("rifornito\n");
        checkordini();
        time++;
        if(strcmp(lotto,"ordine")==0)
            ordine();
        else if(strcmp(lotto,"rifornimento")==0)
            rifornimento();
        else if(strcmp(lotto, "aggiungi_ricetta")==0)
            aggiungi_ricetta();
        else if(strcmp(lotto,"rimuovi_ricetta")==0)
            rimuovi_ricetta();
    return; 
}
int ordiniinsospeso(char * nomeric){//funzione chiamata nella rimuovi_ricetta, restituisce 1 se ci sono ordini in sospeso 0 altrimenti
    ordsosp * curr;
    pronti * corr=hep;
    curr=head;
    while(curr!=NULL){
        if(strcmp(curr->pointrecipe->nome,nomeric)==0){
            return 1;
        }
        curr=curr->next;
    }
    while(corr!=NULL){
        if(strcmp(corr->nome,nomeric)==0){
            return 1;
        }
        corr=corr->next;
    }
    return 0;
}

void deleterecipe(char * dacanc){//funzione che effettivamente cancella la ricetta in input
    int k=hash(dacanc,TABLESIZE);
    ricetta * curr=hash_table[k];
    ricetta * pre= NULL;
    while(curr!=NULL){
        if(strcmp(curr->nome,dacanc)==0){
            if(pre==NULL){
                hash_table[k]=curr->nextric;
            }
            else{
                pre->nextric=curr->nextric;
            }
            ingric *ing=curr->ingredienti;
            while(ing!=NULL){
                ingric * tmp=ing;
                ing=ing->nexting;
                free(tmp);
            }
            free(curr);
            return;
        }
        pre=curr;
        curr=curr->nextric;
        
    }
}

void rimuovi_ricetta(){
    if(time%periodicita==0 && time!=0){
        caricaordini();
    }
    char darimuovere[MAXLEN];
    if(scanf("%s", darimuovere)!=1){
        printf("errore lettura stringa\n");
        return;
    }
    int k=hash(darimuovere, TABLESIZE);
    if(findrecipe(darimuovere,k)){
        if(ordiniinsospeso(darimuovere)){ //funzione da implementare
            printf("ordini in sospeso\n");
        }
        else {
            deleterecipe(darimuovere);
            printf("rimossa\n");
        }

    }
    else{
        printf("non presente\n");
    }
    time++;
    return;
}

pointeringred * insosppointer(char * nomericetta){

    unsigned int k=hash2(nomericetta,POINTERSIZE);
    pointeringred * curr=hashpointer[k];
    while(curr!=NULL){
        if(strcmp(curr->nome,nomericetta)==0){
            return curr;
        }
        curr=curr->next;
    }
    return NULL;
}



void aggiungiordiniinattesa(char *nomericetta, unsigned short quant, unsigned short k){//aggiunge l'ordine alla lista degli ordini in attesa.
    ordsosp *nuovo=(ordsosp *)malloc(sizeof(ordsosp));
    nuovo->tempo=time;
    nuovo->next=NULL;
    nuovo->k=k;
    //strcpy(nuovo->nome,nomericetta);
    nuovo->quant=quant;
    ricetta * daprep=findrecipe(nomericetta,k);
    nuovo->pointrecipe=daprep;
/*    pointeringred * trovato= insosppointer(nomericetta);
    if(trovato!=NULL){
        nuovo->posizioni=trovato;
         
    }
    else{
    //pointeringred * puntatori=(pointeringred*)malloc(sizeof(pointeringred));
    //strcpy(puntatori->nome,nomericetta);
    ingric * ing=daprep->ingredienti;
    magaz * curr;
    magaz * pre=NULL;
    int i=0;
    while(ing!=NULL){
        pre=NULL;
        int j=hash(ing->ingnome, SIZEHASHMAG);
        curr=hashmag[j];
        while(curr!=NULL && strcmp(ing->ingnome, curr->ing)>0){
            pre=curr;
            curr=curr->next;
        }
        if(curr!=NULL && strcmp(ing->ingnome,curr->ing)==0){
            puntatori->posizioni[i]=curr;
            puntatori->occorrenze++;
        }
        else{
           magaz * new =(magaz *)malloc(sizeof(magaz));
           strcpy(new->ing,ing->ingnome);
           new->dati=(lotto *)malloc(LENHEAP *sizeof(lotto));
           new->lenarr=LENHEAP;
           new->heapsize=0;
           new->next=curr;
           new->totale=0;
           if(pre==NULL){
            hashmag[j]=new;
           } 
           else{
            pre->next=new;
           }
           puntatori->posizioni[i]=new;
        }
        ing=ing->nexting;
        i++;
        
    }
    unsigned int l=hash2(nomericetta,POINTERSIZE);
    puntatori->next=hashpointer[l];
    puntatori->occorrenze=0;
    hashpointer[l]=puntatori;
    nuovo->posizioni=puntatori;

    }*/
    if(head==NULL){
        head=nuovo;
        queue=nuovo;
        return;
    }
    queue->next=nuovo;
    queue=nuovo;
    return;
}
void inseriscipronti(char * nome, unsigned short quant, int tempo, int peso){//funzione che inserisce in lista gli ordini pronti, chiamata in ordine
    pronti * curr;
    pronti * pre=NULL;
    pronti * nuovo=(pronti*)malloc(sizeof(pronti));
    strcpy(nuovo->nome,nome);
    nuovo->quant=quant;
    nuovo->tempo=tempo;
    nuovo->peso=peso;
    nuovo->next=NULL;
    curr=hep;
    if(hep==NULL){
        hep=nuovo;
        return;
    }
    while(curr!=NULL && nuovo->tempo>curr->tempo){
        pre=curr;
        curr=curr->next;
    }
    if(pre==NULL){
        nuovo->next=hep;
        hep=nuovo;
    }
    else{
        nuovo->next=pre->next;
        pre->next=nuovo;
    }
    return;
}


void ordine(){
    if(time%periodicita==0 && time!=0){
        caricaordini();
    } 
    char nomeric[MAXLEN];
    unsigned short quant;
    ricetta * daprep;
    ingric * ing;//ing è l'ingrediente che mi occorre
    magaz * curr;//curr scorre gli ingredienti nel magazzino
    //magaz * pre=NULL;
    int peso;
    if(scanf("%s %hu", nomeric, &quant)!=2){
        printf("errore lettura nella funz ordine\n");
        return;
    }
    unsigned short k2=hash(nomeric, TABLESIZE);
    daprep= findrecipe(nomeric,k2);
    if(daprep==NULL){
        printf("rifiutato\n");
        time++;
        return;
    }
    printf("accettato\n");
    ing = daprep->ingredienti;
    while(ing!=NULL){//ciclo che verifica che ci siano tutti gli ingredienti per evadere il suddetto ordine, non decrementa nulla verifica solo il magazzino
        
        curr=ing->pointer;
        //pre=NULL;
        
            if(curr->totale<ing->quant*quant){
                aggiungiordiniinattesa(nomeric,quant, k2);
                time++;
                return;
            }
            sistemaheap(curr);
            if(curr->totale<ing->quant*quant){
                aggiungiordiniinattesa(nomeric, quant,k2);
                time++;
                return;
            }
       
        ing=ing->nexting;
    }
    ing=daprep->ingredienti;
    peso=0;
    while(ing!=NULL){
        curr=ing->pointer;;
        //pre=NULL;
        peso+=quant*ing->quant;
        decrementaheap(curr,ing->quant*quant);
        ing=ing->nexting;
    }
    inseriscipronti(daprep->nome,quant,time, peso);
    time++;
    return;
}

void print_hash(){
    for(int i=0;i< TABLESIZE;i++){
        printf("%d-----%s\n",  i, hash_table[i]->nome);
    }
    return;
}

void aggiungi_ricetta(){
    if(time%periodicita==0 && time!=0){
        caricaordini();
    }
    char nomericetta[MAXLEN];
    char nomein[MAXLEN];
    unsigned short quantin;

    if(scanf("%s", nomericetta)!=1){
        printf("errore lettura ricetta in aggiungi rice\n");
        return;
    }
    int k=hash(nomericetta,TABLESIZE);
    if(findrecipe(nomericetta,k)){
        printf("ignorato\n");
        if(scanf("%s", nomein)!=1){
            printf("errore lettura ingred in agg rice\n");
            return;
        }
        while(strcmp(nomein, "ordine") != 0 && strcmp(nomein, "rimuovi_ricetta") != 0 && strcmp(nomein, "rifornimento") != 0 && strcmp(nomein,"aggiungi_ricetta")!=0){ //finchè l'ingrediente è diverso dai comandi
            if(scanf(" %hu  %s", &quantin, nomein)!=2){
                printf("errore lettura quantita ed err ing in agg ricetta\n");
                return;
            }
        }
        time++;
        if(strcmp(nomein,"ordine")==0)
            ordine();
        else if(strcmp(nomein,"rimuovi_ricetta")==0)
            rimuovi_ricetta();
        else if(strcmp(nomein,"aggiungi_ricetta")==0)
            aggiungi_ricetta();
        else    
            rifornimento();
        return;
    }
    ricetta * nuova=(ricetta *) malloc(sizeof(ricetta ));
    strcpy(nuova->nome,nomericetta);
    nuova->ingredienti=NULL;
    while(scanf("%s", nomein)==1 && strcmp(nomein, "ordine") !=0 && strcmp(nomein, "rimuovi_ricetta")!=0 && strcmp(nomein, "rifornimento") != 0 && strcmp(nomein,"aggiungi_ricetta")!=0){ //finchè l'ingrediente è diverso dai comandi
        
        if(scanf("%hu", &quantin)!=1){
            printf("errore lettura quant");
            return ;
        }
        aggiungi_ingrediente(nuova,nomein,quantin);
    }
    nuova->nextric=hash_table[k];
    hash_table[k]=nuova;
    printf("aggiunta\n");
    time++;
    //print_hash();
    if(strcmp(nomein,"ordine")==0)
        ordine();
    else if(strcmp(nomein,"rimuovi_ricetta")==0)
        rimuovi_ricetta();
    else if(strcmp(nomein,"aggiungi_ricetta")==0)
        aggiungi_ricetta();
    else   
        rifornimento();
    return;     
}