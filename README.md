# TarInstall

Install applications from Tar Archive on Linux, with Desktop Image

# Installation

Clone the repository.
do make into the repository folder with superuser permissions

You can now use tarinstall from everywhere

# Full Example

```bash
challenger@debian:~$ tarinstall
bash: /usr/bin/tarinstall: Aucun fichier ou dossier de ce type

challenger@debian:~$ git clone https://github.com/shChallenger/TarInstall
Clonage dans 'TarInstall'...
remote: Enumerating objects: 8, done.
remote: Counting objects: 100% (8/8), done.
remote: Compressing objects: 100% (8/8), done.
remote: Total 8 (delta 1), reused 4 (delta 0), pack-reused 0 (from 0)
Réception d'objets: 100% (8/8), fait.
Résolution des deltas: 100% (1/1), fait.

challenger@debian:~$ cd TarInstall/

challenger@debian:~/TarInstall$ sudo make
[sudo] Mot de passe de challenger : 
gcc -c -O2 -Wall -Wextra -Wpedantic -Werror tarinstall.c -o tarinstall.o
gcc tarinstall.o -o tarinstall -larchive
ln -fs "/home/challenger/TarInstall/tarinstall" /usr/bin/tarinstall

challenger@debian:~/TarInstall$ cd ..

challenger@debian:~$ sudo tarinstall
tarinstall: missing path

challenger@debian:~$ wget https://download.sublimetext.com/sublime_text_3_build_3211_x64.tar.bz2
--2024-09-28 02:30:45--  https://download.sublimetext.com/sublime_text_3_build_3211_x64.tar.bz2
Résolution de download.sublimetext.com (download.sublimetext.com)… 104.236.0.104
Connexion à download.sublimetext.com (download.sublimetext.com)|104.236.0.104|:443… connecté.
requête HTTP transmise, en attente de la réponse… 200 OK
Taille : 13677324 (13M) [application/octet-stream]
Sauvegarde en : « sublime_text_3_build_3211_x64.tar.bz2 »

sublime_text_3_build_3211_x64. 100%[====================================================>]  13,04M  16,1MB/s    ds 0,8s    

2024-09-28 02:30:46 (16,1 MB/s) — « sublime_text_3_build_3211_x64.tar.bz2 » sauvegardé [13677324/13677324]

challenger@debian:~$ sudo tarinstall sublime_text_3_build_3211_x64.tar.bz2 
Extracting app : sublime_text_3
Found app bin : /usr/share/sublime_text_3/sublime_text
Found app icon : /usr/share/sublime_text_3/Icon/16x16/sublime-text.png
Found app icon : /usr/share/sublime_text_3/Icon/32x32/sublime-text.png
Found app icon : /usr/share/sublime_text_3/Icon/128x128/sublime-text.png
Found app icon : /usr/share/sublime_text_3/Icon/256x256/sublime-text.png
Creating Desktop File : /usr/share/applications/sublime_text.desktop
