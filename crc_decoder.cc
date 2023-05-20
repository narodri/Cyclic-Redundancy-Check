#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	if(argc!=6){
		printf("usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
		return 0;
	}
	FILE *pre, *bin, *input, *output, *result;
	pre = fopen(argv[1], "rb");
	if(pre==NULL){
		printf("input file open error.\n"); return 0;
	}
	bin = fopen("binary2","w");
	output = fopen(argv[2],"w");
	if(output==NULL){
		printf("output file open error.\n"); return 0;
	}
	result = fopen(argv[3], "w");
	if(result==NULL){
		printf("result file open error.\n"); return 0;
	}
	if(!((*argv[5]=='4')||(*argv[5]=='8'))){
		printf("dataword size must be 4 or 8.\n"); return 0;
	}
	char* generator_c = argv[4];
	int generator = atoi(generator_c);
	int dataword_size = *argv[5]-48; // dataword의 사이즈를 저장함 
	//bin
	unsigned char receive[100000];
	int tmp, tmp_cal=128;
	int length;
	FILE* sry = fopen("binary","r");
	fseek(sry,0,SEEK_END);
	length = ftell(sry)/8;
	fseek(sry,0,SEEK_SET);
	fclose(sry);
	fread(receive, sizeof(unsigned char)*length, 1, pre);
	for(int i=0;i<length;i++){
		tmp=receive[i];
		tmp_cal = 128;
		for(int i=0;i<8;++i){
			if(tmp>=tmp_cal) {fputc(49, bin); tmp-=tmp_cal;}
			else {fputc(48, bin);}
			tmp_cal/=2;
		}
	}
	//padding num reading
	fclose(bin);
	input = fopen("binary2","r");
	int padding_num=0, padding_cal=128;
	int get;
	for(int i=0;i<8;++i){
		get=fgetc(input)-48;
		padding_num += get*padding_cal;
		padding_cal /= 2;
	}
	fseek(input, padding_num, SEEK_CUR);
	int total=2, error=0; //result file
	int dataword;
	int p[4], p_cal=1000;
	for(int i=0;i<4;i++){
		p[i]=generator/p_cal%10;
		p_cal/=10;
	}
	if(dataword_size==4){
		int s[7];
		while(padding_num > 0){
			for(int i=0;i<7;++i) s[i]=fgetc(input)-48;
			dataword = s[0]*128+s[1]*64+s[2]*32+s[3]*16;
			++total;
			int f = 0;
			while(f<4){
				if(s[f] == 1) {
					for(int i=0;i<4;++i) s[f+i] = s[f+i]^p[i];
				}
				++f;
			}
			int state=0;
			for(int i=0;i<7;++i){
				if(s[i]!=0) {state=1; break;}
			}
			if(state) ++error;
			
			for(int i=0;i<7;++i) s[i]=fgetc(input)-48;
			dataword += s[0]*8+s[1]*4+s[2]*2+s[3];
			++total;
			f = 0;
			while(f<4){
				if(s[f] == 1) {
			 		for(int i=0;i<4;++i) s[f+i] = s[f+i]^p[i];
				}
				++f;
			}
			state=0;
			for(int i=0;i<7;++i){
				if(s[i]!=0) {state=1; break;}
			}
			if(state) ++error;
			fputc(dataword, output); //printf("%c(%d) put\n",dataword,dataword);
			padding_num -= 2;
		}
		fprintf(result, "%d %d", total, error);
	}
	else{
		int s[11];
		while(padding_num > 0){
			for(int i=0;i<11;++i) s[i]=fgetc(input)-48;
			dataword = s[0]*128+s[1]*64+s[2]*32+s[3]*16+s[4]*8+s[5]*4+s[6]*2+s[7];
            ++total;
            int f = 0;
            while(f<8){
                if(s[f] == 1) {
                    for(int i=0;i<4;++i) s[f+i] = s[f+i]^p[i];
                }
                ++f;
            }
            int state=0;
            for(int i=0;i<11;++i){
                if(s[i]!=0) {state=1; break;}
            }
            if(state) ++error;
            fputc(dataword, output);
            padding_num -= 5;
        }
        fprintf(result, "%d %d", total, error);	
	}
	fclose(input); fclose(output); fclose(result);
	return 0;
}
