#   File:       Aifc_fix.make
#   Target:     Aifc_fix
#   Sources:    Aifc_fix.c
#   Created:    Wednesday, August 9, 1995 10:24:24 am


OBJECTS = Aifc_fix.c.o



Aifc_fix �� Aifc_fix.make {OBJECTS}
	Link -t APPL -c '????' �
		{OBJECTS} �
		"{CLibraries}"CSANELib.o �
		"{CLibraries}"Math.o �
		#"{CLibraries}"Complex.o �
		"{CLibraries}"StdClib.o �
		"{Libraries}"Runtime.o �
		"{Libraries}"Interface.o �
		-o Aifc_fix
Aifc_fix.c.o � Aifc_fix.make Aifc_fix.c
	 C -r  Aifc_fix.c
