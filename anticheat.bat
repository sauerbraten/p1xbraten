@ECHO OFF

start bin64\start_protected_game.exe -e "-k%cd%" "-q$HOME\My Games\Sauerbraten" -glog.txt %*
