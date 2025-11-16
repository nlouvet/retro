; https://www.triceraprog.fr/des-outils-pour-programmer-en-assembleur-sur-vg5000m.html
    ORG $7000         ; Spécification de l'adresse mémoire d'implentation

    PUSH AF           ; Sauvegarde des registres sur la pile
    PUSH BC
    PUSH DE
    PUSH HL

                      ; Le cœur du programme :
    LD HL, chaine     ; Chargement de l'adresse du texte dans le registre HL
    CALL $36AA        ; Puis appel de la routine d'affichage de chaîne de caractères.

    POP HL            ; Restauration des registres depuis la pile
    POP DE
    POP BC
    POP AF

    RET               ; Retour au programme appelant
chaine:
    DEFB "Hello world!", 0
