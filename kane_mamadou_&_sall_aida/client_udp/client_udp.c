/**
    @author: KANE MAMADOU & SALL AIDA
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <unistd.h>
#include <limits.h>
#include <SDL2/SDL.h>

#define nb_animaux 14

/**************************** PROTOTYPE DES FONCTIONS ********************/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur, char * ip_serveur);
int initialiser_socket(char * ip_serveur, unsigned short port_serveur, struct sockaddr_in * serveur);
void afficher_image(char * chemin_image);
void visiter_zoo(int sock, struct sockaddr_in serveur);
int main(int argc, char** argv);

/******************************* VARIABLES GLOBALES ***************************/

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


/**************************** INIT_SOCKADDR_IN *******************/

/**
   Initialise la structure sockaddr_in pour que la socket qui y sera liée
   puisse communiquer avec le serveur d'adresse 'ip_serveur', sur le port 'port_serveur'
   @param serveur est un pointeur vers la structure que l'on souhaite initialiser
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @require serveur != NULL && ip_serveur != NULL
*/
void init_sockaddr_in(struct sockaddr_in * serveur, unsigned short port_serveur, char * ip_serveur){
	serveur->sin_family = AF_INET;
	serveur->sin_port = htons(port_serveur);
	inet_aton(ip_serveur, &serveur->sin_addr);
}

/**************************** INITIALISER_SOCKET *******************/

/**
   Initialise une socket UDP, et la structure serveur qui contienne des informations 
   sur l'ip et le port utilisés.
   @param ip_serveur est l'adresse du serveur avec lequel on souhaite communiquer
   @param port_serveur est le port sur serveur avec lequel on souhaite communiquer
   @param serveur est un pointeur vers la structure représentant les informations du serveur
   @require serveur != NULL && ip_serveur != NULL
   @return la socket initialisée, quitte le programme en cas d'échec.
*/
int initialiser_socket(char * ip_serveur, unsigned short port_serveur, struct sockaddr_in * serveur){
	int sock;
	init_sockaddr_in(serveur, port_serveur, ip_serveur);
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Erreur lors de la création de socket UDP");
		exit(EXIT_FAILURE);
	}
	return sock;
}


/************************ AFFICHER_IMAGE ***************************/

/**
    Cette fonction permet l'affichage des images dont le chemin est donné en paramètre.  
    @param chemin_image est le chemin de l'image.
    @requires chemin_image != NULL.
 */
void afficher_image(char * chemin_image){
        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;
        SDL_Texture *texture = NULL;
        int statut = EXIT_FAILURE;
        SDL_Color blanc = {200, 200, 200, 0};
        
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) != 0 ){
            fprintf(stderr,"Erreur SDL_Init : %s", SDL_GetError());
            goto Quit;
        }

        if(SDL_CreateWindowAndRenderer(840, 680, SDL_WINDOW_SHOWN, &window, &renderer) != 0){
            fprintf(stderr, "Erreur SDL_CreateWindowAndRenderer : %s", SDL_GetError());
            goto Quit;
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 200, 200);
        if(texture == NULL){
            fprintf(stderr, "Erreur SDL_CreateTexture : %s", SDL_GetError());
            goto Quit;
        } 

        SDL_SetRenderDrawColor(renderer, blanc.r, blanc.g, blanc.b, blanc.a);
        SDL_RenderClear(renderer);

        SDL_Surface *tmp = NULL;
        tmp = SDL_LoadBMP(chemin_image);
        if(tmp == NULL){
            fprintf(stderr, "Erreur SDL_LoadBMP : %s\n", SDL_GetError());
            goto Quit;
        }
        texture = SDL_CreateTextureFromSurface(renderer, tmp);

        if(texture == NULL){
            fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
            goto Quit;
        }

        SDL_Rect dstrect = { 0, 0, 840, 680};
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_RenderPresent(renderer);
        SDL_Delay(5000);
Quit:
        if(tmp != NULL)
            SDL_FreeSurface(tmp);
        if(texture != NULL)
            SDL_DestroyTexture(texture);
        if(renderer != NULL)
            SDL_DestroyRenderer(renderer);
        if(window != NULL)
            SDL_DestroyWindow(window);

        SDL_Quit();
}


/******************************** VISITER_ZOO **********************/

/**
    Cette fonction affiche la liste de animaux disponibles , demande à l'utilisateur 
    l'animal qu'il veut voir et puis fait appel à la fonction afficher_image pour l'affichage 
    de lanimal choisi. 
    @param sock est la socket utilisé pour communiquer.
    @param serveur permet de préciser le destinataire du message
    @requires sock et serveur sont déjà initialisés.
 */
void visiter_zoo(int sock, struct sockaddr_in serveur){
	char animal[20] , chemin_image[50];
	int ret;
	size_t serveur_len = sizeof(serveur);

	printf("/**************************** VOICI LA LISTE DES ANIMAUX DU ZOO ***************\n/");

    for(int i = 0; i < nb_animaux ; i++){
        printf("%d \t %s\n",i ,animaux[i]);
    }

	printf("Saisissez un nom d'animal: ");
	scanf("%s", animal);
    ret = sendto(sock, &animal, 20*sizeof(char), 0, (struct sockaddr *)&serveur, serveur_len);
    if(ret < 0) perror("sendto animal");

	ret = recvfrom(sock, &chemin_image, sizeof(chemin_image), 0,
			(struct sockaddr *)&serveur, (socklen_t *)&serveur_len);
	if(ret < 0)
        perror("Erreur recvfrom");
    else
	    afficher_image(chemin_image);
}


/**************************** MAIN ***************************/

/**
   Dans ce programme, à l'aide du protocole UDP, on affiche la liste de tous les animaux disponibles 
   et on affiche l'image de l'animal choisi.
   @param argc c'est pour le nombre d'arguments
   @param argv correspond aux arguments passés lors de l'éxécution
   @requiresa rgc != 3
*/
int main(int argc, char** argv) {
	struct sockaddr_in serveur; 
	short port_serveur = 0;
	char * ip_serveur;
	int sock = 0; /* socket de communication */

	if (argc != 3) {
		fprintf(stderr,"usage: %s host port_serveur\n", argv[0]);
		return EXIT_FAILURE;
	}
	port_serveur = atoi(argv[2]);
	ip_serveur = argv[1];

	sock = initialiser_socket(ip_serveur, port_serveur, &serveur);
	    
    afficher_image("../animaux/bienvenu.bmp");

	visiter_zoo(sock, serveur);

    close(sock);
	
	return EXIT_SUCCESS;
}









