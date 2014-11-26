### Makefile

### Example for a C++ agent:
agent: agent.cpp
	g++ agent.cpp -o agent

run: agent
	./agent

### For Java:
#agent: agent.class
#	
#agent.class: agent.java
#	javac agent.java
#
#run: agent.class
#	java agent


### For python or other non-compiled languages:
#agent:
#	
#run:
#	./agent.py

