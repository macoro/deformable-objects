static const char* getNodes() { return(
"8  3  0  0\n""   0    0.0 0.0 0.0\n""   1    1.0 0.0 0.0\n""   2   1.0 1.0 0.0\n""   3    0.0 1.0 0.0\n"
"4    0.0 0.0 1.0\n""5    1.0 0.0 1.0\n""6    1.0 1.0 1.0\n""7    0.0 1.0 1.0\n"
); }
static const char* getElements() { return(
"6  4  0\n""0      5     6     0    4\n" "1      0     7     2     3\n" "2      2     6     0     1\n" 
"3      0     6     5     1\n" "4      0     6     2     7\n" "5      6     7     0     4\n"
); }