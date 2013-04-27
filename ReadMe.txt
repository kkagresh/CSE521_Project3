CSE 521: Project-3: To implement a Distributed Event Coordination System

There are 2 source file and a makefile in this package i.e.  
1. dec_server.c
2. dec_client.c
3. makefile

Compilation using makefile
make -f makefile

Compilation Command: 
For Server
gcc dec_server.c -o dec_server

For Client
gcc dec_client.c -o dec_client

Execution Command:
For Server
./dec_server -h -p 5555 -l logs.txt

For Client
./dec_client -h -s 127.0.0.1 -p 5555

(Note: You can change the above arguments. It is just a sample)

			
