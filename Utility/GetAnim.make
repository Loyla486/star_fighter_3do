#   File:       GetAnim.make
#   Target:     GetAnim
#   Sources:    GetAnim.c
#   Created:    Thursday, May 4, 1995 11:33:12 am


OBJECTS = GetAnim.c.o



GetAnim �� GetAnim.make {OBJECTS}
	Link -t APPL -c '????' �
		{OBJECTS} �
		"{CLibraries}"CSANELib.o �
		"{CLibraries}"Math.o �
		#"{CLibraries}"Complex.o �
		"{CLibraries}"StdClib.o �
		"{Libraries}"Runtime.o �
		"{Libraries}"Interface.o �
		-o GetAnim
GetAnim.c.o � GetAnim.make GetAnim.c
	 C -r  GetAnim.c
