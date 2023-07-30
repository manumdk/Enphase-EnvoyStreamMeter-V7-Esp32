# Enphase-EnvoyStreamMeter-V7-Esp32
Ce code permet de récupérer le flow de data envoyé toutes les secondes par la passerelle Envoy S du fabricant Enphase de micro onduleur photovoltaïque.
Je l'ai adapté à la version de firmware de l'Envoy déployé depuis peu par Enphase.

J'ai repris le principe de l'authentification JWT que Kristof [@TofMassilia13320](https://github.com/TofMassilia13320/pv-router-esp32) a publié récemment sur son fork du Pv router xlyric/pv-router-esp32
Pour comprendre le fonctionnement, j’ai utilisé un flow sous NodeRed.
https://community.jeedom.com/t/tuto-enphase-metered-node-red-jmqtt/87453

Le token doit être récupéré en se connectant sur la passerelle Envoy depuis un PC et rentré dans le code.
Pour simplifier le débogage, le partage et les updates, les infos SSID, PWD, URL, et Token sont sauvegardés via la librairie Preferences.

Une évolution avec la biliothéque mobizt/Firebase ESP32 Client serait très certainement intéressant pour suivre les messages avec des events et non du pooling.
