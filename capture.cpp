#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <string>
using namespace cv;

using namespace std;

void send_image(int* socket_desc)
{
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;
    picture = fopen("send.jpg", "r");
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

void socket_init(int* socket_desc){
	struct sockaddr_in server;
        *socket_desc = socket(AF_INET , SOCK_STREAM , 0);
        if (*socket_desc == -1) {
                exit(0);
        }
        memset(&server,0,sizeof(server));
        server.sin_addr.s_addr = inet_addr("119.17.253.45");
        server.sin_family = AF_INET;
        server.sin_port = htons( 8879 );
        if (connect(*socket_desc , (struct sockaddr *)&server , sizeof(server)) <0){
                 cout<<strerror(errno);
                close(*socket_desc);
                puts("Connect Error");
                exit(0);
        }	
}
	

int main() {
        Mat image,frame_gray;
        VideoCapture cap(0);
        int socket_desc; 
//        cap.set(CAP_PROP_FRAME_WIDTH, 200);
//        cap.set(CAP_PROP_FRAME_HEIGHT, 200);
	char name[20] = "Tan Tai";
//	cin.ignore();
	cin.getline(name,sizeof(name));
	cout << name << endl;
        if (!cap.isOpened()) {
                cout << "cannot open camera";
        }
        CascadeClassifier faceCascade;
        faceCascade.load("haarcascade_frontalface_default.xml");
        if (faceCascade.empty())
        {
                cout << "XML not loaded" << endl;
        }
	socket_init(&socket_desc);
	write(socket_desc, &name,sizeof(char)*19);
        while (true) {
                cap >> image;
                cvtColor(image,frame_gray,COLOR_BGR2GRAY);
                equalizeHist(frame_gray,frame_gray);
                vector<Rect> faces;
                faceCascade.detectMultiScale(frame_gray, faces, 1.9, 5);
               /* for (int i = 0;i < faces.size(); i++)
                {
                        rectangle(image, faces[i].tl(),faces[i].br(), Scalar(25>
                }*/
                if (faces.size() != 0)
                {
                	 resize(frame_gray, frame_gray, Size(200, 200));
                        imwrite("send.jpg",frame_gray);
                        send_image(&socket_desc);
                        break;
                }
                imshow("Name",frame_gray);
                char c = (char)waitKey(25);//Allowing 25 milliseconds frame processing time and initiating break condition//
      		if (c == 27){ //If 'Esc' is entered break the loop//
         		break;
      		}

        }
        return 0;
}

