#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h> /* para poder manipular threads */
#include <unistd.h>

pthread_t recebe;
pthread_mutex_t lock;
pthread_mutex_t lock2;
pthread_mutex_t lock3;
int resposta;
struct clientes{
    char user_name[24];
    int so;
    int online;
};

struct send_file{
    char buffer[35700];
    int size;
    char nome_arquivo[30];
};

struct mensagem{
  int codigo;
  int resposta;
  long ip;
  int porta;
  char buf[1024];
};

void menu_primario(int sock, struct sockaddr_in name, char *user_name);
void menu_secundario(int sock, struct sockaddr_in name, char *user_name, struct clientes * cli, struct mensagem * msg);
void usuarios_online(struct mensagem lista_nomes, char *meu_nome);
void lista_de_contatos(struct mensagem lista_nomes, char *meu_nome);
void le_linha(FILE * arquivo, char *linha);
void envia_arquivo(struct mensagem contato);
void recebe_arquivo(int sock,int sock_contato);
void grupos(int sock, char *user_name);
void *envia(void *sock_)
{
    char user_name[20];
    int tam;
    
    //struct mensagem msg;

    struct mensagem *msg2;
    struct mensagem msg;
    struct clientes *cli;
    int sock; 

    bcopy(sock_,&sock, sizeof(int));
    bcopy(sock_+sizeof(int), user_name, sizeof(user_name));
    bcopy(sock_+sizeof(int)+sizeof(user_name), &msg2,  sizeof(struct mensagem *));
    

 

    while(1){
    recv((int) sock, (char *) &msg, sizeof ( msg), 0);

    if(msg.codigo==2){
        usuarios_online(msg, user_name);
        pthread_mutex_unlock(&lock);

    }
    if(msg.codigo == 3){
        resposta= msg.resposta;
        pthread_mutex_unlock(&lock3);
    }
    if(msg.codigo==4){
        lista_de_contatos(msg, user_name);
        pthread_mutex_unlock(&lock);


    }
    if(msg.codigo==10){

        printf("%s", msg.buf);
    }
    if(msg.codigo==11){
        envia_arquivo(msg);
        pthread_mutex_unlock(&lock2);
    }
    if(msg.codigo==12){
        recebe_arquivo(sock, msg.resposta);
        pthread_mutex_unlock(&lock);
    }
    }

    pthread_exit(NULL);
}

int main(){
    int tp;
    char user_name[20];
    struct mensagem msg;
    struct clientes cli;

	int sock, cont;
	struct sockaddr_in server;
	struct hostent *hp, *gethostbyname();
	char buf[1024];

   
    struct clientes *cli1=&cli;
    struct mensagem *msg1=&msg;
    void *sock_name;

        /* Cria o socket de comunicacao */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock<0) {
	/*
	/- houve erro na abertura do socket
	*/
		perror("opening stream socket");
		exit(1);
	}

	/* Associa */
	server.sin_family=AF_INET;
        hp = gethostbyname("localhost");
        if (hp==0) {
            fprintf(stderr, "Unknown host\n");
            exit(2);
        }
        bcopy ((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port = htons(1234);


  //printf ("CCC  Digite a mensagem a ser enviada:");
  //fgets(buf, 1023, stdin);
  
	if (connect (sock,(struct sockaddr *)&server,sizeof server)<0) {
                perror("connecting stream socket");
		exit(1);
	}

    printf("CCC Conexao realizada com o servidor:\n");
    printf("CCC  IP: %s   porta:%d \n\n", inet_ntoa(server.sin_addr), server.sin_port);

    menu_primario(sock, server, user_name);
    sprintf(cli.user_name,"%s", user_name);

    sock_name=malloc(sizeof(int)+ sizeof(user_name)+sizeof(struct mensagem *));
    bcopy(&sock,sock_name, sizeof(int ));
    bcopy(&user_name, sock_name+sizeof(int), sizeof(user_name));
    bcopy(&msg1, sock_name+sizeof(int)+sizeof(user_name), sizeof(struct mensagem *));
     if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
        if (pthread_mutex_init(&lock2, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&lock3, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    tp = pthread_create(&(recebe), NULL, envia,sock_name);
        if (tp)
        {
            printf("ERRO: impossivel criar a thread receptora\n");
            exit(EXIT_FAILURE);
        }

    menu_secundario(sock, server, user_name, cli1, msg1);   
        close(sock);
        exit(0);
}

void menu_primario(int sock, struct sockaddr_in name, char *user_name){

    int menu;
    FILE *lista_de_contatos;
    lista_de_contatos=fopen("lista_de_contatos.txt", "a");
    fclose(lista_de_contatos);
    struct mensagem msg;
    int tam;
    tam=sizeof(name);
    printf("Logar\n");
    printf("Digite o seu User Name:");
    __fpurge(stdin);
    scanf("%s", user_name);
    sprintf(msg.buf, "%s", user_name);
    msg.codigo= 1;
    msg.ip=name.sin_addr.s_addr;
    msg.porta=name.sin_port;
    if (send (sock, (char *)&msg, sizeof msg, 0) < 0) 
    perror("Envio da mensagem");
    recv(sock, (char *)&msg, sizeof msg, 0);
    if(msg.resposta!=0) {
        printf("Usuario logado!\n");
        usuarios_online(msg, user_name);
    }
    else {
        printf("O login não pode ser realizado");
    }
}
void usuarios_online(struct mensagem lista_nomes, char *meu_nome){
    struct clientes *lista;
    system("clear");
    if(lista_nomes.resposta!=1){
        lista = malloc(sizeof(lista_nomes.buf));
        printf("\nLista de Usuario Online\n");
        bcopy(lista_nomes.buf, lista, sizeof(lista_nomes.buf));
        for(int i=0; i<lista_nomes.resposta-1; i++){
            if(strcmp(meu_nome, lista[i].user_name)!=0)
                printf("%s\n",lista[i].user_name);
        }
    }
    else{
        printf("\nNenhum usuario logado ainda!\n");
    }
}
void menu_secundario(int sock, struct sockaddr_in name, char * user_name, struct clientes * cli,struct mensagem * msg ){
    struct mensagem msg2;
    int menu;

   do{
        pthread_mutex_lock(&lock);

        printf("\nMenu de opcoes\n");
        printf("1-Lista de Usuarios logados\n");
        printf("2-Lista de Seus contatos\n");
        printf("3-Iniciar Conversa\n");
        printf("4-Realizar Descadastro\n");
        printf("5-Iniciar Chat em grupo\n");
        printf("6-Sair\n");
        printf("\n Escolha uma opção:");
        __fpurge(stdin);
        scanf("%d", &menu);
        if(menu==1){ //l=usuarios online

            msg->codigo= 2;
            msg2.codigo=2;
            if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
            perror("Envio da mensagem");
        }
        if(menu==2){ //lista de contatos
            msg->codigo= 4;
            msg2.codigo=4;
            if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
            perror("Envio da mensagem");

        }
        if(menu==3){ //iniciar conversa
            system("clear");
            pthread_mutex_unlock(&lock);
            pthread_mutex_lock(&lock3);
            char mensagem[1003];
            printf("Com quem deseja iniciar uma conversa?:");
            printf("Digite o user name:");
            scanf("%s",msg2.buf);
            msg2.codigo=3;
            name.sin_family = AF_INET;
            if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
            perror("Envio da mensagem");
            pthread_mutex_lock(&lock3);
            if(resposta!=-1){
            while(1){
                pthread_mutex_lock(&lock2);

                msg2.codigo=10;
                __fpurge(stdin);
                fgets(mensagem, 1003, stdin);
               
                char voltar[9];
                sprintf(voltar, "<voltar\n\0");
                if(strcmp(mensagem, "<voltar\n\0" )==0) 
                {
                    pthread_mutex_unlock(&lock2);
                    break;
                }
                if(strcmp(mensagem, "arquivo()\n\0" )==0){

                    msg2.codigo=11;
                                    
                    if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
                        perror("Envio da mensagem");
                    }

                else{
                
                sprintf(msg2.buf,"\n[%s]:%s",user_name, mensagem);
                
                if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
                    perror("Envio da mensagem");
                pthread_mutex_unlock(&lock2);
                }
            }
    
            }
            pthread_mutex_unlock(&lock3);
        }
        if(menu==4){
            printf("Realizando Descadastro\n");
            msg2.codigo=5;
                if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
            perror("Envio da mensagem");
            pthread_mutex_destroy(&lock);
            pthread_mutex_destroy(&lock2);
            close(sock);
            exit(0);
        }
        if(menu==5){
            pthread_mutex_unlock(&lock);
            grupos(sock, user_name);
        }
        if(menu==6){
                printf("Saindo.......");
                msg2.codigo=6;

                sprintf(msg->buf, "%s", user_name);
                if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
                    perror("Envio da mensagem");
                pthread_mutex_destroy(&lock);
                pthread_mutex_destroy(&lock2);
                close(sock);
                exit(0);

            }
    }while(1);


}

void lista_de_contatos(struct mensagem lista_nomes, char *meu_nome){
    FILE *lista_de_contato;
    struct clientes *lista;
    int lenz, flag, entrada=0;
    size_t len=20; // valor arbitrário
    char *linha= malloc(len);
    system("clear");
    __fpurge(stdout);
    __fpurge(stdout);
       
      printf("\n lista de Contatos\n");
      lista = malloc(sizeof(lista_nomes.buf));

    while(entrada != 3){

        printf("1-Adicionar Contato\n");
        printf("2-Ver minha lista \n");
        printf("3-Voltar ao menu principal\n");
        printf("\n Escolha uma opção:");
        __fpurge(stdin);
        scanf("%d", &entrada);
        switch (entrada){
        case 1:
            
            lista_de_contato=fopen("lista_de_contatos.txt", "a");
            char contato[20];
            printf("Nome do Contato:");
            __fpurge(stdin);
            scanf("%s", contato);
            fprintf(lista_de_contato, "%s\n", contato);
            fclose(lista_de_contato);
            break;

        case 2:
            bcopy(lista_nomes.buf, lista, sizeof(lista_nomes.buf));
            printf("\n\nMinha lista de Contatos\n\n");
            lista_de_contato=fopen("lista_de_contatos.txt", "r");
            if (!lista_de_contato)
              {
                perror("lista_de_contatos.txt");
                exit(1);
              }
            while (getline(&linha, &len, lista_de_contato) > 0){
                lenz = strlen(linha); 
                if( linha[lenz-1] == '\n' ) linha[lenz-1] = 0;
                flag=0;
                for(int i=0; i<lista_nomes.resposta-1; i++){
                    if(strcmp(meu_nome, lista[i].user_name)!=0){
                        if(strcmp(lista[i].user_name, linha)==0){
                            printf("%s     [ONLINE]\n", linha);
                            flag=1;
                        }
                    }
                }
                if(flag==0){
                    printf("%s     [OFFLINE]\n", linha);
                }
              }
            
            fclose(lista_de_contato);
            break;

        default:
            break;
        }
    }
    free(linha);

}
void envia_arquivo(struct mensagem contato){
    system("clear");
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *hp, *gethostbyname();
    struct sockaddr_in contatin;
    //bcopy(contato.buf, &contatin,sizeof(contatin) );
   /* printf("CCC Conexao realizada com o servidor:\n");
    printf("CCC  IP: %s   porta:%d \n\n", inet_ntoa(contatin.sin_addr), contatin.sin_port);
	*/if(sock<0) {
	/*
	/- houve erro na abertura do socket
	*/
		perror("opening stream socket");
		exit(1);
	}
    contatin.sin_family=AF_INET;
        hp = gethostbyname("localhost");
        if (hp==0) {
            fprintf(stderr, "Unknown host\n");
            exit(2);
        }
        bcopy ((char *)hp->h_addr, (char *)&contatin.sin_addr, hp->h_length);
	contatin.sin_port = htons(4321);
	if (connect (sock,(struct sockaddr *)&contatin,sizeof contatin)<0) {
                perror("connecting stream socket");
		exit(1);
	}
/*
    printf("CCC Conexao realizada com o servidor:\n");
    printf("CCC  IP: %s   porta:%d \n\n", inet_ntoa(contatin.sin_addr), contatin.sin_port);
    
     */ 
    printf("Seus arquivos:\n");
    system("ls");
    printf("Digite o nome do arquivo que deseja enviar:");
  struct send_file buff;
    __fpurge(stdin);
    scanf("%s", buff.nome_arquivo);
    //fgets(nome_arquivo, 19,stdin);
    //   nome_arquivo[strlen(nome_arquivo)-1]=0;
    FILE *arquivo;
    arquivo=fopen(buff.nome_arquivo, "r");
    if(arquivo ==NULL){
        printf("O arquivo nao pode ser aberto\n");
    }
    else{
        fseek(arquivo, 0, SEEK_END); // seek to end of file
        buff.size = ftell(arquivo); // get current file pointer
        fseek(arquivo, 0, SEEK_SET); // seek back to beginning of file
        // proceed with allocating memory and reading the file
       // buff.buffer = malloc(buff.size);
       // printf("Tamanho do arquivo: %d\n", buff.size);
        fread(buff.buffer,1, buff.size,arquivo);
        printf("Tamanho do arquivo: %d\n", buff.size);
        printf("Nome do arquivo: %s\n", buff.nome_arquivo);
        if (send (sock, (void *)&buff, sizeof(buff), 0) < 0)
        perror("Envio da mensagem");
    }
    fclose(arquivo);
    close(sock);
      
}
void recebe_arquivo(int sock, int sock_contato){

    
    int sock_serv;
    struct sockaddr_in serv;
    struct sockaddr_in contato;
    int length;
    struct mensagem msg;
    int msgsock;

    sock_serv=socket(AF_INET, SOCK_STREAM, 0);
	if (sock_serv<0) {
		perror("opening stream socket");
		exit(1);
	}
	
	/* Associa */
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = INADDR_ANY;
	serv.sin_port=htons(4321);
	if (bind(sock_serv,(struct sockaddr *) &serv, sizeof serv) < 0) {
		perror("binding stream socket");
		exit(1);
	}

	/* Conexoes */
	int rval;
	if (listen(sock_serv, 5) == -1) 
			printf("errou");
	length= sizeof(struct sockaddr_in);

    bcopy(&serv, msg.buf, length);
    msg.resposta=sock_contato;
    msg.codigo=12;
    if (send (sock, (char *)&msg, length, 0) < 0)
        perror("Envio da mensagem");
    msgsock=accept(sock_serv,(struct sockaddr *)&contato, &length);
    if (msgsock==-1)
			perror("accept");

    struct send_file buff;

    if ((rval=recv(msgsock,(void *) &buff, sizeof(buff),0))<0){
	  perror("reading stream message");
    }
    else{
        printf("Tamanho do arquivo: %d\n", buff.size);
        printf("Nome do arquivo: %s\n", buff.nome_arquivo);
        //fgets(nome_arquivo, 19,stdin);
    //    nome_arquivo[strlen(nome_arquivo)-1]=0;
        FILE *arquivo;
        arquivo=fopen(buff.nome_arquivo, "w");
        //arquivo=fopen("1.txt", "w");
        if(arquivo ==NULL){
            printf("O arquivo nao pode ser aberto\n");
        }
        else{

            // proceed with allocating memory and reading the file
 
            
           // buff.buffer = malloc(buff.size);
            //printf("%s", buff.buffer);
            fwrite(buff.buffer, buff.size,1,arquivo);
            fclose(arquivo);


        }
    }
    close(msgsock);
    close(sock_serv);
    printf("fim\n");

}

void grupos(int sock, char *user_name){
    system("clear");
    struct mensagem msg2; 
    char linha[10][20];
    int l;
    char mensagem[1003];
    printf("Digite quantas pessoas e seus nomes:");
    scanf("%d", &l);
    for(int i=0; i<l;i++){
        scanf("%s", linha[i]);
    }
    while(1){
        
        __fpurge(stdin);
        fgets(mensagem, 1003, stdin);

        if(strcmp(mensagem, "<voltar\n\0" )==0) 
        {
            break;
        }
        else{
            for(int i=0; i<l;i++){
                pthread_mutex_lock(&lock3);
                msg2.codigo=3;
                bcopy(linha[i], msg2.buf, 20 );
                if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
                    perror("Envio da mensagem");
                pthread_mutex_lock(&lock3);
                if(resposta!=-1){
                msg2.codigo=10;
                sprintf(msg2.buf,"\n[%s]:%s",user_name, mensagem);
                if (send (sock, (char *)&msg2, sizeof msg2, 0) < 0) 
                    perror("Envio da mensagem");
                pthread_mutex_unlock(&lock3);
                usleep(500);
                }
        
        }
            }

    }
        
}
