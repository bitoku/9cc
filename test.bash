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

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34  -    5;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '+5+5;'
assert 0 '+5+-5;'
assert 1 '-1 * -1;'
assert 1 '1==1;'
assert 1 '1<2;'
assert 1 '1<=2;'
assert 1 '1!=0;'
assert 0 '1==0;'
assert 1 '2*2==1+3;'
assert 1 '1<0!=1<2;'
assert 1 '(1==1)>(1==0);'
assert 1 'i=1;i;'
assert 1 'i=j=1;i;'
assert 2 'i=1+1;i;'
assert 11 'i=2*3;j=i+5;j;'
assert 1 'foo=bar=1;foo;'
assert 1 'foo=bar=1;bar;'
assert 0 'return 0;'
assert 2 'return 1+1;'
assert 3 'i=1+2;return i;'
assert 1 'if(1) 1;'
assert 1 'i=0;if(1) i=i+1; i;'
assert 0 'i=0;if(0) i=i+1; i;'
assert 0 'i=0;if(1-1) i=i+1; i;'
assert 0 'i=0;if(i-i) i=i+1; i;'
assert 0 'if(0) 1; else 0;'
assert 10 'j=0; for(i=0;i<10;i=i+1) j=j+1; j;'
assert 10 'i=0; j=0; for(;i<10;i=i+1) j=j+1; j;'
assert 10 'i=0; for(;i<10;) i=i+1; i;'
assert 10 'i=0; while(i<10) i = i + 1; i;'
assert 1 '{i=0;i=i+1;}i;'
assert 5 'i=0;if(i==0){return 5;}else{return 10;}'
assert 5 'i=0;while(i<10){i=i+1; if(i==5) {return 5;}}'
assert 31 'i=0;for(j=0;j<5;j=j+1){i=i*2;i=i+1;}i;'
assert 3 '{ return ret3(); }'
assert 5 '{ return ret5(); }'
assert 8 '{ return ret3() + ret5(); }'
assert 3 '{ return add(1, 2); }'
assert 8 '{ return add(ret3(), ret5()); }'
assert 2 '{ return sub(5, 3); }'
assert 21 '{ return add6(1, 2, 3, 4, 5, 6); }'
assert 66 '{ return add6(1, 2, add6(3, 4, 5, 6, 7, 8), 9, 10, 11); }'
assert 136 '{ return add6(1, 2, add6(3, add6(4, 5, 6, 7, 8, 9), 10, 11, 12, 13), 14, 15, 16); }'

echo OK
