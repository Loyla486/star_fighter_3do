#   File:       Get16.make
#   Target:     Get16
#   Sources:    Get16.c
#   Created:    Tuesday, April 11, 1995 10:50:52 am


OBJECTS = Get16.c.o



Get16 �� Get16.make {OBJECTS}
	Link -t APPL -c '????' �
		{OBJECTS} �
		"{CLibraries}"CSANELib.o �
		"{CLibraries}"Math.o �
		#"{CLibraries}"Complex.o �
		"{CLibraries}"StdClib.o �
		"{Libraries}"Runtime.o �
		"{Libraries}"Interface.o �
		-o Get16
Get16.c.o � Get16.make Get16.c
	 C -r  Get16.c
