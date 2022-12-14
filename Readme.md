# TP Bus et Réseaux





## Déscription

Ce TP consiste à réaliser un système permettant l'accés aux données des capteurs et le pilotage d'actionneur à travers une interface API Rest.

L'architecture du système :

![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/System_architecture.PNG)

L'interrogation des capteurs se fait par le bus I²C avec la STM32.

Le pilotage d'actionneur se fait par le bus CAN avec la STM32.

La Raspberry Pi récupère les données de la STM32 à travers le bus UART.

L'interface API Rest est implémentée sur la Raspberry Pi.

## TP1: Bus I2C

### Objectif

Interfacer un STM32 avec des un capteur I2C (BMP280)

### Capteur BMP280
 
Le BMP280 est un capteur de pression et température développé par Bosch qui utilise un protocole de communication de type I²C
Il dispose de differents registres qui peuvent etre modifiés ou lus afin de bien configurer le capteur , les registres utilisés sont les suivants:
![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/mapping.png)

Tout d'abord, on commence par l'identification du composant (ID = 0x58): 

L'identification du composant se fait grace a la fonction`BMP_Verify_Id()`
cette fonction nous permet de lire la valeur du registre ID se trouvant a l'adresse 0xD0 ,la valeur lue doit correspondre à 0x58 qui est l'adresse du composant.

Ensuite on configure le capteur: mode normal, Pressure oversampling x16, Temperature oversampling x2: 

Pour cela nous utilisons la fonction `BMP_Config(bmp)` qui modifiera le registre `ctrl_meas` qui se trouve a l'adresse 0xF4, les deux bits LSB nous permettent de choisir le mode , les trois bits suivant nous permettent de choisir "l'oversampling pression" et les trois bits de poid fort nous permettent de choisir "l'oversampling température"

Après, nous effectuons le calibrage du composant:
 
La calibration se fait grace a la fonction `Calibration(calibration_data)`, cette fonction récupère les données stockées dans les registres calib00 à calib25, elle les stocke dans differents tableaux , ces données seront par la suite utilisées pour effectuer la compensation.  

Nous pouvons a présent lire la valeur de la température et de la pression qui se trouvent dans les six registres suivants: 
* temp_msb ,temp_lsb, temp_xlsb	pour la température
* press_msb ,press_lsb, press_xlsb	pour la pression 

Nous remarquons que les valeurs que nous lisons sont des valeurs en hexadécimal de l'ordre de 0x80000 ,cela correspond a une valeur de température d'environ 25°C , cependant une derniere étape consiste a utiliser et adapter l'algorithme présent dans la documentation afin d'avoir une valeur en degrés Celsius.


Pour cela, on écrit 2 fonctions distinctes:
* Une fonction `Read_Temp()` qui renvoie la valeur de la température compensée en 100°C
* Une fonction `Read_Press()` qui renvoie la valeur de la pression compensée en Pa

On affiche ces valeurs sur l'ordinateur en rederigeant la fonction printf vers l'UART1 de la STM32.

## TP2: Interfaçage STM32 - Raspberry

### Objectif

Permettre l'interrogation du STM32 via un Raspberry Pi Zero

### Mise en route du Raspberry PI Zéro

Tout d'abord, il faut commencer par télecharger l'image  "Raspberry Pi OS (32-bit) Lite" et l'installer sur la carte SD en utilisant BalenaEtcher. 
Par la suite nous devons configurer l'image, cela se fait en créant des fichiers ssh et wpa_supplicant.conf dans la partition boot afin de lancer 
automatiquement le serveur SSH sur le réseau et pour activer le port série afin d'assurer la connexion avec la STM32.  
le fichier wpa_supplicant.conf contient le code suivant:
```
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=FR

network={
 ssid="<Name of your wireless LAN>"
 psk="<Password for your wireless LAN>"
}   
```
par la suite il faut activer le port serie en modifiant le fichier config.txt de la partition boot , il faut rajouter le code suivant:  
```
enable_uart=1
dtoverlay=disable-bt
```
la prochaine étape consiste à liberer le port UART en retirant l'option suivante dans le fichier cmdline.txt :  

`console=serial0,115200`

La derniere étape consiste à modifier les fichier  /etc/hostname et /etc/hosts dans la partition root afin de modifier le nom de la Raspberry. 
 
Une fois la configuration terminer, on peut commencer à écrire notre protocole de communication avec la STM32.   

Pour cela on doit tous d'abord connecter la STM32 avec la Raspberry, il faut biensur croiser les fils RX et TX , et surtout ne pas oublier de relier le GND de la raspberry avec le GND de la STM32, cela peut nous faire perdre énormement de temps.  
à ce stade du TP nous nous contentons juste d'afficher les valeurs des température et de la pression en ecrivant les commandes "GET_T" et "GET_P" sur minicom, on fera la meme chose pour les autres commandes tel que "GET_K" , "SET_K", "GET_A".  

Maintenant que le protocole de communication fonctionne avec minicom, il faut faire la meme chose mais cette fois ci en utilisant Python.


### Commande depuis Python

Il faut d'abord installer pip pour python3 avec la commande `sudo apt install python3-pip` puis installer le pack pyserial 
avec la commande `pip3 install pyserial`

Nous avons rencontré un probleme en ecrivant le code python, lorsqu'on utilisait minicom , on recevait via l'uart un caractere, un traitement etait effectué puis on recevait un autre caractere, l'etre humain ayant une vitesse d'ecrtire relativement faible,
cela fonctionnait bien , cependant , lorsqu'on envoie en utilisant ser.write la vitesse de transmission est élevée,sachant que le traitement de chacuns des caracteres prend un certain temps, il arrive parfois de rater un caractere ou deux , il a donc fallu trouver une solution qui est d'utiliser 
la fonction `HAL_UARTEx_ReceiveToIdle_DMA(&huart3, RxBuffer, RX_BUFFER_SIZE)` qui attend de tous recevoir avant d'effectuer le traitement.  

Le probleme étant resolu, nous avons ecris le protocole de communication suivant:  

![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/protocole.PNG)

Ensuite, Nous avons écrit un code python permettant la récupération des données de la STM32, ainsi a la fin de ce TP nous avons pu afficher la température sur la raspberry en effectuant une simple demande via le programme python.    


## TP3: Interface REST

### Objectif

Développement d'une interface REST sur le Raspberry

### Installation du serveur Python

Il faut d'abord créer son propre utilisateur avec les droits de sudo et  d'accès au port série (dialout)

`sudo adduser XXX`

`sudo usermod -aG sudo XXX`

`sudo usermod -aG dialout XXX`

Il faut ensuite installer le pack flask avec la commande `pip3 install flask` qui va nous permettre de créer notre page REST

### Première page REST avec métodes HTTP

Nous avons créé notre serveur web et nous l'avons lancé avec la commande `FLASK_APP=nom_du_programme.py FLASK_ENV=development flask run --host 0.0.0.0`   
`--host 0.0.0.0` Afin d'eviter que notre serveur ne fonctionne qu'en loopback 

La constante `FLASK_ENV=development` permet de lancer en mode debug pour permettre de tester le serveur web.	

On choisit le format JSON pour les données car il est lisable par un homme ou une machine.

Nous avons ajouté une page d'erreur 404 (html) avec la fonction `render_template('page_not_found.html')`.  
pour cela il faut telecharger le fichiers `page_not_found.html` et le placer dans un repertoire template 
par la suite ajouter les lignes de code suivantes a notre `app.py`:  
```
@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404
	
```  

	

Ensuite, nous avons ajouté les méthodes POST, GET, PUT, PATCH et DELETE pour définir les fonctions CRUD.Les fonctions ajoutées sont les suivante :  

![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/crud_methode.png)

finalement, nous les avons testé grace a l'extension rested sur firefox. dans laquelle il faut donner l'adresse ip de la raspberry, mais aussi le chemin comme on peut le voir sur l'exemple suivant:   
![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/rested.png)

Le resultats d'une telle requete doit correspondre a la lettre L car nous avons choisi l'index 3 qui correspond a la troisieme lettre du message que nous retournant ("welcome")  



## TP4: Bus CAN 

### Objectif

Mise en place d'un moteur pas-à-pas sur bus CAN avec un baud rate de 500kbits/s. Le ratio seg2/(seg1+seg2) détermine 
l'instant de décision. Il doit être aux alentours de 87%.il faut bien le choisir car cela peut donner lieu a des erreurs de transmissions , Nous avons choisi un prescaler de 21 et seg1 de 2 times et seg2 de 1 time

Pour pouvoir utiliser le bus CAN de la STM32, il faut passer par un Tranceiver CAN. Ce composant a été installé sur une carte 
fille (shield) au format Arduino pour pouvoir le connecter facilement. Ce shield possède un connecteur subd9, qui permet de 
connecter un câble au format CAN avec le moteur.
le RX et TX Du Tranceiver sont connectés a PB8 et PB9 de la carte nucléo, il faut donc penser à les changer pour la communication entre la STM32 et la raspberry.  

Pour activer le module CAN, nous allons utiliser la fonction `HAL_CAN_Start(&hcan1)`

Ensuite pour donner un message: nous allons utiliser la fonction `HAL_CAN_AddTxMessage( &hcan1, &pHeader,aData,&pTxMailbox)`

Le moteur fonctionne en 2 modes: automatique et manuel: 

![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/moteur.PNG)

Donc, pour que le moteur tourne à un angle défini, nous allons utilisé le mode automatique en modifiant le `pHeader` pour choisir 
l'ID d'arbitration et nous allons choisir l'angle et le signe en modifiant le aData, ainsi pour tourner d'un angle de 90° dans le sens positif:  
* aData[0] doit valoir 90. 
* aData[1] doit valoir 0.


### Pilotage du moteur en fonction de la température
le but de cette partie est de faire tourner le moteur en fonction de l'evolution de la température , le coefficient k determine l'angle de rotation pour chaque degrés celsius.   

Pour cela nous avons d'abord configuré le bus CAN , par la suite nous avons recuperé la temperature grace au capteur BMP280 en utilisant un timer qui genere une intéruption chaque dixieme de seconde,la temperature est donc recolter chaque dixieme de seconde.
Nous savons que la temperature n'evolue pas de maniere significative dans la salle, elle peut varier entre 20 et 30°C, si l'on pose 1°C= 1° de rotation , on ne verra pas le moteur tourner, 
on a donc ajouté un coefiscient de multiplication K et un offset de 25°C pour avoir la position 0° du moteur à 25°C , le coefficient peut etre modifier par la suite en utilisant l'interface REST, ainsi pour K=10 chaque evolution d'un degrés correspond à
un angle de rotation de 10 degrés. Nous avons verifier cela de maniere experimentale grace au graduations du moteur pas-à-pas.


## TP5: Intégration I²C - Serial - REST - CAN

### Objectif
l'objectif de ce TP est d'assembler le travail effectué lors des quatre TP précedent et de faire fonctionner le tous.  
A ce stade, la raspberry interroge la STM32 qui lui fournie les données du capteurs, En conséquence la STM32 pilote le moteur pas-à-pas proportionnellement
à la temperature, il faut à present implementer les fonctions suivantes sur l'API REST:  
![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/crud.png)
afin de realiser cela nous nous sommes aidé du TP3, tous d'abord nous avons créé l'url de Temperature grace a la fonction `@app.route('/temp/', methods=['GET', 'POST'])`
dans laquelle nous avons aussi définie les deux methode possible pour cette derniere `GET` et `POST`, puis par la suite nous avons défini les deux methodes dans `def api_temp():`
l'objet retourné lorsqu'on fait appel à la methode `GET` est un json dans lequel on trouve la température, l'unité ainsi que le timestamp, nous stockons par la suite 
cet objet json dans une liste de json pour avoir à terme une liste de toutes les températures relevées à chaque `GET` temperature , nous faisons la meme chose pour la pression. 
Concernant le coefficient K, nous avons proceder de la meme maniere en utilisant `@app.route('/scale/<int:K>', methods=['POST'])` mais dans ce cas la seule methode possible est un `POST`
qui permet de modifier la valeur du coefficient K.  

pour vérifier le bon fonctionnement de l'ensemble, il suffit d'utiliser l'extension REST de Firefox dans laquelle on effectue dans un premier temps un GET de la temperature et de la pression.  
on peut aussi changer la valeur du coefficient K en utilisant la méthode `POST` et ainsi nous pouvons voir que suivant la valeur de K le moteur tourne plus ou moin rapidement. 

En conclusion, ce TP a été trés formateur pour les etudiants, nous avons pu mettre en oeuvre differents protocole de communications à savoir: I²C et CAN, mais nous avons aussi 
pu voir l'aspect réseaux et REST API.  































## Auteurs

[Adam CHEIKH BRAHIM](mailto:adam.cheikh-brahim@ensea.fr?subject=[GitHub]%20TP%20Bus%20Reaseaux)

[Karim ABDELLAZIZ](mailto:karim.abdellaziz@ensea.fr?subject=[GitHub]%20TP%20Bus%20Reaseaux)


