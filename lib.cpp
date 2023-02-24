#include "lib.hpp"

using namespace std;

void socket_init(int* socket_desc, int* c){
    struct sockaddr_in server,client;
    //Create socket
    *socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (*socket_desc == -1)
    {
        printf("Could not create socket");
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8889 );
    //Bind
    while(bind(*socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      puts("bind failed");
        sleep(1);
    }

    puts("bind done");
    //Listen

    listen(*socket_desc , 3); // int listen(int s, int backlog); | `backlog` (3)
    //limits the number of outstanding connections in the socket's listen
    //queue to the value specified by the backlog argument.
    *c = sizeof(struct sockaddr_in);
}

void socket_init2(int* socket_desc2, int* c2){
    struct sockaddr_in server,client;
    //Create socket
    *socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
    if (*socket_desc2 == -1)
    {
        printf("Could not create socket 2");
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8879 );
    //Bind
    while(bind(*socket_desc2,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      puts("bind failed");
        sleep(1);
    }

    puts("bind done");
    //Listen

    listen(*socket_desc2 , 3); // int listen(int s, int backlog); | `backlog` (3)
    //limits the number of outstanding connections in the socket's listen
    //queue to the value specified by the backlog argument.
    *c2 = sizeof(struct sockaddr_in);
}

void send_image(int* socket_desc)
{
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;
    picture = fopen("name.jpg", "r");
    printf("Getting Picture Size\n");
    if(picture == NULL) {
         printf("Error Opening Image File"); }
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);
    //Send Picture Size
    printf("Sending Picture Size\n");
    write(*socket_desc, (void *)&size, sizeof(int));
    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");
    printf("Received data in socket\n");
    printf("Socket data: %s\n", read_buffer);
    while(!feof(picture)) {
       //Read from the file into our send buffer
       read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
       //Send data through our socket
       do{
         stat = write(*socket_desc, send_buffer, read_size);
       }while (stat < 0);
       printf("Packet Number: %i\n",packet_index);
       printf("Packet Size Sent: %i\n",read_size);
       printf(" \n");
       printf(" \n");
       packet_index++;
//      /Zero out our send buffer
       bzero(send_buffer, sizeof(send_buffer));
        }
}

void receive_image2(char image_file_name[], int* new_socket)
{ // Start function

        int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
        char imagearray[10241],verify = '1';
        FILE *image;
//Find the size of the image
        do{
                stat = read(*new_socket, &size, sizeof(int));
        }while(stat<0);

        printf("Image size: %i\n",size);
        printf(" \n");
        if(size == 0 )
        {
                printf("Thiết bị ngắt kết nối \n");
                *new_socket = 0;
                return ;
        }
        char buffer[] = "Got it";
        printf(" \n");
        image = fopen(image_file_name, "w");
        if( image == NULL) {
                printf("Error has occurred. Image file could not be opened\n");
                exit(1);
        }
//Loop while we have not received the entire file yet
        int need_exit = 0;
        struct timeval timeout = {10,0};
        fd_set fds;
        int buffer_fd, buffer_out;
        while(recv_size < size) {
//while(packet_index < 2){
                FD_ZERO(&fds);
                FD_SET(*new_socket,&fds);
                buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
                if (buffer_fd < 0)
                        printf("error: bad file descriptor set.\n");

                if (buffer_fd == 0)
                {
                        printf("error: buffer read timeout expired.\n");
                        return;
                }

                if (buffer_fd > 0)
                {
                        do{
                                read_size = read(*new_socket,imagearray, 10241);
                        }while(read_size <0);

        //Write the currently read data into our image file
                        write_size = fwrite(imagearray,1,read_size, image);
                        if(read_size !=write_size) {
                                printf("error in read write\n");    }
             //Increment the total number of bytes read
                        recv_size += read_size;
                        packet_index++;

                }
        }
//      return image;
        fclose(image);
        printf("Image successfully Received!\n");
        return;
}

