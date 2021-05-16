/**
	@author: KANE MAMADOU & SALL AIDA
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define nb_animaux 14

/********************************** PROTOTYPE DES FONCTIONS ********************/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur);
int initialiser_socket(unsigned short port_serveur);
void set_interruption(int signal);
void quitter_proprement(int signal);
void *visiter_zoo(void *arg);
int main(int argc, char** argv);

/**************************** VARIABLES GLOBALES *************************/
int sock = 0;
pthread_t thread;
int interruption =1;

char *animaux[] = {
  "chat",
  "cheval",
  "chien",
  "chimpanzes",
  "cobra_royal",
  "crocodile",
  "lapin",
  "leopard",
  "oiseau",
  "panda",
  "panthere",
  "paon",
  "renard_polaire",
  "singe"
};



/********************************* INIT_SOCKADDR_IN ***************************/

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec toutes les adresses ips possibles, sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port auquel la socket sera liée.
   @require serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur){
  	serveur->sin_family = AF_INET;
  	serveur->sin_port = htons(port_serveur);
  	serveur->sin_addr.s_addr = htonl(INADDR_ANY);
}

/************************************ INITIALISER SOCKET  ******************************/

/**
   Initialise une socket permettant d'accepter des communications en UDP depuis n'importe
   qu'elle adresse IP.   
   @param port_serveur est le port auquel la socket sera liée.
   @return une socket prête à l'emploi si aucune erreur de survient, quitte le programme sinon.
*/
int initialiser_socket(unsigned short port_serveur){
	struct sockaddr_in serveur;
	int sock;
	init_sockaddr_in(&serveur, port_serveur);
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Erreur lors de la création de socket UDP");
		exit(EXIT_FAILURE);
	}  
	if(bind(sock, (struct sockaddr *) &serveur, sizeof(struct sockaddr_in)) < 0){
		perror("Erreur lors de l'appel à la fonction bind");
		exit(EXIT_FAILURE);
	}
  	return sock;
}

/***************************** SET_INTERRUPTION ****************************/

/**
	Cette fonction sera éxecutée quand le signal SIGINT sera reçu.
	@param signal 
 */
void set_interruption(int signal) {
	printf("Au revoir\n");
	pthread_kill(thread,SIGUSR1 );
	interruption = 0;
}

/****************************** QUITTER_PROPREMENT ***************************/

/**
	Cette fonction sera éxecutée quand le signal SIGUSR2 sera reçu sur le thread courant .
	@param signal 
 */
void quitter_proprement(int signal){
	pthread_exit(NULL);
}

/****************************** VISITER_ZOO ************************************/

/**
    Cette fonction reçoit le nom d'animal que le client veut voir puis lui envoie
	le chemin complet vers cette image. 
    @param arg est la socket utilisé pour communiquer.
	@requires arg != NULL.
*/
void *visiter_zoo(void *arg){
	int sock = *(int*)arg;
	struct sockaddr_in client;
	size_t client_len = sizeof(client);
	char animal[20];
	char chemin_image[50] = "../animaux/";
	int ret;
	
	signal(SIGUSR1 , quitter_proprement);
	
	ret = recvfrom(sock, &animal, sizeof(animal), 0, (struct sockaddr *)&client, (socklen_t *)&client_len);
	if(ret < 0)	perror("Erreur recvfrom animal");
	printf("connecté\n");
	printf("Le client veut voire: %s\n",animal);
	strcat(chemin_image,animal);
	strcat(chemin_image, ".bmp");
	
	if(sendto(sock, chemin_image, sizeof(chemin_image),0,
			(struct sockaddr *)&client, sizeof(client)) < 0)
		perror("Erreur sendto");
	pthread_exit(NULL);
}

/******************************* MAIN ************************************/

/**
	Ce programme crée un serveur dédié qui attend la connexion des clients et
	lance un thread qui va être éxécuter 
	@param argc c'est pour le nombre d'arguments
  @param argv correspond aux arguments passés lors de l'éxécution (voir makefile)
	@requires argc != 2
*/
int main(int argc, char** argv) {

	short port = 0;
	
	if (argc!=2) {
		fprintf(stderr,"usage: %s port\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	port = atoi(argv[1]);

	sock = initialiser_socket(port);
	
	signal(SIGINT , set_interruption);
	
	while (interruption == 1) {
		
		int *arg = malloc(sizeof(int));
		*arg = sock;
		pthread_create(&thread ,NULL , visiter_zoo, arg);						
		pthread_join(thread,NULL);
		free(arg);	
	}
	return EXIT_SUCCESS;
}
