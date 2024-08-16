import ctypes
import sys
import random


class BigNum(ctypes.c_void_p):
    pass


bndll = ctypes.CDLL("./libignum.so")

_create = bndll.bn_create
_create.argtypes = [ctypes.c_char_p]
_create.restype = BigNum

bn_delete = bndll.bn_delete
bn_delete.argtypes = [ctypes.c_void_p]
bn_delete.restype = None

bn_add_to = bndll.bn_add
bn_add_to.argtypes = [BigNum, BigNum]
bn_add_to.restype = None

bn_noop = bndll.bn_noop
bn_noop.argtypes = [ctypes.c_int, ctypes.c_int]
bn_noop.restype = None

bn_sub_from = bndll.bn_subtract
bn_sub_from.argtypes = [BigNum, BigNum]
bn_sub_from.restype = None

bn_increment = bndll.bn_increment
bn_increment.argtypes = [BigNum]
bn_increment.restype = None

bn_decrement = bndll.bn_decrement
bn_decrement.argtypes = [BigNum]
bn_decrement.restype = None

bn_compare = bndll.bn_compare
bn_compare.argtypes = [BigNum, BigNum]
bn_compare.restype = ctypes.c_int

_tohexstr = bndll.bn_tohexstr
_tohexstr.argtypes = [BigNum]
_tohexstr.restype = ctypes.POINTER(ctypes.c_char)

bn_copyto = bndll.bn_copyto
bn_copyto.argtypes = [BigNum, BigNum]
bn_copyto.restype = None


def bn_create(n: int) -> BigNum:
    return _create(str(n).encode())


def bn_tohexstr(a: BigNum) -> str:
    rs = _tohexstr(a)
    ret = ctypes.cast(rs, ctypes.c_char_p).value.decode("utf-8")
    bn_delete(rs)
    return ret


# test = True
if True:  # True:
    BIG = 10 ** 2500

    def bigrandom():
        return random.randrange(-BIG, BIG)

    # test addition
    print("testing addition/increment...", end="")
    sys.stdout.flush()
    for i in range(1, 1000):
        a, b = bigrandom(), bigrandom()
        ps = hex(a + b + 1)

        ab, bb = bn_create(a), bn_create(b)
        bn_add_to(ab, bb)
        bn_increment(ab)
        cs = bn_tohexstr(ab)

        if ps != cs:
            print("failed test:")
            print("expected:", ps, "\nactual:  ", cs, "\n")
            exit(-1)

    print(" done\ntesting subtraction/decrement...", end="")
    sys.stdout.flush()
    for i in range(1, 1000):
        a, b = bigrandom(), bigrandom()
        ps = hex(a - b - 1)

        ab, bb = bn_create(a), bn_create(b)
        bn_sub_from(ab, bb)
        bn_decrement(ab)
        cs = bn_tohexstr(ab)

        if ps != cs:
            print("failed test:")
            print("expected:", ps, "\nactual:  ", cs, "\n")
            exit(-1)

    print(" done\ntesting comparison...", end="")
    sys.stdout.flush()
    for i in range(1, 1000):
        a, b = bigrandom(), bigrandom()
        pc = 0

        ab, bb = bn_create(a), bn_create(b)
        cc = bn_compare(ab, bb)
        if a > b:
            pc = 1
        elif a < b:
            pc = -1

        if pc != cc:
            print("failed test:")
            print("expected:", pc, "\nactual:  ", cc, "\n")
            exit(-1)
    print(" done")
