# ----------------------------------------------------------
# Makefile based on gmake for compiling the project
# still lacks some features (building proto and tarball)
# ----------------------------------------------------------



# ----------------------------------------------------------
# file lists
#CPP_SRCS = graph.cpp main.cpp
CPP_SRCS = $(wildcard *.cpp)
CPP_OBJS = $(patsubst %.cpp, %.o, $(CPP_SRCS))
EXEC = tp1


# ----------------------------------------------------------
# compiler and related options
CC     = g++
#CPPFLAGS = -Wall -ansi -Wextra -std=c++11
CPPFLAGS = -Wall -ansi -pedantic -Wextra
LFLAGS =
VFLAGS = -v --tool=memcheck --leak-check=yes --show-reachable=yes

MYDATE := `date +%Y%m%d`
MYDIR  := $(notdir $(CURDIR))/


# ----------------------------------------------------------
# conditional insertion of dependancies
ifeq (Makefile.depend, $(wildcard Makefile.depend))
  all :: $(EXEC)
  include Makefile.depend
else
  all :: depend $(EXEC)
endif


# ----------------------------------------------------------
# compilation rules

%.o : %.cpp
	@echo "   obj file : "$*.cpp
	@$(CC) -c $(CPPFLAGS) $*.cpp -o $*.o

$(EXEC) : $(CPP_OBJS)
	@echo "   exe file : "$(EXEC)
	@$(CC) -o  $(EXEC) $(CPP_OBJS) $(LFLAGS)


debug: CPPFLAGS += -g
debug:: clean $(CPP_OBJS)
	@echo "   building the debug version..."
	@$(CC) -o  $(EXEC) $(CPP_OBJS) $(LFLAGS)

#release: CFLAGS += -fvisibility
release: CPPFLAGS += -O2
release:: clean $(CPP_OBJS)
	@echo "   building the release..."
	@echo "   exe file : "$(EXEC)
	@$(CC) -o  $(EXEC) $(CPP_OBJS) $(LFLAGS)
	strip  $(EXEC)

stddebug: $(EXEC)
	@$(MAKE) valgrind $(VFLAGS) PARAMS=' example.dat'

valgrind: $(EXEC)
	@echo " running memory check with "$(PARAMETERS)
#	@valgrind $(VFLAGS) $(EXEC) $(PARAMS)
	@valgrind $(VFLAGS) $(EXEC) $(PARAMETERS)

depend:: Makefile.depend

Makefile.depend :
	@echo "   generating dependencies for ${EXEC}..."
	@$(CC) -MM $(CPP_SRCS) > Makefile.depend

archive:: clean
#	$(MAKE) clean
	@echo "   archiving working directory "$(MYDIR)"..."
	@cd .. ; tar zcf $(EXEC)_$(MYDATE).tgz $(MYDIR)

clean::
	@echo "   cleaning trash files"
	@rm -f core *~
	@echo "   cleaning obj files"
	@rm -f $(CPP_OBJS)
	@echo "   cleaning exe files"
	@rm -f $(EXEC)
	@echo "   removing the dependency file"
	@rm -f Makefile.depend
