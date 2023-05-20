#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cal4(int dataword, int generator);
int cal8(int dataword, int generator);

int main(int argc, char* argv[]){
	if(argc!=5){
		printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
		return 0;
	}
	FILE *input, *output;
	input = fopen(argv[1], "r");
	if(input==NULL){
		printf("input file open error.\n"); return 0;
	}
	output = fopen(argv[2],"wb");
	if(output==NULL){
		printf("output file open error.\n"); return 0;
	}
	if(!((*argv[4]=='4')||(*argv[4]=='8'))){
		printf("dataword size must be 4 or 8.\n"); return 0;
	}
	FILE *bin_file = fopen("binary","w"); // 옮겨적기
	char* generator_c = argv[3];
	int generator = atoi(generator_c);
	int dataword_size = *argv[4]-48; // dataword의 사이즈를 저장함 
	fseek(input, 0, SEEK_END);
	int file_size = ftell(input)-1; // file의 크기를 구하고 패딩의 개수를 계산
	fseek(input, 0, SEEK_SET);
	if(dataword_size==4){ // dataword가 4일경우
		int padding = file_size*2; // output처음에 패딩 수 입력
		for(int i=7;i>=0;--i){
			int pad = padding >> i & 1;
			fputc(pad+48 ,bin_file);
		}
		for(int i=0;i<file_size*2;++i){ // 패딩 입력
			fputc(48, bin_file);
		}
		int data1, data2;
		int dataword1, dataword2;
		for(int i=0;i<file_size;++i){
			int temp = fgetc(input);
			if(temp >= 16){
				data2=temp % 16;
				data1=temp >> 4;
			}
			else{
				data1=0;
				data2=temp;
			}
			dataword1 = data1/8*1000+(data1-data1/8*8)/4*100+(data1-data1/4*4)/2*10+(data1-data1/2*2);
			dataword2 = data2/8*1000+(data2-data2/8*8)/4*100+(data2-data2/4*4)/2*10+(data2-data2/2*2);
			//part1 starts
			int codeword1 = cal4(dataword1, generator);
			int flag1 = 1000000;
			while(codeword1 < flag1){
				fputc(48, bin_file);
				flag1 /= 10;
			}
			while(flag1 > 0){
				if(codeword1 >= flag1){
					fputc(49, bin_file);
					codeword1 -= flag1;
				}
				else fputc(48, bin_file);
				flag1 /= 10;
			}
			//part2 starts
			int codeword2 = cal4(dataword2, generator);
			int flag2 = 1000000;
			while(codeword2 < flag2){
				fputc(48, bin_file);
				flag2 /= 10;
			}
			while(flag2 > 0){
				if(codeword2 >= flag2){
					fputc(49, bin_file);
					codeword2 -= flag2;
				}
				else fputc(48, bin_file);
				flag2 /= 10;
			}
		}
	}
	else{ // dataword가 8일경우
		int padding = file_size * 5;
		for(int i=7;i>=0;--i){
			int pad = padding >> i & 1;
			fputc(pad+48 ,bin_file);
		}
		for(int i=0;i<file_size*5;++i){
			fputc(48,bin_file);
		}
		int data, dataword;
		for(int i=0;i<file_size;++i){
			data = fgetc(input);
			dataword = data/128*10000000+(data-data/128*128)/64*1000000+ (data-data/64*64)/32*100000+(data-data/32*32)/16*10000+(data-data/16*16)/8*1000+(data-data/8*8)/4*100+(data-data/4*4)/2*10+(data-data/2*2);
			int codeword = cal8(dataword, generator);
			int flag = 1000000000;
			fputc(48, bin_file);
			while(codeword < flag){ //앞 0처리
				fputc(48, bin_file);
				flag /= 10;
			}
			while(flag > 0){
				if(codeword >= flag){
					fputc(49, bin_file);
					codeword -= flag;
				}
				else fputc(48, bin_file);
					flag /= 10;
				}
		}

	}
	fclose(input); fclose(bin_file);
	FILE* convert = fopen("binary","r");
	int check, check_cal;

	unsigned char u = 0xff, send[100000];
	for(int i=0;i<1+file_size*2;++i){
		check=0; check_cal=128;
		for(int j=0;j<8;++j){
			check += (fgetc(convert)-48)*check_cal;
			check_cal /= 2;
		}
		send[i] = u & check;
	}
	fwrite(send, sizeof(unsigned char)*(1+2*file_size),1,output);
	
	fclose(output); fclose(convert);
	return 0;
}

int cal4(int dataword, int generator){
	int s[7]={0,}, p[4];
	for(int i=0;i<4;++i) {
		s[3-i] = dataword % 10;
		p[3-i] = generator % 10;
		dataword /= 10; generator /= 10;
	}
	int codeword = s[0]*1000000+s[1]*100000+s[2]*10000+s[3]*1000;
	int f = 0;
	while(f<4){
		if(s[f] == 1) {
			for(int i=0;i<4;++i) s[f+i] = s[f+i]^p[i];
		}
		++f;
	}
	codeword += s[4]*100+s[5]*10+s[6];
	return codeword;
}
int cal8(int dataword, int generator){
	int s[11]={0,}, p[4];
	for(int i=0;i<8;++i){
		s[7-i] = dataword % 10;
		dataword /=10;
	}
	for(int i=0;i<4;++i){
		p[3-i] = generator %10;
		generator /=10;
	}
	int codeword = s[7]*1000+s[6]*10000+s[5]*100000+s[4]*1000000+s[3]*10000000+s[2]*100000000+s[1]*1000000000+s[0]*10000000000;
	int f=0;
	while(f<8){
		if(s[f]==1){
			for(int i=0;i<4;++i) s[f+i] = s[f+i]^p[i];
		}
		++f;
	}
	codeword += s[8]*100+s[9]*10+s[10];
	return codeword;
}
