:run.bat
:
:runs Remote Code Management solution


:FORMAT: start .\Debug\Peer.exe <Self IP of Peer> <Listening port of peer> <Destination IP> <Destination Port Number> <File to be sent>

start .\Debug\Peer.exe localhost 9070
start .\Debug\CppCli-WPF-App.exe localhost 9060
start .\Debug\Peer.exe localhost 9080
start .\Debug\CppCli-WPF-App.exe localhost 9050

