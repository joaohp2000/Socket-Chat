#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h> /* para poder manipular threads */
#include <stdlib.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <stdio_ext.h>


pthread_t producers;

struct clientes{
    int online;
    char user_name[24];
    struct sockaddr_in user;
};


struct mensagem{
  int codigo;
  int resposta;
  long ip;
  int porta;
  char buf[1024];
};

void menu_primario(int sock, struct sockaddr_in name, char *user_name);
void menu_secundario(int sock, struct sockaddr_in name, char *user_name);
void usuarios_online(struct mensagem lista_nomes, char *meu_nome);
struct clientes cli;


void *envia(void *sock_)
{
    char user_name[24];
    int tam;
    struct sockaddr_in name;
    int *sock = (int *)( sock_); // recebe socket
    sprintf(user_name, "%s", cli.user_name);
    name.sin_family = AF_INET;
    name.sin_port=htons(1234);
    struct hostent *hp, *gethostbyname();
    /* Acessa o DNS */
    hp = gethostbyname("localhost");
    if (hp==0){
      perror("Host nao encontrado");
      exit(2);
    }

    bcopy ((char*)hp->h_addr, (char *)&name.sin_addr, hp->h_length);
    struct mensagem msg;
    tam = sizeof(struct sockaddr_in);
   
 

    while(1){
    recvfrom((int) *sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *) &name, &tam);
    if(msg.codigo==2){
        usuarios_online(msg, user_name);
    }
    if(msg.codigo==3){
        //cli = (struct clientes *) malloc(sizeof(struct clientes));
        cli.user.sin_family=AF_INET;
        bcopy(msg.buf, &cli, sizeof(cli));

    }
    if(msg.codigo==10){
        printf("%s", msg.buf);
    }
    
    }

    pthread_exit(NULL);
}


int main (int argc, char *argv[]){
int menu;
int tp;
char user_name[20];
int sock,tam;
struct sockaddr_in name;
struct hostent *hp, *gethostbyname();
struct mensagem msg;

/*Cria o socket */
sock = socket (AF_INET, SOCK_DGRAM, 0);
if (sock<0){
  perror ("Abertura de socket");
  exit (1);
}

name.sin_family = AF_INET;
name.sin_port=htons(1234);

/* Acessa o DNS */
hp = gethostbyname("localhost");
if (hp==0){
  perror("Host nao encontrado");
  exit(2);
}

bcopy ((char*)hp->h_addr, (char *)&name.sin_addr, hp->h_length);
menu_primario(sock, name, user_name);    
sprintf(cli.user_name,"%s", user_name);
tp = pthread_create(&(producers), NULL, envia,(void *) &sock);
    if (tp)
    {
        printf("ERRO: impossivel criar a thread receptora\n");
        exit(EXIT_FAILURE);
    }

menu_secundario(sock, name, user_name);   
close (sock);
exit(0);

}

void menu_primario(int sock, struct sockaddr_in name, char *user_name){

    int menu;
    
    struct mensagem msg;
    int tam;
    tam=sizeof(name);
    printf("Menu de Opções\n:");
    printf("1-Realizar Cadastro\n");
    printf("2-Logar\n");

    scanf("%d", &menu);

    if(menu==1){
        printf("realize seu cadastro\n");
        printf("Digite o User Name que desejar:");
        __fpurge(stdin);
        scanf("%s", user_name);
        sprintf(msg.buf, "%s", user_name);
        msg.codigo= 0;
        msg.ip=name.sin_addr.s_addr;
        msg.porta=name.sin_port;

        if (sendto (sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name) < 0) 
        perror("Envio da mensagem");
        recvfrom(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *) &name, &tam);
        if(msg.resposta!=0) {
            printf("Cadastro Realizado Com sucesso!");
            printf("Usuario logado!\n");
            usuarios_online(msg, user_name);
        }
        else {
            printf("O cadastro não pode ser realizado");
      
        }
    }
    else{
        if(menu==2){
            printf("Digite o seu User Name:");
            __fpurge(stdin);
            scanf("%s", user_name);
            sprintf(msg.buf, "%s", user_name);
            msg.codigo= 1;
            msg.ip=name.sin_addr.s_addr;
            msg.porta=name.sin_port;
            if (sendto (sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name) < 0) 
            perror("Envio da mensagem");
            recvfrom(sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *) &name, &tam);
            if(msg.resposta!=0) {
                printf("Usuario logado!\n");
                usuarios_online(msg, user_name);
            }
            else {
                printf("O login não pode ser realizado");
        

            }
        }
    }
    
    
}

void usuarios_online(struct mensagem lista_nomes, char *meu_nome){
    struct clientes *lista;
    
    lista = malloc(sizeof(lista_nomes.buf));
    printf("\nLista de Usuario Online\n");
    bcopy(lista_nomes.buf, lista, sizeof(lista_nomes.buf));
    for(int i=0; i<lista_nomes.resposta; i++){
        if(strcmp(meu_nome, lista[i].user_name)!=0)
            printf("%s\n",lista[i].user_name);
    }
}
void menu_secundario(int sock, struct sockaddr_in name, char * user_name){
    int menu;
    struct mensagem msg;
   do{
    
    printf("\nMenu de opcoes\n");
    printf("1-Lista de Usuarios logados\n");
    printf("2-Lista de Seus contatos\n");
    printf("3-Iniciar Conversa\n");
    printf("4-Realizar Descadastro\n");
    printf("5-Sair\n");
    printf("\n Escolha uma opção:");
    __fpurge(stdin);
    scanf("%d", &menu);
    if(menu==1){ //l=usuarios online
        msg.codigo= 2;
        if (sendto (sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name) < 0) 
        perror("Envio da mensagem");
    }
    if(menu==2){
        printf("lista de contatos");
    }
    if(menu==3){

        char mensagem[1003];
        printf("Com quem deseja iniciar uma conversa?:");
        printf("Digite o user name:");
        scanf("%s",msg.buf);
        msg.codigo=3;
        name.sin_family = AF_INET;
        if (sendto (sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name) < 0) 
        perror("Envio da mensagem");
        while(1){
            
            msg.codigo=10;
            __fpurge(stdin);
            fgets(mensagem, 1003, stdin);
            char voltar[9];
            sprintf(voltar, "<voltar\n\0");
            if(strcmp(mensagem, voltar)==0) break;
            sprintf(msg.buf,"\n[%s]:%s",user_name, mensagem);
            cli.user.sin_family=AF_INET;
            if (sendto (sock, (char *)&msg, sizeof msg, 0,(struct sockaddr *) &(cli.user), sizeof (cli.user)) < 0) 
                perror("Envio da mensagem");
        }
        }
        if(menu==5){
            printf("Saindo.......");
            msg.codigo=5;
            sprintf(msg.buf, "%s", user_name);
            if (sendto (sock, (char *)&msg, sizeof msg, 0, (struct sockaddr *)&name, sizeof name) < 0)  
                perror("Envio da mensagem");
            exit(2);
            
        }
    }while(1);
        

}