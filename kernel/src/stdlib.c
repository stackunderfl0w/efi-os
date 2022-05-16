int atoi(const char* buf){
	int output=0;
	int index=0;
	int sign =1;
	if(buf[index]=='-'){
		sign=-1;
		index++;
	}
	while(buf[index]>='0'&&buf[index]<='9'&&index<20){
		output*=10;
		output+=buf[index++]-'0';
	}
	return output*sign;
}
int atol(const char* buf){
	long output=0;
	long index=0;
	long sign =1;
	if(buf[index]=='-'){
		sign=-1;
		index++;
	}
	while(buf[index]>='0'&&buf[index]<='9'&&index<20){
		output*=10;
		output+=buf[index++]-'0';
	}
	return output*sign;
}
///todo base
char* itoa (int value, char* str, int base){
	if(!value){
		str[0]='0';
		str[1]=0;
		return str;
	}
	int index=0;
	int size=0;
	int temp=value;
	while(temp/=10){
		size++;
	}
	while(value){
		str[size-index]='0'+value%10;
		value/=10;
		index++;
	}
	str[size+1]=0x00;
	return str;
}
char* ltoa (long value, char* str, int base){
	if(!value){
		str[0]='0';
		str[1]=0;
		return str;
	}
	int index=0;
	int size=0;
	int temp=value;
	while(temp/=10){
		size++;
	}
	while(value){
		str[size-index]='0'+value%10;
		value/=10;
		index++;
	}
	str[size+1]=0x00;
	return str;
}