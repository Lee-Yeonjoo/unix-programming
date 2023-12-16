#ifndef STRUCT_H
#define STRUCT_H

//전송 데이터가 될 구조체. 주차 구역과 차 번호 정보.
typedef struct {
	int spot_number;
	char car_number[20];
}ParkingSpot;


//클라이언트당 소켓 정보와 클라이언트 아이디를 담을 구조체.
typedef struct {
	int socket;
	int id;
}cli_info;

#endif
