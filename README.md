# Pour pouvoir utiliser les librairies SDL2 sur linux voici la commande 
    sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

# coté serveur 
  ## Compilation 
        make ou make build qui correspond a :
            gcc serveur_udp.c -o serveur -lpthread

   ## Execution
        make run qui correspond a :
            ./serveur 50000
    
   ## Suppression de l'executable
        make clean qui correspond a :
            rm serveur


# Coté client
   ## Compilation
        make ou make build qui correspond a :
            gcc client_udp.c -o client -lSDL2

   ## Execution
        make run qui correspond a :
            ./client 127.0.0.1 50000

   ## Suppression de l'executable
        make clean qui correspond a :
            rm client
