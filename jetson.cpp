#include "util.h"
#include <pthread.h>
#include <mosquitto.h>

using namespace cv;
using namespace std;
pthread_t thread1,thread2;
int socket_desc;
vector<string> names;
char find_name[20] = " ";

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
        printf("ID: %d\n", * (int *) obj);
        if(rc) {
                printf("Error with result code: %d\n", rc);
                exit(-1);
        }
        mosquitto_subscribe(mosq, NULL, "test/t1", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
        printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);

        string mes((char *) msg->payload);
        strcpy(find_name, mes.c_str());
        cout << find_name << endl;
//      cout << (char *) msg->payload   << endl
}

void* Receive(void *arg){
	char message[20] = {};
	int id = 12;
	ofstream myfile("name.txt",ios_base::app);
	// MQTT Config
	mosquitto_lib_init();
	struct mosquitto *mosq;
	mosq = mosquitto_new("subscribe-test", true, &id);
	int rc = mosquitto_connect(mosq,"119.17.253.45", 1883,10);
	if(rc != 0) {
                printf("Could not connect to Broker with return code %d\n", rc);
                exit(1);
        }
        mosquitto_connect_callback_set(mosq, on_connect);
        mosquitto_message_callback_set(mosq, on_message);
        mosquitto_loop_start(mosq);
        
	while(socket_desc > 0){
		read(socket_desc, message, 20);
		string s(message);
		int i = names.size();
		s = "dataset/" s + ".jpg";
		char image_file_name[s.size() +1];
		strcpy(image_file_name, s.c_str());
		int f = receive_image2(image_file_name,&socket_desc);
		if (f == 1){
			myfile << s << endl;
			names.push_back(s);
		}
		cout << i << endl;
		cout << s << endl;
		
		
	}
	mosquitto_loop_stop(mosq, true);
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
	myfile.close();
	pthread_exit(NULL);
}

void* CaptureFunction(void *arg){
	CascadeClassifier face_cascade;
        VideoCapture cap(0);
        Mat frame;
//    char* window_name = "Face Recognition Application";
        string face_cascade_name = "haarcascade_frontalface_alt2.xml";
        string fn_csv = string("face_db.csv");

        vector<Mat> images;
        vector<int> labels;
        Ptr<face::FaceRecognizer> model;

    // Read the csv file to get all images and its labels
        if (read_csv(fn_csv, images, labels) == -1)
                exit(0);

    // Load Haar Cascade for face detection
        if (loadCascade(face_cascade, face_cascade_name) == -1)
                exit(0);

    // Check camera availability
        if (checkCamera(cap) == -1)
                exit(0);

    // Create LBPHFaceRecognizer with 80 of threshold
        create_model(model, 80, images, labels);
        std::string ffmpeg_cmd = std::string("ffmpeg -y -f rawvideo -r ") + std::to_string(5) + " -video_size " + std::to_string(1280) + "x" +  std::to_string(720) + " -pixel_format bgr24 -i pipe: -vcodec libx264 -crf 24 -pix_fmt yuv420p -f rtsp -rtsp_transport tcp rtsp://119.17.253.45:8556/mystream2";
	while(1){
		FILE *pipeout = popen(ffmpeg_cmd.c_str(), "w");
		while (strcmp(find_name, "Stream") == 0 && socket_desc > 0){
			bool bSuccess = cap.read(frame);
		        if (!bSuccess)
		        {
		                cout << "Error reading frame from camera feed" << endl;
		                break;
		        }
		// Flip the frame
		        flip(frame, frame, 1);
                       fwrite(frame.data, 1, 1280*720*3, pipeout);
                       imshow("test", frame);
		        switch (waitKey(30))
		        {
		                case 27:
		                exit(0);
		        }

		}
		fflush(pipeout);
                pclose(pipeout);
		while (strcmp(find_name, "Stream") != 0 && socket_desc > 0){
			bool bSuccess = cap.read(frame);
		        if (!bSuccess)
		        {
		                cout << "Error reading frame from camera feed" << endl;
		                break;
		        }
		// Flip the frame
		        flip(frame, frame, 1);
		// Detect and predict all faces in frame
		        int t = detect_and_predict(face_cascade, model, frame, names,find_name);
			if(t==1)
			{
				imwrite("name.jpg",frame);
				send_image(&socket_desc);
			}
		// Show the frame
		        imshow("test", frame);
		        switch (waitKey(30))
		        {
		                case 27:
		                exit(0);
		        }
		}
        }
        pthread_exit(NULL);
	
}

int main(){
	socket_init(&socket_desc);
	get_name(names);
	pthread_create(&thread1, NULL, &CaptureFunction, NULL);
	pthread_create(&thread2, NULL, &Receive, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
}

