#include<stdio.h>
#include<stdlib.h>
#include <netinet/in.h>
#include <string.h>



struct reg {
      elemento conteudo; 
      struct reg *prox;
   };

typedef struct reg celula;  // célula

void insere (elemento x, celula * *pri);
celula * busca (elemento x, celula *le);
void busca_e_remove (elemento y, celula * *le);
void copiar(char *msg_buf, celula *le);

void copiar(char * msg_buf, celula *le){
    celula *p;
    p=le;
    int i=0;
    while(p != NULL){
      if(p->conteudo.online==1){
        bcopy( &(p->conteudo), msg_buf+i*sizeof(elemento),sizeof(elemento) );
         i++;
      }
        p=p->prox;

    }
}

celula * busca (elemento x, celula *le){
   celula *p;
   p = le;
   while (p != NULL && strcmp(x.user_name, p->conteudo.user_name)!=0) 
      p = p->prox; 
   return p;
}

void insere (elemento x, celula* *pri)
{
    celula * p, *novo;
    novo=(celula *) malloc (sizeof (celula));
    novo->conteudo =x;
    novo->prox =NULL;
    p =*pri;
    while(p!=NULL && p->prox != NULL)
    p=p->prox;
    if(p==NULL) *pri =novo;
    else p->prox=novo;
}

void busca_e_remove (elemento y, celula * *le)
{
   celula *p, *q;
   p = *le;
   q = *le;
   while ((strcmp(y.user_name, q->conteudo.user_name))!=0) {
      p = q;
      q = q->prox;
   }
   if (q == p) {
      *le=p->prox;
      free(p);
      
   }
   else{
      p->prox = q->prox;
      free (q);
      
   }
}