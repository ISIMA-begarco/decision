#! /bin/bash

############################## PARAMETRES ##############################

#Choix de l'os pour certaines regles (Windows / Unix)
OS = Unix

#Extention des fichiers
EXT = cpp
#Compilateur
CXX = g++-4.7
#Nom de l'executable (windows ajoutera automatiquement .exe)
EXEC = vrp

############################## OPTIONS ##############################

#Options de compilation
CXXFLAGS = -Wall -W -Wextra -O2 -std=c++11
#Options pour les librairies
LDFLAGS = 

#Repertoire des fichiers objet
OBJDIR = obj
#Sources et objets
SRC = $(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.o)
OBJ := $(addprefix $(OBJDIR)/, $(OBJ))

############################## REGLES ##############################

#Regles speciales
.PHONY: clean

all: $(EXEC)

#Regles de base
$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.$(EXT)
	@$(CXX) -o $@ -c $< $(CXXFLAGS)

#Clean : s'adapte a Windows ou Unix
clean:
ifeq ($(OS), Windows)
	del $(addsuffix .exe, $(EXEC))
	del $(OBJDIR)\*.o
else
	rm -f $(EXEC)
	rm -rf $(OBJDIR)/*.o
endif
