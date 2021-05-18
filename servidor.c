#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>

struct clientes{
    int online;
    char user_name[24];
    struct sockaddr_in user;
};

typedef struct clientes elemento;

#include "lista.h"


struct mensagem{
  int codigo;
  int resposta;
  long ip;
  int porta;
  char buf[2048];
};

void cadastro(){
  
}
/*Servidor UDP*/
int main(){
int sock,tam;
int cont=0;
struct sockaddr_in name, serv;
struct hostent *hp, *gethostbyname();

struct mensagem msg;
struct clientes cli;
celula *lista = NULL;
int indice=1;


FILE *usuarios;


/*Cria o socket*/
/*socket(familia do socket, --- ,num do protocolo)*/
sock = socket(AF_INET, SOCK_DGRAM, 0);
if(sock<0){ /*se nao conseguir abrir o socket*/
  perror ("abertura de socket");
  exit(1);
}

/*Associa*/
name.sin_family = AF_INET; //sin socket internet
name.sin_addr.s_addr = INADDR_ANY; //o endereÃƒÂ§o de socket
name.sin_port = htons(1234); //deixa o SO atribuir a porta

/*parametros do bind o descritor, struct de endereÃƒÂ§o de socket*/
/*se der erro no bind, se a porta ja ta em uso etc*/
if(bind (sock, (struct sockaddr *)&name, sizeof name) < 0){
  perror ("binding datagrama");
  exit(1);
}

/*recupera e imprime o numero da porta*/
tam = sizeof(name);
if(getsockname(sock, (struct sockaddr *)&name, &tam) < 0){
  perror("erro no getsockname");
  exit(1);
}

printf("Numero da porta atribuida: %d\n",ntohs(name.sin_port));


serv.sin_family = AF_INET;

while(1){

recvfrom(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, &tam);


if(msg.codigo==0){
    printf("Realizando Cadastramento\n");
    printf("DDDD  Familia: %d\n", name.sin_family);
    printf("DDDD  Endereco: %s\n", inet_ntoa(name.sin_addr));
    printf("DDDD  Porta: %d\n\n", name.sin_port);
    
    cli.user.sin_port= name.sin_port;
    cli.user.sin_addr.s_addr=name.sin_addr.s_addr;
    cli.online=1;
    sprintf(cli.user_name, "%s", msg.buf);
    msg.resposta=indice;

    insere(cli, &lista);
    copiar(msg.buf, lista);
    sendto(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name);
    usuarios = fopen("usuarios.txt", "a");
    fprintf(usuarios, "%s\n", msg.buf);
    fclose(usuarios);
    indice++;
}
if(msg.codigo==1){
    printf("Realizando login!\n");
    printf("DDDD  Familia: %d\n", name.sin_family);
    printf("DDDD  Endereco: %s\n", inet_ntoa(name.sin_addr));
    printf("DDDD  Porta: %d\n\n", name.sin_port);
    
    cli.user.sin_port= name.sin_port;
    cli.user.sin_addr.s_addr=name.sin_addr.s_addr;
    cli.online=1;
    sprintf(cli.user_name, "%s", msg.buf);
    msg.resposta=indice;

    insere(cli, &lista);
    copiar(msg.buf, lista);
    sendto(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name);
    indice++;
    }
if (msg.codigo==2){
    printf("Usuários Logados:\n");
    printf("DDDD  Familia: %d\n", name.sin_family);
    printf("DDDD  Endereco: %s\n", inet_ntoa(name.sin_addr));
    printf("DDDD  Porta: %d\n\n", name.sin_port);
    msg.resposta=indice;
    msg.codigo=2;
    copiar(msg.buf, lista);
    sendto(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name);
}
if(msg.codigo==3){
  int x;
  printf("buscando usuario\n");
  
  celula *user;
  user=NULL;
  user = malloc(sizeof(celula *));
  sprintf(user->conteudo.user_name,"%s", msg.buf);
  //printf("%s", user->conteudo.user_name);
  //scanf("%d", &x);
  user=busca(user->conteudo, lista);
  
  msg.codigo=3;
  if(user==NULL){

    msg.resposta==-1;
    sendto(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name);
  }
  else{
    bcopy(&(user->conteudo), msg.buf, sizeof(elemento));
//    sprintf(msg.buf, "%s", user->conteudo);
    msg.resposta=1;
    elemento x;
    bcopy(msg.buf, &x, sizeof(elemento));
    sendto(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name);
  }
  

  
}
if (msg.codigo==5){
    
    
    cli.user.sin_port= name.sin_port;
    cli.user.sin_addr.s_addr=name.sin_addr.s_addr;
    cli.online=1;
    sprintf(cli.user_name, "%s", msg.buf);
    printf("Vaaai\n");
    busca_e_remove(cli, &lista);
    indice--;

}
    msg.codigo=-1;
}

close(sock);

exit(0);
}
