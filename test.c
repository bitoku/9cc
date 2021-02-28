//
// Created by 徳備彩人 on 2021/02/28.
//

#include <stdio.h>
#include <stdlib.h>

struct TestCase {
    char *code;
    int ret;
};

typedef struct TestCase TestCase;

int main() {
    TestCase testCase[] = {
            {"main(){ return 0; }", 0},
            {"main(){return 42;}", 42},
            {"main(){return 5+20-4;}", 21},
            {"main(){ return 12 + 34  -    5;}", 41},
            {"main(){return 5+6*7;}", 47},
            {"main(){return 5*(9-6);}", 15},
            {"main(){return (3+5)/2;}", 4},
            {"main(){return +5+5;}", 10},
            {"main(){return +5+-5;}", 0},
            {"main(){return -1 * -1;}", 1},
            {"main(){return 1==1;}", 1},
            {"main(){return 1<2;}", 1},
            {"main(){return 1<=2;}", 1},
            {"main(){return 1!=0;}", 1},
            {"main(){return 1==0;}", 0},
            {"main(){return 2*2==1+3;}", 1},
            {"main(){return 1<0!=1<2;}", 1},
            {"main(){return (1==1)>(1==0);}", 1},
            {"main(){i=1;return i;}", 1},
            {"main(){i=j=1;return i;}", 1},
            {"main(){i=j=1;return j;}", 1},
            {"main(){i=1+1;return i;}", 2},
            {"main(){i=2*3;j=i+5;return j;}", 11},
            {"main(){foo=bar=1;return foo;}", 1},
            {"main(){foo=bar=1;return bar;}", 1},
            {"main(){return 0;}", 0},
            {"main(){return 1+1;}", 2},
            {"main(){i=1+2;return i;}", 3},
            {"main(){if(1) return 1;}", 1},
            {"main(){i=0;if(1) i=i+1; return i;}", 1},
            {"main(){i=0;if(0) i=i+1; return i;}", 0},
            {"main(){i=0;if(1-1) i=i+1; return i;}", 0},
            {"main(){i=0;if(i-i) i=i+1; return i;}", 0},
            {"main(){if(0) return 1; else return 0;}", 0},
            {"main(){j=0; for(i=0;i<10;i=i+1) j=j+1; return j;}", 10},
            {"main(){i=0; j=0; for(;i<10;i=i+1) j=j+1; return j;}", 10},
            {"main(){i=0; for(;i<10;) i=i+1; return i;}", 10},
            {"main(){i=0; while(i<10) i = i + 1; return i;}", 10},
            {"main(){{i=0;i=i+1;} return i;}", 1},
            {"main(){i=0;if(i==0){ return 5; } else { return 10; }}", 5},
            {"main(){i=0;while(i<10){i=i+1; if(i==5) {return 5;}}}", 5},
            {"main(){i=0;for(j=0;j<5;j=j+1){i=i*2;i=i+1;} return i;}", 31},
            {"main(){ return ret3(); }", 3},
            {"main(){ return ret5(); }", 5},
            {"main(){ return ret3() + ret5(); }", 8},
            {"main(){ return add(1, 2); }", 3},
            {"main(){ return add(ret3(), ret5()); }", 8},
            {"main(){ return sub(5, 3); }", 2},
            {"main(){ return add6(1, 2, 3, 4, 5, 6); }", 21},
            {"main(){ return add6(1, 2, add6(3, 4, 5, 6, 7, 8), 9, 10, 11); }", 66},
            {"main(){ return add6(1, 2, add6(3, add6(4, 5, 6, 7, 8, 9), 10, 11, 12, 13), 14, 15, 16); }", 136},
            {"main(){ return zero(); } zero(){ return 0; }", 0},
            {"main(){ return one(); } one(){ return 1; }", 1},
            {"main(){ return two() + three(); } two() { return 2; } three() { return 3; }", 5},
            {"main(){ return ident(0); } ident(a) { return a; }", 0},
            {"main(){ return mul(2, 3); } mul(a, b) { return a * b; }", 6},
            {"main(){ return mysub(2, 1); } mysub(a, b) { return a - b; }", 1},
            {"main(){ return add6(1, 2, add6(3, add6(4, 5, 6, 7, 8, 9), 10, 11, 12, 13), 14, 15, 16); } add6(a, b, c, d, e, f) { return a + b + c + d + e + f; }",
             136},
            {"main() { x=3; return *&x; }", 3},
            {"main() { x=3; y=&x; return *y; }", 3},
            {"main() { x=3; y=&x; z=&y; return **z; }", 3},
            {"main() { x=3; y=5; return *(&x+8); }", 5},
            {"main() { x=3; y=5; return *(&y-8); }", 3},
            {"main() { x=3; y=&x; *y=5; return x; }", 5},
            {"main() { x=3; y=5; *(&x+8)=7; return y; }", 7},
            {"main() { x=3; y=5; *(&y-8)=7; return x; }", 7},
            NULL
    };
    for (int i = 0; &testCase[i] != NULL; ++i) {
        char cmd[512];
        sprintf(cmd, "./9cc %s", testCase[i].code);
        int x = system(cmd);
        if (x == testCase[i].ret) {
            printf("%s => %d\n", testCase[i].code, testCase[i].ret);
        } else {
            printf("Fail\n");
            printf("%s => %d (expected %d)", testCase[i].code, x, testCase[i].ret);
        }
    }
}