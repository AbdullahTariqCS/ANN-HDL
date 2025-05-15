#pragma once
#define FOR_1(X) X
#define FOR_2(X) X;X
#define FOR_3(X) X;X;X
#define FOR_4(X) X;X;X;X
#define FOR_5(X) X;X;X;X;X
#define FOR_6(X) X;X;X;X;X;X
#define FOR_7(X) X;X;X;X;X;X;X
#define FOR_8(X) X;X;X;X;X;X;X;X
#define FOR_9(X) X;X;X;X;X;X;X;X;X
#define FOR_10(X) X;X;X;X;X;X;X;X;X;X
#define FOR_11(X) X;X;X;X;X;X;X;X;X;X;X
#define FOR_12(X) X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_13(X) X;X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_14(X) X;X;X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_15(X) X;X;X;X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_16(X) X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_17(X) X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_18(X) X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_19(X) X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X
#define FOR_20(X) X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X;X

#define CONCAT(a, b) a##b
#define EXPAND_CONCAT(a, b) CONCAT(a, b)
#define FOR(I, X) EXPAND_CONCAT(FOR_, I)(X)