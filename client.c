#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include "struct.h" //ParkingSpot구조체 타입이 들어있는 헤더파일.(주차구역, 차번호 정보)

#define PORT 9000
#define MAX_SPOTS 15 //주차장 자리 수

extern void print_menu();
extern void print_park_info(ParkingSpot parking_lot[]);
extern int calc_fee(struct tm tm_start, int start, int end);

int main() {
	//소켓 통신에 쓰이는 변수들. 
	struct sockaddr_in serv_addr;
	int sock = 0;

	//전송데이터에 쓰이는 변수들.
	ParkingSpot parking_lot[MAX_SPOTS]; //주차장 전체 구역에 대한 정보를 담을 구조체 배열.
	int input;	//사용자가 입력할 메뉴번호.
	ParkingSpot parking;  //사용자가 요청할 주차 구역과 차 번호를 담을 변수.

	//입차, 출차 시간 관련 변수들.
	time_t start, end;  //요금 측정을 위한 입차, 출차시간.
	struct tm tm_start;	//입차 시간을 담을 tm구조체.
	struct tm tm_end;	//출차 시간을 담을 tm구조체
	char str[] = "%G년 %m월 %d일 %H:%M";  //시간 출력을 위한 형식.
	char buf[50];  //시간 출력을 위한 버퍼.

	int is_parking = 0; //주차 여부를 나타냄. 사용자 입력에 대한 예외 처리를 위함.

	//소켓 파일 생성
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	//소켓 주소 구조체 설정.
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr("10.0.2.15");

	//서버에 연결 요청.
	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
		exit(1);
	}
	printf("연결완료\n");

	 //서버로부터 처음 주차장 정보 바로 받기.
	recv(sock, parking_lot, sizeof(parking_lot), 0);

//준비 마치고, 본격적인 클라이언트 코드.
while(1) {
	//사용자 메뉴 출력.
	print_menu();
	printf("번호를 선택하세요(숫자만 입력하세요!): ");
	scanf("%d", &input);
	
	//사용자 메뉴번호 따른 처리.	
	if(input==1){
		
		//(1) 주차장 상황 보기
		//서버에 사용자가 입력한 메뉴번호를 전송.
		send(sock, &input, sizeof(input), 0);

		//서버로부터 주차장정보 수신.
		recv(sock, parking_lot, sizeof(parking_lot), 0);
		printf("서버로부터 주차장 정보를 받았습니다.\n");

		//받은 주차장 정보를 출력.
		printf("주차장 정보:\n");
		print_park_info(parking_lot);
		printf("\n");
	}
	else if(input == 2){
		//(2) 주차요금 안내
		//서버에 사용자가 입력한 메뉴번호를 전송.
		send(sock, &input, sizeof(input), 0);

		//결과를 보기 위해 초 단위로 설정.
		printf("\n주차 요금\n");
		printf("평일 요금: 초당 10원\n");
		printf("주말 요금: 초당 20원\n\n");
	}
	else if(input == 3){
		
		//(3) 입차하기
		if(is_parking == 1) {  //이미 입차를 한 경우.
			printf("이미 입차를 하셨습니다. 출차 후 다시 입차해주세요.\n\n");

		} else if(is_parking == 0) {

			//사용자에게 입차할 구역과 차 번호 입력받기.
			while(1) {
				printf("주차할 구역과 차 번호를 입력해주세요(입력 예시 1 1234): ");
				scanf("%d %s", &parking.spot_number, parking.car_number);
	
				if(strcmp(parking_lot[parking.spot_number-1].car_number, "없음") != 0){
					printf("이미 다른 차가 주차되어 있습니다! 다시 선택해주세요.\n\n");
					continue;
				}else{
					//서버에 사용자가 입력한 메뉴번호를 전송.
					send(sock, &input, sizeof(input), 0);

					//입력받은 정보를 서버에 보내 입차 요청.
					send(sock, &parking, sizeof(parking), 0);
					printf("입차가 완료되었습니다.");
					is_parking = 1;  //입차했음을 나타냄.	

					//입차 시간 측정하고, 출력.
					time(&start);
					tm_start = *localtime(&start);
					strftime(buf, sizeof(buf), str, &tm_start);
					printf("(입차 시간: %s)\n\n", buf);
					break;
				}
			}
		}
	}
	else if(input == 4){
		
		//(4) 출차하기
		if(is_parking == 1) {
			//서버에 사용자가 입력한 메뉴번호를 전송.
			send(sock, &input, sizeof(input), 0);
	
			//출차 완료.
			printf("출차가 완료되었습니다.");
			is_parking = 0; //출차했음을 나타냄.

			//출차 시간 측정하고, 출력.
			time(&end);
			tm_end = *localtime(&end);
			strftime(buf, sizeof(buf), str, &tm_end);
			printf("(출차 시간: %s)\n", buf);
		
			//calc_fee함수를 통해 요금 계산 후, 출력.
			printf("주차 요금: %d원(%d초 이용)\n\n", calc_fee(tm_start, start, end), (int)(end-start));
		}else if(is_parking == 0){  //입차를 하지 않았는데 출차 요청한 경우.
			printf("입차를 하지 않았습니다!\n\n");
		}
		
	}
	else if(input == 5){

		//(5) 종료
		if(is_parking == 1){	//출차를 하지 않고, 종료하는 경우.
			printf("출차를 하지 않았습니다! 출차를 해주세요.\n\n");
		}
		else if(is_parking == 0){
			//서버에 사용자가 입력한 메뉴번호를 전송.
			send(sock, &input, sizeof(input), 0);

			printf("종료\n");
			break;
		}
	}
	else {
		//1~5외의 숫자 입력 시 문구.
		printf("다시 입력하세요.\n\n");
	}

}
	//소켓파일 닫기.
	close(sock);
	return 0;
}
