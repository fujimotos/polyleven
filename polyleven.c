#include <Python.h>
#include <stdint.h>
#include <assert.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define CEILDIV(a,b) (a / b + (a % b != 0))

#define BITMAP_GET(bm,i) (bm[i / 64] & ((uint64_t) 1 << (i % 64)))
#define BITMAP_SET(bm,i) (bm[i / 64] |= ((uint64_t) 1 << (i % 64)))
#define BITMAP_CLR(bm,i) (bm[i / 64] &= ~((uint64_t) 1 << (i % 64)))


/*
 * Basic data structure for handling Unicode objects
 */
struct strbuf {
    void *data;
    int kind;
    Py_ssize_t len;
};

static void strbuf_init(PyObject *unicode, struct strbuf *sb)
{
    sb->data = PyUnicode_DATA(unicode);
    sb->kind = PyUnicode_KIND(unicode);
    sb->len = PyUnicode_GET_LENGTH(unicode);
}

#define STRBUF_READ(sb,idx) (PyUnicode_READ((sb)->kind, (sb)->data, (idx)))
#define ISASCII(o) (PyUnicode_KIND(o) == PyUnicode_1BYTE_KIND)

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
 * Myers' bit-parallel algorithm
 *
 * See: G. Myers. "A fast bit-vector algorithm for approximate string
 *      matching based on dynamic programming." Journal of the ACM, 1999.
 */
static uint64_t myers1999_geteq(uint64_t *Peq, uint64_t *map, Py_UCS4 c)
{
    uint8_t h = c % 256;
    while (1) {
        if (map[h] == c)
            return Peq[h];
        if (map[h] == UINT64_MAX)
            return 0;
        h++;
    }
}

static void myers1999_setup(uint64_t *Peq, uint64_t *map, struct strbuf *sb, uint64_t start, uint8_t len)
{
    Py_UCS4 c;
    uint8_t h;

    memset(map, -1, sizeof(uint64_t) * 256);

    while (len--) {
        c = STRBUF_READ(sb, start + len);
        h = c % 256;
        while (map[h] != UINT64_MAX && map[h] != c)
            h++;
        if (map[h] == UINT64_MAX) {
            map[h] = c;
            Peq[h] = 0;
        }
        Peq[h] |= (uint64_t) 1 << len;
    }
}

static Py_ssize_t myers1999_simple(struct strbuf *sb1, struct strbuf *sb2)
{
    uint64_t Peq[256];
    uint64_t map[256];
    uint64_t Eq, Xv, Xh, Ph, Mh, Pv, Mv, Last;

    Py_ssize_t idx, Score;

    Mv = 0;
    Pv = ~ (uint64_t) 0;
    Score = sb2->len;
    Last = (uint64_t) 1 << (sb2->len - 1);

    myers1999_setup(Peq, map, sb2, 0, sb2->len);

    for (idx = 0; idx < sb1->len; idx++) {
        Eq = myers1999_geteq(Peq, map, STRBUF_READ(sb1, idx));

        Xv = Eq | Mv;
        Xh = (((Eq & Pv) + Pv) ^ Pv) | Eq;

        Ph = Mv | ~ (Xh | Pv);
        Mh = Pv & Xh;

        if (Ph & Last)
            Score += 1;
        if (Mh & Last)
            Score -= 1;

        Ph = (Ph << 1) | 1;
        Mh = (Mh << 1);

        Pv = Mh | ~ (Xv | Ph);
        Mv = Ph & Xv;
    }
    return Score;
}

static Py_ssize_t myers1999_block(struct strbuf *sb1, struct strbuf *sb2, uint64_t b, uint64_t *Phc, uint64_t *Mhc)
{
    uint64_t Peq[256];
    uint64_t map[256];
    uint64_t Eq, Xv, Xh, Ph, Mh, Pv, Mv, Last;
    uint8_t Pb, Mb, vlen;

    Py_ssize_t idx, start, Score;

    start = b * 64;
    vlen = MIN(64, sb2->len - start);

    Mv = 0;
    Pv = ~ (uint64_t) 0;
    Score = sb2->len;
    Last = (uint64_t) 1 << (vlen - 1);

    myers1999_setup(Peq, map, sb2, start, vlen);

    for (idx = 0; idx < sb1->len; idx++) {
        Eq = myers1999_geteq(Peq, map, STRBUF_READ(sb1, idx));

        Pb = !!BITMAP_GET(Phc, idx);
        Mb = !!BITMAP_GET(Mhc, idx);

        Xv = Eq | Mv;
        Eq |= Mb;
        Xh = (((Eq & Pv) + Pv) ^ Pv) | Eq;

        Ph = Mv | ~ (Xh | Pv);
        Mh = Pv & Xh;

        if (Ph & Last) {
            BITMAP_SET(Phc, idx);
            Score++;
        } else {
            BITMAP_CLR(Phc, idx);
        }
        if (Mh & Last) {
            BITMAP_SET(Mhc, idx);
            Score--;
        } else {
            BITMAP_CLR(Mhc, idx);
        }

        Ph = (Ph << 1) | Pb;
        Mh = (Mh << 1) | Mb;

        Pv = Mh | ~ (Xv | Ph);
        Mv = Ph & Xv;
    }
    return Score;
}

static Py_ssize_t myers1999(struct strbuf *sb1, struct strbuf *sb2)
{
    uint64_t i;
    uint64_t vmax, hmax;
    uint64_t *Phc, *Mhc;
    uint64_t res;

    if (sb2->len == 0)
        return sb1->len;

    if (sb2->len <= 64)
        return myers1999_simple(sb1, sb2);

    hmax = CEILDIV(sb1->len, 64);
    vmax = CEILDIV(sb2->len, 64);

    Phc = malloc(hmax * sizeof(uint64_t));
    Mhc = malloc(hmax * sizeof(uint64_t));

    if (Phc == NULL || Mhc == NULL) {
        PyErr_NoMemory();
        return -1;
    }

    for (i = 0; i < hmax; i++) {
        Mhc[i] = 0;
        Phc[i] = ~ (uint64_t) 0;
    }

    for (i = 0; i < vmax; i++)
        res = myers1999_block(sb1, sb2, i, Phc, Mhc);

    free(Phc);
    free(Mhc);

    return res;
}


/*
 * Interface function
 */
static PyObject* polyleven_levenshtein(PyObject *self, PyObject *args)
{
    PyObject *o1, *o2;
    struct strbuf sb1, sb2, tmp;
    Py_ssize_t k = -1;
    Py_ssize_t res;

    if (!PyArg_ParseTuple(args, "UU|n", &o1, &o2, &k))
        return NULL;

    strbuf_init(o1, &sb1);
    strbuf_init(o2, &sb2);

    if (sb1.len < sb2.len) {
        tmp = sb1;
        sb1 = sb2;
        sb2 = tmp;
    }

    if (0 <= k && k < sb1.len - sb2.len)
        return PyLong_FromSsize_t(k + 1);

    if (k == 0) {
        res = PyUnicode_Compare(o1, o2) ? 1 : 0;
    } else if (1 <= k && k <= 3) {
        if (ISASCII(o1) && ISASCII(o2)) {
            res = mbleven_ascii(PyUnicode_DATA(o1),
                                PyUnicode_DATA(o2),
                                PyUnicode_GET_LENGTH(o1),
                                PyUnicode_GET_LENGTH(o2),
                                k);
        } else {
            res = mbleven(PyUnicode_DATA(o1),
                          PyUnicode_DATA(o2),
                          PyUnicode_GET_LENGTH(o1),
                          PyUnicode_GET_LENGTH(o2),
                          PyUnicode_KIND(o1),
                          PyUnicode_KIND(o2),
                          k);
        }
    } else {
        res = myers1999(&sb1, &sb2);
    }

    if (res < 0)
        return NULL;
    if (0 < k && k < res)
        res = k + 1;

    return PyLong_FromSsize_t(res);
}

/*
 * Define an entry point for importing this module
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
