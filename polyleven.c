#include <Python.h>
#include <stdint.h>
#include <assert.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CDIV(a,b) ((a) / (b) + ((a) % (b) > 0))
#define BIT(i,n) (((i) >> (n)) & 1)
#define FLIP(i,n) ((i) ^ ((uint64_t) 1 << (n)))
#define ISASCII(kd) ((kd) == PyUnicode_1BYTE_KIND)

/*
 * An encoded mbleven model table.
 *
 * Each 8-bit integer represents an edit sequence, with using two
 * bits for a single operation.
 *
 *   01 = DELETE, 10 = INSERT, 11 = REPLACE
 *
 * For example, 13 is '1101' in binary notation, so it means
 * DELETE + REPLACE.
 */
static const uint8_t MBLEVEN_MATRIX[] = {
     3,   0,  0,  0,  0,  0,  0,  0,
     1,   0,  0,  0,  0,  0,  0,  0,
     15,  9,  6,  0,  0,  0,  0,  0,
     13,  7,  0,  0,  0,  0,  0,  0,
     5,   0,  0,  0,  0,  0,  0,  0,
     63, 39, 45, 57, 54, 30, 27,  0,
     61, 55, 31, 37, 25, 22,  0,  0,
     53, 29, 23,  0,  0,  0,  0,  0,
     21,  0,  0,  0,  0,  0,  0,  0,
};

#define MBLEVEN_MATRIX_GET(k, d) ((((k) + (k) * (k)) / 2 - 1) + (d)) * 8

static int64_t mbleven_ascii(uint8_t *s1, uint8_t *s2, int64_t len1, int64_t len2, int k)
{
    int pos;
    uint8_t m;
    int64_t i, j, c, r;

    assert(len1 > len2);
    assert(0 < k && k <= 3);

    pos = MBLEVEN_MATRIX_GET(k, len1 - len2);
    r = k + 1;

    while (MBLEVEN_MATRIX[pos]) {
        m = MBLEVEN_MATRIX[pos++];
        i = j = c = 0;
        while (i < len1 && j < len2) {
            if (s1[i] != s2[j]) {
                c++;
                if (!m) break;
                if (m & 1) i++;
                if (m & 2) j++;
                m >>= 2;
            } else {
                i++;
                j++;
            }
        }
        c += (len1 - i) + (len2 - j);
        r = MIN(r, c);
    }
    return r;
}

static int64_t mbleven(void *p1, void *p2, int64_t len1, int64_t len2,
                       int kd1, int kd2, int64_t k)
{
    int pos;
    uint8_t m;
    int64_t i, j, c, r;

    assert(len1 > len2);
    assert(0 < k && k <= 3);

    pos = MBLEVEN_MATRIX_GET(k, len1 - len2);
    r = k + 1;

    while (MBLEVEN_MATRIX[pos]) {
        m = MBLEVEN_MATRIX[pos++];
        i = j = c = 0;
        while (i < len1 && j < len2) {
            if (PyUnicode_READ(kd1, p1, i) != PyUnicode_READ(kd2, p2, j)) {
                c++;
                if (!m) break;
                if (m & 1) i++;
                if (m & 2) j++;
                m >>= 2;
            } else {
                i++;
                j++;
            }
        }
        c += (len1 - i) + (len2 - j);
        r = MIN(r, c);
    }
    return r;
}

/*
 * A hash table that maps Unicode characters to bitmap integers.
 *
 * (1 << 31) might seem odd, but it allows us to handle '\0' as a
 * valid character. Since Unicode only uses the lower 21 bits,
 * flagging 31th bit should break nothing.
 */
struct bit_table {
    uint32_t k[128];
    uint64_t v[128];
};

static uint64_t bit_table_get(struct bit_table *bt, uint32_t c)
{
    uint8_t h = c % 128;
    uint32_t k = c | ((uint32_t) 1 << 31);

    while (bt->k[h] && bt->k[h] != k)
        h++;
    return bt->k[h] == k ? bt->v[h] : 0;
}

static void bit_table_set(struct bit_table *bt, uint32_t c, int8_t i)
{
    uint8_t h = c % 128;
    uint32_t k = c | ((uint32_t) 1 << 31);

    while (bt->k[h] && bt->k[h] != k)
        h++;
    bt->k[h] = k;
    bt->v[h] |= (uint64_t) 1 << i;
}

static struct bit_table *create_bit_tables(void *p, int kd, int64_t len)
{
    int64_t i;
    struct bit_table *tables;

    tables = calloc(1, CDIV(len, 64) * sizeof(struct bit_table));
    if (tables == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    for (i = 0; i < len; i++)
        bit_table_set(&tables[i / 64], PyUnicode_READ(kd, p, i), i % 64);
    return tables;
}

/*
 * Myers' bit-parallel algorithm
 *
 * See: G. Myers. "A fast bit-vector algorithm for approximate string
 *      matching based on dynamic programming." Journal of the ACM, 1999.
 */
static int64_t myers1999(void *p1, void *p2, int64_t len1, int64_t len2,
                         int kd1, int kd2, struct bit_table *tables)
{
    uint64_t Eq, Xv, Xh, Ph, Mh, Pv, Mv;
    uint64_t *Mhc, *Phc;
    int64_t i, b, hsize, vsize;
    int64_t Score = len2;
    uint8_t Pb, Mb;

    assert(len1 > len2);
    assert(len2 > 0);

    hsize = CDIV(len1, 64);
    vsize = CDIV(len2, 64);

    Phc = malloc(hsize * 2 * sizeof(uint64_t));
    if (Phc == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    Mhc = Phc + hsize;
    memset(Phc, -1, hsize * sizeof(uint64_t));
    memset(Mhc, 0, hsize * sizeof(uint64_t));

    for (b = 0; b < vsize; b++) {
        Mv = 0;
        Pv = (uint64_t) -1;
        Score = len2;

        for (i = 0; i < len1; i++) {
            Eq = bit_table_get(&tables[b], PyUnicode_READ(kd1, p1, i));

            Pb = BIT(Phc[i / 64], i % 64);
            Mb = BIT(Mhc[i / 64], i % 64);

            Xv = Eq | Mv;
            Xh = ((((Eq | Mb) & Pv) + Pv) ^ Pv) | Eq | Mb;

            Ph = Mv | ~ (Xh | Pv);
            Mh = Pv & Xh;

            Score += BIT(Ph, len2 % 64 - 1);
            Score -= BIT(Mh, len2 % 64 - 1);

            if ((Ph >> 63) ^ Pb)
                Phc[i / 64] = FLIP(Phc[i / 64], i % 64);

            if ((Mh >> 63) ^ Mb)
                Mhc[i / 64] = FLIP(Mhc[i / 64], i % 64);

            Ph = (Ph << 1) | Pb;
            Mh = (Mh << 1) | Mb;

            Pv = Mh | ~ (Xv | Ph);
            Mv = Ph & Xv;
        }
    }
    free(Phc);
    return Score;
}

static int64_t myers1999_ascii(uint8_t *s1, uint8_t *s2, int64_t len1, int64_t len2)
{
    uint64_t Peq[256];
    uint64_t Eq, Xv, Xh, Ph, Mh, Pv, Mv, Last;
    int64_t i;
    int64_t Score = len2;

    assert(len1 < len2);

    memset(Peq, 0, sizeof(Peq));

    for (i = 0; i < len2; i++)
        Peq[s2[i]] |= (uint64_t) 1 << i;

    Mv = 0;
    Pv = (uint64_t) -1;
    Last = (uint64_t) 1 << (len2 - 1);

    for (i = 0; i < len1; i++) {
        Eq = Peq[s1[i]];

        Xv = Eq | Mv;
        Xh = (((Eq & Pv) + Pv) ^ Pv) | Eq;

        Ph = Mv | ~ (Xh | Pv);
        Mh = Pv & Xh;

        if (Ph & Last) Score++;
        if (Mh & Last) Score--;

        Ph = (Ph << 1) | 1;
        Mh = (Mh << 1);

        Pv = Mh | ~ (Xv | Ph);
        Mv = Ph & Xv;
    }
    return Score;
}

/*
 * Interface functions
 */
static int64_t levenshtein(PyObject *o1, PyObject *o2, int64_t k)
{
    void *p1 = PyUnicode_DATA(o1);
    void *p2 = PyUnicode_DATA(o2);
    int kd1 = PyUnicode_KIND(o1);
    int kd2 = PyUnicode_KIND(o2);
    int64_t len1 = PyUnicode_GET_LENGTH(o1);
    int64_t len2 = PyUnicode_GET_LENGTH(o2);

    int64_t ret;
    struct bit_table *tables;

    if (len1 < len2)
        return levenshtein(o2, o1, k);

    if (k == 0)
        return PyUnicode_Compare(o1, o2) ? 1 : 0;

    if (0 < k && k < len1 - len2)
        return k + 1;

    /* shortcut: levenshtein(s, "") == len(s) */
    if (len2 == 0)
        return len1;

    if (0 < k && k < 4) {
        if (ISASCII(kd1) && ISASCII(kd2)) {
            return mbleven_ascii(p1, p2, len1, len2, k);
        } else {
            return mbleven(p1, p2, len1, len2, kd1, kd2, k);
        }
    }

    if (ISASCII(kd1) && ISASCII(kd2) && len2 < 64)
        return myers1999_ascii(p1, p2, len1, len2);

    /* Resort to myers1999 */
    tables = create_bit_tables(p2, kd2, len2);
    if (tables == NULL)
        return -1;

    ret = myers1999(p1, p2, len1, len2, kd1, kd2, tables);
    free(tables);
    return ret;
}

static PyObject* polyleven_levenshtein(PyObject *self, PyObject *args)
{
    PyObject *o1, *o2;
    int64_t k = -1;
    int64_t ret;

    if (!PyArg_ParseTuple(args, "UU|n", &o1, &o2, &k))
        return NULL;

    ret = levenshtein(o1, o2, k);
    if (ret < 0)
        return NULL;
    if (0 < k && k < ret)
        ret = k + 1;

    return PyLong_FromLongLong(ret);
}

/*
 * Implement Python C module API
 */
static PyMethodDef polyleven_methods[] = {
    {"levenshtein", polyleven_levenshtein, METH_VARARGS,
     "Compute the levenshtein distance between two strings"},
};

static struct PyModuleDef polyleven_definition = {
    PyModuleDef_HEAD_INIT,
    "polyleven",
    "Yet another library to compute Levenshtain distance",
    -1,
    polyleven_methods
};

PyMODINIT_FUNC PyInit_polyleven(void)
{
    return PyModule_Create(&polyleven_definition);
}
