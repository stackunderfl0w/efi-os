inline bool isupper(char c){
	return((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
inline bool islower(char c){
	return((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
inline bool isalpha(char c){
	return ((unsigned char)c|32)-'a' < 26;
}
inline bool isprint(char c){
	return((c >= 0x20 && c <= 0x7e));
}
inline bool isdigit(char c){
	return((c >= '0' && c <= '9'));
}