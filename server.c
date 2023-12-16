#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "struct.h"

#define PORT 9000
#define MAX_SPOTS 15  //주차장 자리 수

ParkingSpot parking_lot[MAX_SPOTS]; //주차장 정보. 모든 스레드가 공유하기 위해 전역.
pthread_mutex_t mutex; //스레드 동기화를 하기 위한 mutex.
extern void *tfunction(void *arg);  //각 스레드에서 실행할 함수.

int main() {
	
	//소켓 관련 변수들.
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	//스레드에 전달할 클라이언트 정보 변수.
	cli_info client;
	int cnt = 0; //클라이언트 아이디 

	//ParkingSpot구조체 배열인 parking_lot 초기화.
	for(int i=0; i<MAX_SPOTS; i++) {
		parking_lot[i].spot_number = i+1;
		strcpy(parking_lot[i].car_number, "없음");
	}

	//소켓 생성.
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(1);
	}

	//소켓 주소 구조체 설정.
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("10.0.2.15"); 
	address.sin_port = htons(PORT);

	//소켓 바인딩.
	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind");
		exit(1);
	}

	//클라이언트의 연결 요청을 기다림.
	if(listen(server_fd, 3) < 0) {
		perror("listen");
		exit(1);
	}

	//준비 마치고, 본격적인 서버 코드.
	printf("서버가 클라이언트를 기다리고 있습니다...\n");
	pthread_mutex_init(&mutex, NULL);  //pthread mutex 초기화.

	//여러 클라이언트를 받기 위해 무한 루프를 통해 클라이언트의 요청을 계속 받음.
	while(1) {
		//클라이언트 요청 수락.
		if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
			perror("accept");
			exit(1);
		}

		//스레드 생성.
		pthread_t t;
		//스레드에 전달할 클라이언트 정보 설정. 클라이언트 소켓과 아이디.
		client.socket = new_socket;
		client.id = ++cnt;
		printf("클라이언트%d 연결 완료\n", client.id);		
	
		if(pthread_create(&t, NULL, tfunction, (void*)&client) != 0) {
			perror("thread");
			exit(1);
		}
		//스레드 분리.
		pthread_detach(t);
	}

	//mutex 해제.
	pthread_mutex_destroy(&mutex);
	return 0;
}
