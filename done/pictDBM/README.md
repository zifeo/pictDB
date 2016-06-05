#Haystack

## Utilisation

Le `makefile` contient deux executables relatifs à ce projet, une version command-line et une version serveur d'Haystack.

### Version command line
Pour utiliser celle-ci, une fois compilée, différentes commandes s'offrent à vous :

`pictDBM [COMMAND] [ARGUMENTS]`<br>
- `help`: displays this help.<br>
- `list <dbfilename>`: list pictDB content.<br>
- `create <dbfilename>`: create a new pictDB.<br>
    options are:<br>
  - `max_files <MAX_FILES>`: maximum number of files.<br>
                                  default value is *10*<br>
                                  maximum value is *100000*<br>
  - `thumb_res <X_RES> <Y_RES>`: resolution for thumbnail images.<br>
                                  default value is *64x64*<br>
                                  maximum value is *128x128*<br>
  - `small_res <X_RES> <Y_RES>`: resolution for small images.<br>
                                  default value is *256x256*<br>
                                  maximum value is *512x512*<br>
- `read <dbfilename> <pictID> [original|orig|thumbnail|thumb|small]`: read an image from the pictDB and save it to a file, default resolution is "original".<br>
- `insert <dbfilename> <pictID> <filename>`: insert a new image in the pictDB.<br>
- `delete <dbfilename> <pictID>`: delete picture pictID from pictDB.<br>
- `gc <dbfilename> <tmp dbfilename>`: performs garbage collecting on pictDB. Requires a temporary filename for copying the pictDB.<br>
- `interpretor <dbfilename>`: run an interpretor to perform above operations on a pictDB file.<br>

### Version serveur
Pour lancer le serveur, une fois celui-ci compilé, il suffit d'exécuter :
`pictDB_server db_file` depuis le terminal. Pour accéder à celui-ci, il suffit de se connecter à un navigateur et d'accéder à `http://IP_server:8000`, typiquement sur votre machine, cela devient `http://localhost:8000`.<br>
Notez cependant que nous utilisons une librairie externe qui requiert d'être liée à l'excutable, c'est pourquoi un script se charge de ceci. Pour l'utiliser, il suffit de taper `server.sh db_file`.<br>
Notez que cette version n'accepte aucun argument supplémentaire.

## Ressources

Ce projet utilise différentes librairies, à savoir :

- **VIPS** pour la gestion d'images (redimensionnement etc)
- **LibSSL** pour la génération d'un SHA en fonction d'une chaine de caractère donnée
- **libmongoose** pour les connections réseaux
- **json-c** pour l'envoi d'informations au format JSON

L'interface graphique du server utilise du style provenant de **Semantic UI**.