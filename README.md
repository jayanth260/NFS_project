# final-project-36

## Error Codes:
    1-> file/path doesnot exist
    2-> other client is writing to the requested file
    3-> no required permission for file
    4-> file/dir already exists
    5-> create/delete file/dir failed
    6-> server down
    7-> file/dir already deleted


## HOW TO RUN COMMANDS :
    For reading a file we use "read PATH"
    For writing a file we use "write PATH"
    For creating a file we use "create file PATH"
    For reading a directory we use "create dir PATH"
    For deleting a file we use "delete file PATH"
    For deleting a directory we use "delete dir PATH"

## Assumptions :
    The initial ip address we are using is 127.0.0.1 
    Path name should start with ./
    Naming Server must be run first
    

## Client:-

    Server Connection:
        The connect_S function creates a TCP socket, sets up the server address, and connects to the server using the specified IP address (SS_ip) and port number (SS_port).

    Error Handling:
        The die function is a utility function to print an error message using perror and exit the program with EXIT_FAILURE in case of an error.

    Main Functionality:
        It establishes a connection to the server, sends commands and user input to the server, and receives responses.
        It uses non-blocking mode for socket operations during a certain period and then switches back to blocking mode.

    Server Address and Path Validation:
        The code receives information about the server address (SServer_addr) and validates the path on the server.
        If the path is valid, it calls the init function with the server address and buffer.

    Acknowledgment Handling:
        The code waits for an acknowledgment from the server, and if it doesn't receive it within a specified time, it prints a timeout message and exits.

    Socket Configuration:
        The code uses the fcntl function to set the socket to non-blocking mode and later switches it back to blocking mode.

    Path Validation Handling:
        Depending on the validity of the path, the code takes different actions (e.g., calling init or handling invalid path).

    Acknowledgment Printing:
        The code prints messages based on the received acknowledgment from the server.

    We also use resquest_read, requst_write, request_getinfo to implement the respective commands .

## Naming Server

        Initialization:
            The code starts by initializing necessary variables, including the server IP address, port number, and socket for communication.
            It sets up a server socket and binds it to the specified IP address and port.
            Server and Client Threads:
            The program uses two types of threads: Storage_Sfunc for handling storage-related tasks and Client_func for handling client requests.
        
        Server Set Structure:
            The server maintains information about connected storage servers in a structure named Server_set. Each storage server (SServer_details) contains details such as IP address, port, and accessible paths.
       
        Storage Server Function (Storage_Sfunc):
            This thread function is responsible for handling connections from storage servers.
            It receives information about a storage server (SS_recv) and extracts accessible paths using the break_string function.
            The details are stored in the Server_set structure, and the function exits.
        
        Client Function (Client_func):
            This thread function handles connections from clients.
            It receives a command from the client, such as "create," "delete," or other operations, and extracts the file path.
            The accessible paths are checked against the stored server information, and if a match is found, an acknowledgment is sent to the client.
            If the operation is "create" or "delete," a request is forwarded to the appropriate storage server using Request_Create_delete.
            The LRU logic is implemented, and the client connection is closed.
        
        LRU Implementation:
            The code maintains an LRU (Least Recently Used) structure (lru) to track the most recent file path operations and associated storage servers.
            If a path is encountered again, it is moved to the last position in the LRU structure.(QUE priority implementation)



## Storage Server

        Server Initialization (SS_Initialize):
            Initializes the storage server by creating a socket for communication with the Naming Server (NM).
            Binds the socket to a specific port (SS_Nmport) for NM communication.
            Connects to the NM server and sends information about the storage server, including its IP, accessible paths, and communication port.
            Returns the socket for further communication with the NM server.
        
        NM Thread Function (NM_thread_func):
            Runs in a separate thread and continuously initializes the storage server by calling SS_Initialize.
            Listens for commands from the NM server (currently expecting "create" or "delete").
            When a command is received, it calls the create_delete function.
        
        Communication with Clients (C_conn_func):
            Initializes a server socket to listen for client connections.
            Accepts incoming client connections and creates a new thread (C_Request_thread) for each connection.
            Listens for commands from clients (currently expecting "read" or "write").
            Depending on the command, it spawns threads (Read or Write) to handle the corresponding file operation.
        
        Read and Write Operations (Read and Write functions):
            These functions handle read and write operations requested by clients.
            The details of the operations depend on the specific implementation within the Read_file and Write_file functions.
       
        Naming Server Communication:
            Communication with the NM server occurs in the SS_Initialize function, where the storage server sends its details to the NM server.


## Memory and Threads

        Dynamic Memory Allocation:
            The program dynamically allocates memory for various data structures, such as the server set, accessible paths,LRU and buffers.
        
        Infinite Loop and Thread Management:
            The main function runs an infinite loop, continuously accepting client and storage server connections.
            Threads are created for each connection, and the program waits for their completion using pthread_join.
        
        Cleanup:
            Memory allocated dynamically is freed before the program exits.
