// do not edit this generated file!
#include "idkeyword.h"

#define IS_A_(c) (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c == '-')
#define IS_AN_(c) (IS_A_(c) || c >= '0' && c <= '9')
#define ID_KW(c) if (IS_AN_(c)) return -1; else return TT_ID
#define KW(c, k) if (IS_AN_(c)) return -1; else return k

int st_idkeyword(int s, int c)
{
switch(s) {
case -1: ID_KW(c);
case 0: switch (c) { case 105: return -2;
case 110: return -4;
case 97: return -7;
case 111: return -10;
case 101: return -12;
case 102: return -22;
default: if (IS_A_(c)) return -1; else return 0;
}
case -2: switch (c) { case 102: return -3;
case 110: return -25;
default: ID_KW(c);
}
case -3: KW(c, TT_KW_if);
case -25: KW(c, TT_KW_in);
case -4: switch (c) { case 111: return -5;
default: ID_KW(c);
}
case -5: switch (c) { case 116: return -6;
default: ID_KW(c);
}
case -6: KW(c, TT_KW_not);
case -7: switch (c) { case 110: return -8;
default: ID_KW(c);
}
case -8: switch (c) { case 100: return -9;
default: ID_KW(c);
}
case -9: KW(c, TT_KW_and);
case -10: switch (c) { case 114: return -11;
default: ID_KW(c);
}
case -11: KW(c, TT_KW_or);
case -12: switch (c) { case 108: return -13;
case 110: return -18;
default: ID_KW(c);
}
case -13: switch (c) { case 115: return -14;
case 105: return -16;
default: ID_KW(c);
}
case -14: switch (c) { case 101: return -15;
default: ID_KW(c);
}
case -15: KW(c, TT_KW_else);
case -16: switch (c) { case 102: return -17;
default: ID_KW(c);
}
case -17: KW(c, TT_KW_elif);
case -18: switch (c) { case 100: return -19;
default: ID_KW(c);
}
case -19: switch (c) { case 105: return -20;
case 102: return -26;
default: ID_KW(c);
}
case -20: switch (c) { case 102: return -21;
default: ID_KW(c);
}
case -21: KW(c, TT_KW_endif);
case -26: switch (c) { case 111: return -27;
default: ID_KW(c);
}
case -27: switch (c) { case 114: return -28;
default: ID_KW(c);
}
case -28: KW(c, TT_KW_endfor);
case -22: switch (c) { case 111: return -23;
default: ID_KW(c);
}
case -23: switch (c) { case 114: return -24;
default: ID_KW(c);
}
case -24: KW(c, TT_KW_for);
default: return 0;
}
}
