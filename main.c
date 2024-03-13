/*
 Raspberry Pi driving the TM1637
 port from here
 https://github.com/petrows/esp-32-tm1637 
 cc -o tm1637 main.c tm1637.c -lwiringPi
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <wiringPi.h>
#include "tm1637.h"

void usage() {
	printf("usage: tm1637  [-c CLK] [-d DIO] [-t TEXT]\n\n");
	printf("optional arguments:\n");
	printf("  -c CLK	CLK GPIO\n");
	printf("  -d DIO	DIO GPIO\n");
	printf("  -t TEXT	Display Text\n");
}

int _isdigit(char *text) {
	for (int i=0;i<strlen(text);i++) {
		int c = text[i];
		// printf("text[%d]=%c\n",i, c);
		if (isdigit(c) == 0) return -1;
	}
	return atoi(text);
}

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void my_ip_address(char * address) {
	int fd;
	struct sockaddr_in dst_addr={0};
	struct sockaddr_in src_addr={0};
	socklen_t addrlen;
	char str[16];

	fd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	dst_addr.sin_family=AF_INET;
	dst_addr.sin_port=htons(7);
	inet_aton("128.0.0.0",&dst_addr.sin_addr);
	connect(fd,(struct sockaddr *)&dst_addr,sizeof(dst_addr));
	addrlen=sizeof(src_addr);
	getsockname(fd,(struct sockaddr *)&src_addr,&addrlen);
	inet_ntop(AF_INET,&src_addr.sin_addr,str,sizeof(str));
	// printf("%s\n",str);
	strcpy(address, str);
	close(fd);
}


int main(int argc, char *argv[]) {
	if(wiringPiSetupGpio() == -1) {
		printf("wiringPiSetupGpio Fail\n");
		return 1;
	}

	int opt;
	int clk_gpio = CLK_GPIO;
	int dio_gpio = DIO_GPIO;
	char *text;
	while ((opt = getopt(argc, argv, "c:d:t:")) != -1) {	
		switch (opt) {
		case 'c':
			//printf("optarg=[%s]\n", optarg);
			if (_isdigit(optarg) < 0) {
				usage();
				exit(0);
			}
			clk_gpio = _isdigit(optarg);
			break;
		case 'd':
			//printf("optarg=[%s]\n", optarg);
			if (_isdigit(optarg) < 0) {
				usage();
				exit(0);
			}
			dio_gpio = _isdigit(optarg);
			break;
		case 't':
			//printf("optarg=[%s]\n", optarg);
			text = optarg;
			break;
		default:
			usage();
			exit(0);
		}
	}

	// clock & data pin
	//printf("clk_gpio=%d\n", clk_gpio);
	//printf("dio_gpio=%d\n", dio_gpio);
	tm1637_begin(clk_gpio, dio_gpio);

	// You can set the brightness level from 0(darkest) till 7(brightest)
	// BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
	tm1637_set_brightness(BRIGHT_TYPICAL);

	//printf("text=[%s]\n", text);
	if (text == NULL) {
		// Get my ip address
		char ip_address[16];
		my_ip_address(ip_address);
		printf("ip_address=[%s]\n", ip_address);

		printf("Start Auto Demo. Cntl+C to stop\n");
		while(1) {
			// Test segment control
			uint8_t seg_data[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
			for (int x=0; x<32; ++x)
			{
				uint8_t v_seg_data = seg_data[x%6];
				tm1637_set_segment_fixed(0, v_seg_data);
				tm1637_set_segment_fixed(1, v_seg_data);
				tm1637_set_segment_fixed(2, v_seg_data);
				tm1637_set_segment_fixed(3, v_seg_data);
				delay(100);
			}

			// Test brightness
			for (int x=0; x<7; x++) {
				tm1637_set_brightness(x);
				tm1637_set_number(8888);
				delay(300);
			}

			// Test display text
			tm1637_show_ascii("PLAY");
			sleep(1);
			tm1637_show_ascii("1234");
			sleep(1);
			if (strlen(ip_address)) {
				char work[20];
				snprintf(work, sizeof(work)-1, "IP %s", ip_address);
				//tm1637_show_ascii(ip_address);
				tm1637_show_ascii(work);
				sleep(1);
			} else {
				tm1637_show_ascii("IP 192.168.100.120");
				sleep(1);
			}
			tm1637_show_ascii_with_time("1234", 1000);
			sleep(1);
			tm1637_show_ascii("STOP");
			sleep(1);
		}
	} else {
		tm1637_show_ascii(text);
	}
	return 0;
}
