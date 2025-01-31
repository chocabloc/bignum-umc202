# Modify this file to implement the count_pairs_file function
# using ultra-large integers in C/C++.
import random
import cProfile
import sys

from bignum import *


def read_file(filename: str) -> tuple[list[int], int]:
    with open(filename) as file:
        # First line is the target
        target = int(file.readline())
        # Second line is the number of integers
        n = int(file.readline())
        # Read the n integers and return them as a list
        return [int(file.readline()) for _ in range(n)], target


def count_pairs(data: list[int], target: int) -> int:
    """Count the number of pairs of
    list indices i < j such that
    data[i] - data[j] = target.
    Time complexity: Naive O(n^2).
    """
    result = 0
    n = len(data)
    for i in range(n - 1):
        for j in range(i + 1, n):
            if data[i] - data[j] == target:
                result += 1
    return result


def count_pairs_new(data: list[BigNum], target: BigNum) -> BigNum:
    result = bn_create(0)
    tmp = bn_create(0)

    n = len(data)
    for i in range(n - 1):
        for j in range(i + 1, n):
            # copyto(tmp, data[i])
            bn_sub_from(tmp, data[j])
            cmpres = bn_compare(tmp, target)
            if cmpres == 0:
                bn_increment(result)
    return result


def test_count_pairs():
    # Simple correctness tests
    assert count_pairs([1, 2, 3, 4, 5], 1) == 0
    assert count_pairs([5, 4, 3, 2, 1], 1) == 4
    assert count_pairs([1, 2, 3, 4, 5], -3) == 2
    # Test with huge integers
    assert count_pairs([10 ** 20 + 2, 10 ** 20 + 1, 10 ** 20], 1) == 2
    print("count_pairs.py: All tests passed")


def count_pairs_file(filename: str) -> int:
    data, target = read_file(filename)
    return count_pairs(data, target)


datap = []
datac = []
tgt = 20
tgtb = bn_create(tgt)

sys.set_int_max_str_digits(430000)
for i in range(0, 200):
    na = random.randrange(-10 ** 100000, 10 ** 100000)
    nb = random.randrange(-10 ** 100000, 10 ** 100000)
    if random.randint(0, 1) == 0:
        nb = na - tgt
    datap.append(na)
    datap.append(nb)

    naa, nab = bn_create(na), bn_create(nb)
    datac.append(naa)
    datac.append(nab)

print("doing regular...")

na, nb = 3 ** 1000, 4 ** 1000
# naa, nbb = bn_create(na), bn_create(nb)


def b0():
    global na, nb
    for i in range(0, 1000000):
        na = na + nb


def b1():
    for i in range(0, 1000000):
        bn_noop(1, 2)


cProfile.run('b0()')
cProfile.run('b1()')

cProfile.run('count_pairs(datap, tgt)')
cProfile.run('count_pairs_new(datac, tgtb)')
# print(hex(count_pairs(datap, tgt)))
# print("doing c...")
# print(tohexstr(count_pairs_new(datac, tgtb)))
