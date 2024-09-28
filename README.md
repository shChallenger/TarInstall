# TarInstall

Install applications from Tar Archive on Linux, creating Desktop Image

# Installation

Clone the repository.

Go into the repository folder and execute 'make' with sudo

You can now use tarinstall from everywhere

# Full Example to install Sublime Text 3

```bash
git clone https://github.com/shChallenger/TarInstall
&& cd TarInstall/
&& sudo make
&& cd ..
&& wget https://download.sublimetext.com/sublime_text_3_build_3211_x64.tar.bz2
&& sudo tarinstall sublime_text_3_build_3211_x64.tar.bz2 
