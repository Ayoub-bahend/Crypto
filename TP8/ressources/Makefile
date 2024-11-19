# On purge la liste des suffixes utilise pour les roles implicites
.SUFFIXES:

# On ajoute simplements les extensions dont l'on a besoin
.SUFFIXES: .cpp .o

# Nom de l'executable
EXEC=tea

# Liste des fichiers sources separes par des espaces
SOURCES=tea_starter.cpp

# Liste des fichiers objets
OBJETS=$(SOURCES:%.cpp=%.o)

# Compilateur et options de compilation
CCPP=g++
CFLAGS= -W -Wall -Wextra -pedantic -std=c++0x -I /usr/X11R6/include
LFLAGS= -L . -L /usr/X11R6/lib  -lpthread -lX11 -Dcimg_use_xshm  -lntl -lgmp -lm

# Role explicite de construction de l'executable
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

#DEPENDANCIES
tea_starter.o: tea_starter.cpp