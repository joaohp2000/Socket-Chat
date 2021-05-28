#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h> /* para poder manipular threads */
#include <unistd.h>

#define TRUE 1

pthread_t usuario[1024];

struct sockaddr_in client[1024];

struct clientes{
    char user_name[24];
    int sock;
    int online;

};

typedef struct clientes elemento;

#include "lista.h"

celula *lista = NULL;
int indice=1;

struct mensagem{
  int codigo;
  int resposta;
  long ip;
  int porta;
  char buf[2048];
};

struct thread_param{
  int msgsock;
  int thread_num;
};
void escrever_arquivo(char *nome, char *meu_nome, struct mensagem msg);
void ler_arquivo(char *nome, char *meu_nome,int sock);
void *user(void * sock){
  struct thread_param * sock_id = (struct thread_param *)sock;

	int rval;
  struct mensagem msg;
  int contato_online;
  struct sockaddr_in _cli;
  struct clientes cli;
  struct clientes cli2;
  cli.sock=sock_id->msgsock;
  bcopy(&client[sock_id->thread_num], &_cli, sizeof(_cli) );
   if (sock_id->msgsock==-1)
			perror("accept");
		else do {
      printf("SSSS   Conexão com cliente:\n");
      printf("SSSSS  IP: %s   porta:%d \n\n", inet_ntoa(_cli.sin_addr), _cli.sin_port);

      printf("SSSS   msgsock:%d \n\n", sock_id->msgsock);

      if ((rval=recv(sock_id->msgsock,(char *) &msg,sizeof(msg),0))<0){
			  perror("reading stream message");
       
      }
			if (rval==0)
      {
				printf("Ending connection\n");
        printf("Vaaai\n");
        busca_e_remove(cli, &lista);
        indice--;
        close (sock_id->msgsock);
        pthread_exit(NULL);
		    break;
      }
			else {
        switch (msg.codigo)
        {
        case 1:
          printf("Realizando login!\n");
          printf("DDDD  Familia: %d\n", _cli.sin_family);
          printf("DDDD  Endereco: %s\n", inet_ntoa(_cli.sin_addr));
          printf("DDDD  Porta: %d\n\n", _cli.sin_port);
          
          cli.online=1;
          sprintf(cli.user_name, "%s", msg.buf);
          msg.resposta=indice;

          insere(cli, &lista);
          copiar(msg.buf, lista);
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
            perror("Envio da mensagem");
          indice++;
          break;
        case 2:
          printf("Usuários Logados:\n");
          printf("DDDD  Familia: %d\n", _cli.sin_family);
          printf("DDDD  Endereco: %s\n", inet_ntoa(_cli.sin_addr));
          printf("DDDD  Porta: %d\n\n", _cli.sin_port);
          msg.resposta=indice;
          msg.codigo=2;
          copiar(msg.buf, lista);
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
            perror("Envio da mensagem");
          break;
        case 3:
        printf("buscando usuario\n");
        
        celula *user;
        user=NULL;
        user = malloc(sizeof(celula *));
        sprintf(user->conteudo.user_name,"%s", msg.buf);
        //printf("%s", user->conteudo.user_name);
        //scanf("%d", &x);
        sprintf(cli2.user_name, "%s",msg.buf );
        user=busca(user->conteudo, lista);
        
        msg.codigo=3;
        if(user==NULL){
        printf("Nao encontrado\n\n");
        contato_online=0;
        msg.resposta=-1;
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
            perror("Envio da mensagem");
        }
        else{
          contato_online=1;
          bcopy(&(user->conteudo), &cli2, sizeof(elemento));
      //    sprintf(msg.buf, "%s", user->conteudo);
          printf ("sock conversa : %d", cli2.sock);
          printf("      nome:%s\n", cli2.user_name);
          msg.resposta=1;
  
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
            perror("Envio da mensagem");
        }
        ler_arquivo(cli2.user_name,cli.user_name,sock_id->msgsock);
      
        
          break;
        case 4:
          printf("Usuários Logados:\n");
          printf("DDDD  Familia: %d\n", _cli.sin_family);
          printf("DDDD  Endereco: %s\n", inet_ntoa(_cli.sin_addr));
          printf("DDDD  Porta: %d\n\n", _cli.sin_port);
          msg.resposta=indice;
          msg.codigo=4;
          copiar(msg.buf, lista);
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
            perror("Envio da mensagem");
          break;
        case 5:
          printf("Vaaai\n");
          busca_e_remove(cli, &lista);
          indice--;
          close (sock_id->msgsock);
          pthread_exit(NULL);
          break;
        case 10:
           if(contato_online!=0){
           msg.codigo=10;
           printf("Envia para: %s e sock : %d\n", cli2.user_name, cli2.sock);
           if (send (cli2.sock, (char *)&msg, sizeof msg, 0) < 0) 
               perror("Envio da mensagem");
           }
           else{
             escrever_arquivo(cli2.user_name, cli.user_name, msg);
           }
          break;
        case 11:
          msg.codigo=12;
          msg.resposta=sock_id->msgsock;
          if (send (cli2.sock, (char *)&msg, sizeof msg, 0) < 0) // notifica o outro usuario
            perror("Envio da mensagem");  
          break;
        case 12:
          msg.codigo=11;
          printf("%d\n", msg.resposta);
          if (send (msg.resposta, (char *)&msg, sizeof msg, 0) < 0)
            perror("Envio da mensagem");   
          break;

        default:
          break;
        }
          msg.codigo=-1;
      }
		} while (1);
    pthread_exit(NULL);
}



int main(void)
{

	int sock, length, cont=0;
	struct sockaddr_in server;
	int msgsock;

  int tp;
  int clientes=1;



	/* Cria o socket */
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if (sock<0) {
		perror("opening stream socket");
		exit(1);
	}
	
	/* Associa */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port=htons(1234);
	if (bind(sock,(struct sockaddr *) &server, sizeof server) < 0) {
		perror("binding stream socket");
		exit(1);
	}

	/* Conexoes */
	
	if (listen(sock, 5) == -1) 
			printf("errou");
	length= sizeof(struct sockaddr_in);
    printf("DDDD  Familia: %d\n", server.sin_family);
    printf("DDDD  Endereco: %s\n", inet_ntoa(server.sin_addr));
    printf("DDDD  Porta: %d\n\n", server.sin_port);
  struct thread_param sock_id[1024];
  while(1){
      sock_id[clientes].thread_num=clientes;
      sock_id[clientes].msgsock=accept(sock,(struct sockaddr *)&client[clientes], &length);
		  pthread_create(&usuario[clientes], NULL, user, (void *) &sock_id[clientes]);
		clientes++;
	}
  for(int i = 0 ; i < clientes ; i ++)
		pthread_join(usuario[i],NULL);		
	exit(0);
}
void escrever_arquivo(char *nome, char *meu_nome, struct mensagem msg){
  FILE *meu_arquivo;
  char file[30];
  sprintf(file, "%s_%s.txt", nome, meu_nome);
  meu_arquivo=fopen(file, "a+");
  if(meu_arquivo ==NULL){
    printf("O arquivo nao pode ser aberto\n");
  }
  fprintf(meu_arquivo, "%s", msg.buf);
  fclose(meu_arquivo);
}
void ler_arquivo(char *nome,char *meu_nome, int sock){
  FILE *meu_arquivo;
  char file[30];
  sprintf(file, "%s_%s.txt",meu_nome,nome);
  meu_arquivo=fopen(file, "r");
  if(meu_arquivo ==NULL){
    printf("O arquivo nao pode ser aberto\n");
  }
  else{
  int size;
  struct mensagem msg;

 // buff.buffer = malloc(buff.size);
 // printf("Tamanho do arquivo: %d\n", buff.size);
  while((fgets(msg.buf, 1003, meu_arquivo))!=NULL){
     msg.codigo=10;
    usleep(500);
    printf("%s", msg.buf);
      if (send (sock,(void *) &msg, sizeof(msg), 0) < 0)
      perror("Envio da mensagem");
 }
 
  
  fclose(meu_arquivo);
  remove(file);
  }
}