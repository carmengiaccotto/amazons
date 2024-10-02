# Sujet de programmation impérative

La page du sujet :

https://www.labri.fr/perso/renault/working/teaching/projets/2022-23-S6-C-Amazons.php

La page sur thor :

https://thor.enseirb-matmeca.fr/ruby/projects/projetss6-amaz/overview


This project contains a server and client implementation of a game. The game includes two players, each with their own shared library (libplayer.so and libplayer2.so). The server controls the game, while the client connects to the server and requests to play the game.
Prerequisites

To build and run this project, you will need the following dependencies:

    The GNU Scientific Library (GSL) version 2.6 or higher
    GCC compiler with support for C99 standard

# Building the Project

To build the project, make sure the GSL_PATH variable in the Makefile points to the installation path of the GSL library on your system. Then run the following command in the project directory:

"make"

This will build the server, client, shared libraries and test executable.
Running the Project

To start the server, run the following command in the project directory:

"./server ./libplayer.so ./libplayer2.so"

# Testing the Project

To run the unit tests, run the following command in the project directory:

"make test"


# Cleaning the Project

To clean the project, run the following command in the project directory:

"make clean"

This will remove all object files and executables.

To completely remove the project, including object files, executables and coverage data, run the following command in the project directory:


"make cleanall"

# Liste des développeurs

- C. Giaccotto
- F. Roters

