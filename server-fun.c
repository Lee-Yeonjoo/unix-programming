#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "struct.h"

#define PORT 9000
#define MAX_SPOTS 15

extern ParkingSpot parking_lot[MAX_SPOTS]; //주차장 정보. 모든 스레드가 공유하기 위해 전역배열.
extern pthread_mutex_t mutex; //스레드 동기화를 하기 위한 mutex.

void *tfunction(void *arg) {
	cli_info client = *(cli_info*)arg;  //인자로 받은 클라이언트 소켓과 아이디 
	int input;  //클라이언트에서 보낸 사용자가 입력한 메뉴 번호.
	ParkingSpot parking;  //클라이언트에서 보낸 사용자의 주차 구역과 차 번호.

	//클라이언트에게 처음 주차장 정보 보내기.
	send(client.socket, parking_lot, sizeof(parking_lot), 0); 

	
	//무한루프를 통해 클라이언트와 계속 연결 유지
	while(1) {
		//클라이언트로부터 메뉴번호를 받아서 input에 저장.
		recv(client.socket, &input, sizeof(input), 0);  
			
		//input값에 따른 처리.
		if(input == 1){
			//클라이언트에게 주차장 정보(parking_lot) 전송.
			send(client.socket, parking_lot, sizeof(parking_lot), 0);
			printf("주차장 정보를 클라이언트%d에게 전송했습니다.\n", client.id );
		}
		else if(input == 3) {
			//클라이언트로부터 parking(주차 구역과 차 번호)을 받음.
			recv(client.socket, &parking, sizeof(parking), 0);
		
			//주차장 정보(parking_lot)이 전역변수이므로 mutex lock.
			pthread_mutex_lock(&mutex);
			//critical section 시작
			strcpy(parking_lot[parking.spot_number-1].car_number, parking.car_number);
			printf("클라이언트%d 입차에 성공하였습니다. 구역: %d, 차 번호: %s\n", client.id, parking_lot[parking.spot_number-1].spot_number, parking_lot[parking.spot_number-1].car_number);
			//critical section 끝
			//mutex 잠금 해제.
			pthread_mutex_unlock(&mutex);
		}
		else if(input == 4) {

			//mutex 잠금.
			pthread_mutex_lock(&mutex);
			//critical section 시작.
			strcpy(parking_lot[parking.spot_number-1].car_number, "없음");
			printf("클라이언트%d 출차에 성공하였습니다. 구역: %d\n", client.id, parking_lot[parking.spot_number-1].spot_number);
			
			//critical section 끝.
			pthread_mutex_unlock(&mutex); //잠금 해제.
		}
		else if(input == 5) {
			//클라이언트 종료.
			printf("클라이언트%d 종료\n", client.id);
			break;
		}			
	
	}
		
	close(client.socket);
	pthread_exit(NULL);
} 
