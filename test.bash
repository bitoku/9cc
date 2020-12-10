#!/usr/bin/env bash

cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret5() { return 5; }
int add(int x, int y) { return x + y; }
int sub(int x, int y) { return x - y; }
int add6(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; }
EOF

assert() {
  expected="$1"
  input="$2"

  ./cmake-build-debug/9cc "$input" > tmp.s
  gcc -o tmp tmp.s tmp2.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "main(){ return 0; }"
assert 42 "main(){return 42;}"
assert 21 "main(){return 5+20-4;}"
assert 41 "main(){ return 12 + 34  -    5;}"
assert 47 'main(){return 5+6*7;}'
assert 15 'main(){return 5*(9-6);}'
assert 4 'main(){return (3+5)/2;}'
assert 10 'main(){return +5+5;}'
assert 0 'main(){return +5+-5;}'
assert 1 'main(){return -1 * -1;}'
assert 1 'main(){return 1==1;}'
assert 1 'main(){return 1<2;}'
assert 1 'main(){return 1<=2;}'
assert 1 'main(){return 1!=0;}'
assert 0 'main(){return 1==0;}'
assert 1 'main(){return 2*2==1+3;}'
assert 1 'main(){return 1<0!=1<2;}'
assert 1 'main(){return (1==1)>(1==0);}'
assert 1 'main(){i=1;return i;}'
assert 1 'main(){i=j=1;return i;}'
assert 1 'main(){i=j=1;return j;}'
assert 2 'main(){i=1+1;return i;}'
assert 11 'main(){i=2*3;j=i+5;return j;}'
assert 1 'main(){foo=bar=1;return foo;}'
assert 1 'main(){foo=bar=1;return bar;}'
assert 0 'main(){return 0;}'
assert 2 'main(){return 1+1;}'
assert 3 'main(){i=1+2;return i;}'
assert 1 'main(){if(1) return 1;}'
assert 1 'main(){i=0;if(1) i=i+1; return i;}'
assert 0 'main(){i=0;if(0) i=i+1; return i;}'
assert 0 'main(){i=0;if(1-1) i=i+1; return i;}'
assert 0 'main(){i=0;if(i-i) i=i+1; return i;}'
assert 0 'main(){if(0) return 1; else return 0;}'
assert 10 'main(){j=0; for(i=0;i<10;i=i+1) j=j+1; return j;}'
assert 10 'main(){i=0; j=0; for(;i<10;i=i+1) j=j+1; return j;}'
assert 10 'main(){i=0; for(;i<10;) i=i+1; return i;}'
assert 10 'main(){i=0; while(i<10) i = i + 1; return i;}'
assert 1 'main(){{i=0;i=i+1;} return i;}'
assert 5 'main(){i=0;if(i==0){ return 5; } else { return 10; }}'
assert 5 'main(){i=0;while(i<10){i=i+1; if(i==5) {return 5;}}}'
assert 31 'main(){i=0;for(j=0;j<5;j=j+1){i=i*2;i=i+1;} return i;}'
assert 3 'main(){ return ret3(); }'
assert 5 'main(){ return ret5(); }'
assert 8 'main(){ return ret3() + ret5(); }'
assert 3 'main(){ return add(1, 2); }'
assert 8 'main(){ return add(ret3(), ret5()); }'
assert 2 'main(){ return sub(5, 3); }'
assert 21 'main(){ return add6(1, 2, 3, 4, 5, 6); }'
assert 66 'main(){ return add6(1, 2, add6(3, 4, 5, 6, 7, 8), 9, 10, 11); }'
assert 136 'main(){ return add6(1, 2, add6(3, add6(4, 5, 6, 7, 8, 9), 10, 11, 12, 13), 14, 15, 16); }'
assert 0 'main(){ return zero(); } zero(){ return 0; }'
assert 1 'main(){ return one(); } one(){ return 1; }'
assert 5 'main(){ return two() + three(); } two() { return 2; } three() { return 3; }'
assert 0 'main(){ return ident(0); } ident(a) { return a; }'
assert 6 'main(){ return mul(2, 3); } mul(a, b) { return a * b; }'
assert 1 'main(){ return mysub(2, 1); } mysub(a, b) { return a - b; }'

echo OK
