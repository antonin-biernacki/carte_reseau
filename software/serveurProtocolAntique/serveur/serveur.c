#include "serveur.h"

char fichier_write[MAX_UDP_MESSAGE];
uint8_t numero_write = 0x00;


int initialisationSocketUDP(char* service) {
    struct addrinfo precisions, *resultat, *origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions, 0, sizeof precisions);
    // precisions.ai_family = AF_UNSPEC;
    precisions.ai_family = AF_INET;
    precisions.ai_socktype = SOCK_DGRAM;
    precisions.ai_flags = AI_PASSIVE;
    statut = getaddrinfo(NULL, service, &precisions, &origine);
    if(statut < 0) { 
        perror("initialisationSocketUDP.getaddrinfo");
        exit(EXIT_FAILURE); 
    }
    struct addrinfo* p;
    for(p=origine, resultat=origine ; p!=NULL ; p=p->ai_next)
    if(p->ai_family == AF_INET6) {
        resultat = p;
        break;
    }

    /* Creation d'une socket */
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if(s < 0) {
        perror("initialisationSocketUDP.socket");
        exit(EXIT_FAILURE);
    }

    /* Options utiles */
    int vrai = 1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &vrai, sizeof(vrai)) < 0) {
        perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)");
        exit(EXIT_FAILURE);
    }

    /* Specification de l'adresse de la socket */
    statut = bind(s, resultat->ai_addr, resultat->ai_addrlen);
    if(statut < 0) {
        freeaddrinfo(origine);
        shutdown(s, SHUT_RDWR);
        perror("initialisationServeurUDP.bind");
        return -1;
    }

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    return s;
}





int boucleServeurUDP(int s, int (*traitement)(unsigned char* , int/*, struct sockaddr* , int*/)) {
    while(1) {
        struct sockaddr_storage adresse;
        socklen_t taille = sizeof(adresse);
        unsigned char message[MAX_UDP_MESSAGE];
        int nboctets = recvfrom(s, message, MAX_UDP_MESSAGE, 0, (struct sockaddr *)&adresse, &taille);
        if(nboctets < 0) return -1;
        if(traitement(message, nboctets/*, (struct sockaddr *)&adresse, taille*/) < 0) break;
    }
    return 0;
}





int traitementMessageUDP (unsigned char* message, int nboctets/*, struct sockaddr* adresse, int taille*/) {
    uint8_t operation = message[OPERATION];
    uint8_t longueur = message[LONGUEUR];

    if ((nboctets != (longueur + LONGUEUR_ENTETE)) || (longueur > (MAX_UDP_MESSAGE - LONGUEUR_ENTETE))) {
        // Probleme dans la transmission
        printf("nboctets : %d\nlongueur : %d\n", nboctets, longueur);
        printf("Erreur message mal formé ou mal transmit\n");

    } else if (operation == REQUETE_LS) {
        // Requete LS reçue
        traitement_ls(message);

    } else if (operation == REQUETE_READ) {
        // Requete READ reçue
        traitement_read(message);

    } else if (operation == REQUETE_WRITE || operation == DONNEES_WRITE) {
        // Requete WRITE reçue
        traitement_write(message);

    } else {
        // Requete non reconnue
        printf("Message non reconnu \n\n");
    }
    return 0;
}





void traitement_ls (unsigned char* message) {
    int longueur = message[LONGUEUR];
    DIR* dossier;
    if (longueur == 0x00) {
        // LS sans chemin par default .
        printf(">> ls .\n");
        dossier = opendir(".");

    } else {
        // LS avec chemin
        unsigned char chemin[longueur+1];
        for (int i=0 ; i<longueur ; i++) {
            chemin[i] = message[i+3]; // Recuperation du chemin
        }
        chemin[longueur] = '\0';
        printf(">> ls %s\n", chemin);
        dossier = opendir((const char*)chemin);
    }

    if (dossier == NULL) {
        printf("    impossible d'ouvrir le dossier\n\n");
    } else {

        struct dirent* entree;
        char reponse[MAX_UDP_MESSAGE];
        char contenu[MAX_UDP_MESSAGE - LONGUEUR_ENTETE];
        char num = 0x00;
        while ((entree = readdir(dossier)) != NULL) {
            // Message UDP pour chaque entree
            strcpy(contenu, entree->d_name);
            printf("    %s  ", contenu);
            reponse[OPERATION] = REPONSE_LS;
            reponse[LONGUEUR] = strlen(contenu);
            reponse[NUMERO] = num++;
            for (int i=0 ; i<reponse[LONGUEUR] ; i++) {
                reponse[i+3] = contenu[i];
            }
            messageUDP(IP_LOOPBACK, PORT_CARTE, (unsigned char *)reponse, (int)reponse[LONGUEUR] + LONGUEUR_ENTETE);
        }
        // Message UDP pour signaler la fin de la reponse
        strcpy(contenu, "");
        reponse[OPERATION] = REPONSE_LS;
        reponse[LONGUEUR] = 0x00;
        reponse[NUMERO] = 0x00;
        messageUDP(IP_LOOPBACK, PORT_CARTE, (unsigned char *)reponse, (int)reponse[LONGUEUR] + LONGUEUR_ENTETE);
        printf("\n\n");
        closedir(dossier);
    }
}





void traitement_read (unsigned char* message) {
    FILE* fichier;
    int longueur = message[LONGUEUR];
    char chemin[longueur+1];
    for (int i=0 ; i<longueur ; i++) {
        chemin[i] = message[i+3]; // Recuperation du chemin ou read
    }
    chemin[longueur] = '\0';

    printf(">> read %s\n", chemin);

    fichier = fopen(chemin, "r");
    if (!fichier) {

        printf("    impossible d'ouvrir le fichier\n\n");

    } else {
        
        char reponse[MAX_UDP_MESSAGE];
        char ligne[MAX_UDP_MESSAGE - LONGUEUR_ENTETE];
        char num = 0x00;
        while (fgets(ligne, sizeof(ligne), fichier)) {
            // Message UDP pour chaque ligne
            printf("    %d | %s", (int)num, ligne);
            reponse[OPERATION] = REPONSE_READ;
            reponse[LONGUEUR] = strlen(ligne);
            reponse[NUMERO] = num++;
            for (int i=0 ; i<reponse[LONGUEUR] ; i++) {
                reponse[i+3] = ligne[i];
            }
            messageUDP(IP_LOOPBACK, PORT_CARTE, (unsigned char *)reponse, (int)reponse[LONGUEUR] + LONGUEUR_ENTETE);
        }
        fclose(fichier);
        // Message UDP pour signaler la fin de la reponse
        strcpy(ligne, "");
        reponse[OPERATION] = REPONSE_READ;
        reponse[LONGUEUR] = 0x00;
        reponse[NUMERO] = 0x00;
        messageUDP(IP_LOOPBACK, PORT_CARTE, (unsigned char *)reponse, (int)reponse[LONGUEUR] + LONGUEUR_ENTETE);

        printf("\n");
    }
}





void traitement_write (unsigned char* message) {
    int operation = message[OPERATION];
    int longueur = message[LONGUEUR];
    int numero = message[NUMERO];
    char contenu[longueur+1];
    for (int i=0 ; i<longueur ; i++) {
        contenu[i] = message[i+3]; // Recuperation du contenu
    }
    contenu[longueur] = '\0';
    
    if (operation == REQUETE_WRITE && numero_write == 0x00) {
        // Debut du write on enregistre le contenu qui represente le chemin
        printf(">> write %s\n", contenu);
        strcpy(fichier_write, contenu);

    } else if (operation == DONNEES_WRITE && numero == (numero_write + 1)) { // Verif si aucun paquet n'a été perdu
        // Ecriture de contenu dans le fichier, contenu represente une ligne
        FILE* fichier;
        fichier = fopen(fichier_write, "a");
        if (!fichier) {
            printf("    impossible d'ouvrir le fichier\n\n");
        } else if (fwrite(contenu, sizeof(unsigned char), strlen(contenu), fichier) != strlen(contenu)) {
            printf("    erreur lors de l'ecriture de la ligne\n\n");
        } else {
            numero_write = numero;
            fwrite("\n", 1, 1, fichier);
            printf("    ligne %d ecrite\n", numero);
        }
        fclose(fichier);

    } else if (operation == DONNEES_WRITE && numero == 0x00) {
        // Message de fin de transmission arrive
        numero_write = 0x00;
        printf("    fin de l'ecriture\n\n");
        // Message UDP pour signaler que le write s'est bien passé
        char reponse[MAX_UDP_MESSAGE];
        reponse[OPERATION] = REPONSE_WRITE;
        reponse[LONGUEUR] = strlen(contenu);
        reponse[NUMERO] = 0x00;
        reponse[3] = 0x00; // write terminé correctement
        messageUDP(IP_LOOPBACK, PORT_CARTE, (unsigned char *)reponse, (int)reponse[LONGUEUR] + LONGUEUR_ENTETE);

    } else {
        // Un message a été perdu
        numero_write = 0x00;
        printf("    erreur lors de la transmission\n\n");
        // Message UDP pour signaler que le write s'est mal passé
        char reponse[MAX_UDP_MESSAGE];
        reponse[OPERATION] = REPONSE_WRITE;
        reponse[LONGUEUR] = strlen(contenu);
        reponse[NUMERO] = 0x00;
        reponse[3] = 0x01; // probleme lors du write
        messageUDP(IP_LOOPBACK, PORT_CARTE, (unsigned char *)reponse, (int)reponse[LONGUEUR] + LONGUEUR_ENTETE);
    }
}





void messageUDP(char* hote, char* service, unsigned char* message, int taille) {
    struct addrinfo precisions, *resultat, *origine;
    int statut;
    int s;

    /* Creation de l'adresse de socket */
    memset(&precisions, 0, sizeof precisions);
    // precisions.ai_family = AF_UNSPEC;
    precisions.ai_family = AF_INET;
    precisions.ai_socktype = SOCK_DGRAM;
    statut = getaddrinfo(hote, service, &precisions, &origine);
    if(statut < 0) { 
        perror("messageUDPgenerique.getaddrinfo");
        exit(EXIT_FAILURE);
    }
    struct addrinfo* p;
    for(p=origine, resultat=origine ; p!=NULL ; p=p->ai_next)
    if(p->ai_family == AF_INET6) { 
        resultat = p;
        break;
    }

    /* Creation d'une socket */
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if(s < 0) {
        perror("messageUDPgenerique.socket");
        exit(EXIT_FAILURE);
    }

    /* Option sur la socket */
    int vrai = 1;
    if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &vrai, sizeof(vrai)) < 0) {
        perror("initialisationServeurUDPgenerique.setsockopt (BROADCAST)");
        exit(EXIT_FAILURE);
    }

    /* Envoi du message */
    int nboctets = sendto(s, message, taille, 0, resultat->ai_addr, resultat->ai_addrlen);
    if(nboctets < 0) { 
        perror("messageUDPgenerique.sento");
        exit(EXIT_FAILURE);
    }

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    /* Fermeture de la socket d'envoi */
    shutdown(s, SHUT_RDWR);
}





int main(void) {
    int s = initialisationSocketUDP(PORT_SERVEUR);
    if(s < 0) { 
        fprintf(stderr, "Port non utilisable\n");
        exit(EXIT_FAILURE);
    }
    boucleServeurUDP(s, traitementMessageUDP);
    shutdown(s, SHUT_RDWR);
    return 0;
}