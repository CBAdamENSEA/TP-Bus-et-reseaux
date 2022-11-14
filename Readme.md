# TP Bus et Réseaux





## Déscription

Ce TP consiste à réaliser un système permettant l'accés aux données des capteurs et le pilotage d'actionneur à travers une interface API Rest.

L'architecture du système:

![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/System_architecture.PNG)

L'interrogation des capteurs se fait par le bus I²2C avec la STM32.

Le pilotage d'actionneur se fait par le bus CAN avec la STM32.

La Raspberry Pi récupère les données de la STM32 à travers le bus UART.

L'interface API Rest est implémentée sur la Raspberry Pi.

## TP1: Bus I2C

### Objectif

Interfacer un STM32 avec des un capteur I2C (BMP280)

### Capteur BMP280

Le BMP280 est un capteur de pression et température développé par Bosch

Tout d'abord, on commence par l'identification du composant (ID = 0x58): ('BMP_Verify_Id()')

Ensuite on configure le capteur: mode normal, Pressure oversampling x16, Temperature oversampling x2: ('BMP_Config(bmp)')

Après, en effectue le calibrage du composant : ('Calibration(calibration_data)')

Enfin, on écrit 2 fonctions distinctes:
* Une fonction ('Read_Temp()') qui renvoie la valeur de la température compensée en 100°C
* Une fonction ('Read_Press()') qui renvoie la valeur de la pression compensée en Pa

On affiche ces valeurs sur l'ordinateur en rederigeant la fonction printf vers l'UART1 de la STM32.

## TP2: Interfaçage STM32 - Raspberry

### Objectif

Permettre l'interrogation du STM32 via un Raspberry Pi Zero

### Mise en route du Raspberry PI Zéro

Tout d'abord, il faut préparer la Raspberry PI Zéro en modifiant des fichiers dans lA partition bootafin de lancer 
automatiquement le serveur SSH sur le réseau et pour activer le port série afin d'assurer la connexion avec la STM32.

### Commande depuis Python

On a commencé par écrire un protocole de communication UART (uart3) sur la STM32 pour gérer les demandes de la Raspberry:

![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/protocole.PNG)

Ensuite, Nous avons écrit un code python permettant la récupération des données de la STM32.

## TP3: Interface REST

### Objectif

Développement d'une interface REST sur le Raspberry

###


























### Dependencies

* Describe any prerequisites, libraries, OS version, etc., needed before installing program.
* ex. Windows 10

### Installing

* How/where to download your program
* Any modifications needed to be made to files/folders

### Executing program

* How to run the program
* Step-by-step bullets
```
code blocks for commands
```

## Help

Any advise for common problems or issues.
```
command to run if program contains helper info
```

## Authors

Adam CHEIKH BRAHIM

Karim ABDELLAZIZ
