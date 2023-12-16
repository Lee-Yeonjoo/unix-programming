#include <stdio.h>
#include <time.h>
#include "struct.h" //ParkingSpot구조체 타입이 들어있는 헤더파일.(주차구역, 차번호 정보)

//사용자 메뉴를 출력하는 함수.
void print_menu()
{
	printf("========================================\n");
	printf("주차장 관리 프로그램\n");
	printf("(1) 주차장 상황 보기\n");
	printf("(2) 주차요금 안내\n");
	printf("(3) 입차하기\n");
	printf("(4) 출차하기\n");
	printf("(5) 종료\n");
	printf("========================================\n");
}

//주차장 정보를 주차장 모양으로 출력하는 함수.
void print_park_info(ParkingSpot parking_lot[])
{
	for(int j=0; j<3; j++) {
			for(int i=0; i<5; i++) {
				printf("| %4d | ", parking_lot[(5*j)+i].spot_number);
			}
			printf("\n");
			for(int i=0; i<5; i++){
				printf("| %4s | ", parking_lot[(5*j)+i].car_number);
			}
			printf("\n\n");
		}
}

//tm구조체를 통해 평일인지 주말인지에 따라 요금 계산하는 함수.
int calc_fee(struct tm tm_start, int start, int end)
{
	if(tm_start.tm_wday==0 || tm_start.tm_wday==6){ //주말 요금
		return (int)(end-start)*20;
	}
	else{	//평일 요금
		return (int)(end-start)*10;
	}	
}
