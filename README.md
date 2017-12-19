# PCSS_Miniproject

This miniproject is part of the Programming of Complex Systems course in 3rd Semester Medialogy study at Aalborg University, Denmark. It implements a simple multichat application consisting of a client and a server programs. The application allows for up to 5 clients to be connected in the same time. When connecting to the server, the client should specify a nickname which is to be used in the chat.

## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites
The application is developed for the Windows operating system and makes use of the WinSock API. The project is made in Visual Studio. The Target Platform Version for the project is set to 10.0.16299.0, however, it also works on the 8.1 platform. The platform toolset used is v140 (Visual Studio 2015).

```
Windows OS
Visual Studio
```
### Running the application
In order to run the application, it is enough to clone or download the repository on your computer, open the project in Visual Studio and compile the server and client programs.
* The server program does not take any command-line arguments, it is enough to just run it after it is compiled.
* The client program takes two command-line arguments:
  * The first one is the IP address of the server program;
  * The second one is the nickname of the client.
  
Hence, the client program can be run through Command Prompt as follows:
```
winsockMultichatClient.exe-DIR>winsockMultichatClient server-ip-address client-nickname
```
## Authors
* **Trendafil Gechev** - [tgechev](https://github.com/tgechev)
* **Marlene Lomborg** - [mlombo16](https://github.com/mlombo16)
* **Alexandra Bartas** - [Ale18](https://github.com/Ale18)
* **Benjamin Singer** - [Trephneor](https://github.com/Trephneor)
* **Chatrine Larsen** - [Chatrine](https://github.com/Chatrine)
* **Konrad Matynia** - [KonrolMathisen](https://github.com/KonrolMathisen)
