int strcmp(const CHAR8* str1, const CHAR8* str2){
	int index=0;
	while(str1[index]==str2[index]&&str1[index]!=0){
		index++;
	}
	return str1[index]-str2[index];
}

int atoi(const char* buf){
	unsigned long long output=0;
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
uint64_t strlen(CHAR8* str){
	uint64_t index=0;
	while(str[index++]);
	return index-1;
}
CHAR8** split_string_by_char(CHAR8* str, CHAR8 seperator, int *section_count){
	int index=0;
	int n_index=0;

	int n_count=0;
	while(str[index]){
		if(str[index++]==seperator){
			n_count++;
		}
	}
	
	index=0;
	if(str[0]!=seperator){
		n_count++;
	}
	else{
		index++;
	}

	CHAR8** sections=AllocatePool(n_count*sizeof(CHAR8*)+strlen(str)+1);
	CHAR8* start = ((CHAR8*)sections)+(n_count*sizeof(CHAR8*));

	*section_count=n_count;
	sections[0]=&start[index];

	while(str[index]){
		start[index]=str[index];
		if(str[index]==seperator){
			start[index]=0;
			sections[++n_index]=start+index+1;
		}
		index++;
		
	}
	str[index]=0;

	return sections;
}
UINTN strncmp(CHAR8* a, CHAR8* b, UINTN length){
	for (UINTN i = 0; i < length; i++){
		if (*a != *b) return *a-*b;
	}
	return 0;
}