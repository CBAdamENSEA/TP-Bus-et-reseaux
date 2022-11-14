# TP Bus et Réseaux



## Déscription

Ce TP consiste à réaliser un système permettant l'accés aux données des capteurs et le pilotage d'actionneur à travers une interface API Rest.
L'architecture du système:
![architecture](https://github.com/CBAdamENSEA/TP-Bus-et-reseaux/blob/master/media/System_architecture.PNG)
L'interrogation des capteurs se fait par le bus I²2C avec la STM32.
Le pilotage d'actionneur se fait par le bus CAN avec la STM32.
La Raspberry Pi récupère les données de la STM32 à travers le bus UART.
L'interface API Rest est implémentée sur la Raspberry Pi.

## Getting Started

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

Contributors names and contact info

ex. Dominique Pizzie  
ex. [@DomPizzie](https://twitter.com/dompizzie)

## Version History

* 0.2
    * Various bug fixes and optimizations
    * See [commit change]() or See [release history]()
* 0.1
    * Initial Release

## License

This project is licensed under the [NAME HERE] License - see the LICENSE.md file for details

## Acknowledgments

Inspiration, code snippets, etc.
* [awesome-readme](https://github.com/matiassingers/awesome-readme)
* [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
* [dbader](https://github.com/dbader/readme-template)
* [zenorocha](https://gist.github.com/zenorocha/4526327)
* [fvcproductions](https://gist.github.com/fvcproductions/1bfc2d4aecb01a834b46)