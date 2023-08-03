/*
Ce code permet de récupérer le flow de data envoyé toutes les secondes par la passerelle Envoy S du fabricant Enphase 
de micro onduleur photovoltaïque. Je l'ai adapté à la version de firmware de l'Envoy déployé depuis peu par Enphase.
J'ai repris le principe de l'authentification JWT que Kristof @TofMassilia13320 a publié récemment sur son fork du 
Pv router xlyric/pv-router-esp32. 
Pour comprendre le fonctionnement, j’ai utilisé un flow de Bison sous NodeRed : 
https://community.jeedom.com/t/tuto-enphase-metered-node-red-jmqtt/87453
La récupération du Token sur l'ESP32 a été portée et partagée nathan.wailly du forum Jeedom
Remplacer les ************* par vos données.
Cela vous permez de tester le bon fonctionnement avant de pousser plus loin l'intégration dans un projet.
*/
