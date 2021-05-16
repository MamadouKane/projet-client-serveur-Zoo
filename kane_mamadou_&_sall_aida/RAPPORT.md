
Notre projet consiste à faire une implémentation en C du type client-serveur avec un graceful shutdown. Il s'agit d'une plateforme qui fait la présentation d'un ZOO.
L'adresse ip utilisé 127.0.0.1
Port utilisé 50000 .
On a choisi le protocole UDP parce qu'il permet une communication rapide sans delai. Aussi c'est le protocole qu'on comprend le plus.

CLIENT:
Si on éxécute le client, il affiche une image d'accueil qui est comme genre le logo du ZOO. Il lui affiche aussi la liste d'animaux disponibles dans le ZOO et il demande à l'utilisateur de choisir l'animal qu'il veut visiter grâce à la fonction visiter_zoo, l'utilisateur choisit et le client lui affiche cet animal.
La fonction afficher_image en C et en SDL permet l'affichage des images dont le chemin est donné en paramètre pour l'utilisateur, elle permet de gérer la taille de l'image , la durée d'affichage, la couleur...
On lui a fait une fonction init_sockaddr_in qui initialise la structure sockaddr_in pour que la socket qui y sera liée puisse communiquer avec le serveur.
La fonction initialiser_socket Initialise une socket UDP, et la structure serveur qui contienne des informations sur l'ip et le port utilisé .

SERVEUR:
Le serveur de son côté, attend que le client se connecte pour qu'il accepte.
Pour le graceful shutdown, on a fait une fonction set_interruption qui sera éxecutée quand le signal SIGINT sera reçu et une fonction quitter_proprement qui sera éxecutée quand le signal SIGUSR1 sera reçu sur le thread courant.
La fonction visiter_zoo reçoit le nom d'animal que le client veut voir puis lui envoie le chemin complet vers cette image. 
init_sockaddr_in, cette fonction Initialise la structure sockaddr_in pour que la socket qui y sera liée puisse communiquer avec toutes les adresses ips possibles.