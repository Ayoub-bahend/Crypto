# On purge la liste des suffixes utilisés pour les rôles implicites
.SUFFIXES:

# On ajoute simplement les extensions dont l'on a besoin
.SUFFIXES: .cpp .o

# Nom de l'exécutable
EXEC = tp4

# Liste des fichiers sources séparés par des espaces
SOURCES = main.cpp rsa.cpp

# Liste des fichiers objets
OBJETS = $(SOURCES:%.cpp=%.o)

# Compilateur et options de compilation
CCPP = g++
CFLAGS = -W -Wall -Wextra -pedantic -std=c++0x -I /usr/X11R6/include
LFLAGS = -L . -L /usr/X11R6/lib -lpthread -lX11 -lXext -Dcimg_use_xshm -lm -lgmp -lssl -lcrypto

# Rôle explicite de construction de l'exécutable
$(EXEC): $(OBJETS) Makefile
	$(CCPP) -o $(EXEC) $(OBJETS) $(LFLAGS)

.cpp.o:
	$(CCPP) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJETS)

clear:
	rm -f $(EXEC)

depend:
	sed -e "/^#DEPENDANCIES/,$$ d" Makefile > dependances
	echo "#DEPENDANCIES" >> dependances
	$(CCPP) -MM $(SOURCES) >> dependances
	cat dependances > Makefile
	rm dependances

# Ajout des dépendances générées automatiquement
#DEPENDANCIES
main.o: main.cpp rsa.hpp
rsa.o: rsa.cpp rsa.hpp