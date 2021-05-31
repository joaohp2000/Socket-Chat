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

int count_online=0, count_total=1;

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
  
  celula *contato, *cliente;

  
	int rval;
  struct mensagem msg;
  int contato_online;
  struct sockaddr_in _cli;
  struct clientes cli, cli2;



  bcopy(&client[sock_id->thread_num], &_cli, sizeof(_cli) );
   if (sock_id->msgsock==-1)
			perror("accept");
		else do {

      printf("SSSS   msgsock:%d \n\n", sock_id->msgsock);

      if ((rval=recv(sock_id->msgsock,(char *) &msg,sizeof(msg),0))<0){
			  perror("reading stream message");
       
      }
			if (rval==0)
      {
				printf("Ending connection\n");
        printf("Vaaai\n");
        //busca_e_remove(cli, &lista);
        cliente->conteudo.online=0;
        count_online--;
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
          printf("DDDD  Porta: %d\n", _cli.sin_port);
          printf("SSSS   Meu sock:%d \n", sock_id->msgsock);
          printf("Meu nome:%s\n", msg.buf);
          cli.online=1;
          sprintf(cli.user_name, "%s", msg.buf);
          count_online++;
          msg.resposta=count_online;
          cli.sock=sock_id->msgsock;
          cliente=busca(cli, lista);
          if(cliente == NULL){
            printf("realizando cadastro do cliente");
            insere(cli, &lista);
            cliente=busca(cli, lista);
            count_total++;
          }
          else{
            printf("cliente encontrado");
          }

          cliente->conteudo.online=1;
          cliente->conteudo.sock=sock_id->msgsock;
          //sprintf(cliente->conteudo.user_name, "tessssste" );

          copiar(msg.buf, lista);
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
          perror("Envio da mensagem");

          
 
          break;
        case 2:
          msg.resposta=count_online;
          msg.codigo=2;
          copiar(msg.buf, lista);
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
            perror("Envio da mensagem");
          break;
        case 3:


          printf("buscando usuario\n");
          sprintf(cli2.user_name,"%s", msg.buf);
          //printf("%s", contato->conteudo.user_name);
          //scanf("%d", &x);
          //sprintf(contato->conteudo.user_name, "%s",msg.buf );
          contato=busca(cli2, lista);

          msg.codigo=3;
          if(contato==NULL){
          printf("Nao encontrado\n\n");
          msg.resposta=-1;
            if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
              perror("Envio da mensagem");
          }
          else{
            if(contato->conteudo.online==1){
                msg.resposta=1;
 //             bcopy(&(contato->conteudo), &cli2, sizeof(elemento));
        //      sprintf(msg.buf, "%s", contato->conteudo);
              printf ("de %s para  sock conversa : %d",cliente->conteudo.user_name, contato->conteudo.sock);
              printf("      nome:%s\n", contato->conteudo.user_name);
              if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
                perror("Envio da mensagem");

            }
            else{
  //            bcopy(&(contato->conteudo), &cli2, sizeof(elemento));
              msg.resposta=0;
              if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
                perror("Envio da mensagem");
            }
            ler_arquivo(contato->conteudo.user_name,cli.user_name,sock_id->msgsock);
          }

          break;
        case 4:
          msg.resposta=count_online;          
          msg.codigo=4;
          copiar(msg.buf, lista);
          if (send (sock_id->msgsock, (char *)&msg, sizeof msg, 0) < 0) 
            perror("Envio da mensagem");
          break;
        case 5:
            printf("Descadastrando Usuario");
            count_online--;
            count_total--;
            busca_e_remove(cliente->conteudo, &lista);
            close (sock_id->msgsock);
          pthread_exit(NULL);     
        case 6:
          printf("Vaaai\n");
          //busca_e_remove(cli, &lista);
          cliente->conteudo.online=0;
          cliente->conteudo.sock=0;
         // contato->conteudo.online=0;
          count_online--;
          close (sock_id->msgsock);
          pthread_exit(NULL);
          break;
        case 10:
        if(contato!=NULL){
           msg.codigo=10;
           printf("%d", contato->conteudo.online);
           if(contato->conteudo.online==1){
           printf("Envia para: %s e sock : %d\n", contato->conteudo.user_name, contato->conteudo.sock);
           if (send (contato->conteudo.sock, (char *)&msg, sizeof msg, 0) < 0) 
               perror("Envio da mensagem");
           }
           else{
             escrever_arquivo(contato->conteudo.user_name, cli.user_name, msg);
           }
        }
          break;
        case 11:
          msg.codigo=12;
          msg.resposta=sock_id->msgsock;
          if (send (contato->conteudo.sock, (char *)&msg, sizeof msg, 0) < 0) // notifica o outro usuario
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