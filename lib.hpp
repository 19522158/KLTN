#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<signal.h>
#include<fstream>
#include<errno.h>
#include<sys/stat.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

void receive_image2(char image_file_name[],int* new_socket);
void socket_init2(int* socket_desc2, int* c2);
void socket_init(int* socket_desc, int* c);
void send_image(int* socket_desc);

