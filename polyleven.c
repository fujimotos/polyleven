#include <Python.h>
#include <string.h>

#define min(a,b) ((a) < (b) ? (a) : (b))

/*
 * Basic data structure for handling Unicode objects
 */
struct strbuf {
    void *data;
    Py_ssize_t kind;
    Py_ssize_t len;
};

void get_strbuf (PyObject *unicode, struct strbuf *s)
{
    s->data = PyUnicode_DATA(unicode);
    s->kind = PyUnicode_KIND(unicode);
    s->len = PyUnicode_GET_LENGTH(unicode);
}

#define strbuf_char_at(sb,idx) (PyUnicode_READ(sb->kind, sb->data, idx))

/*
 * The model table for fastcomp algorithm.
 */
static const char *matrix[] = {
      "r",  NULL,  NULL,  NULL,  NULL,  NULL,  NULL, // 1
      "d",  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
     "rr",  "id",  "di",  NULL,  NULL,  NULL,  NULL, // 2
     "rd",  "dr",  NULL,  NULL,  NULL,  NULL,  NULL,
     "dd",  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
    "rrr", "idr", "ird", "rid", "rdi", "dri", "dir", // 3
    "rrd", "rdr", "drr", "idd", "did", "ddi",  NULL,
    "rdd", "drd", "ddr",  NULL,  NULL,  NULL,  NULL,
    "ddd",  NULL,  NULL,  NULL,  NULL,  NULL,  NULL,
};

#define MATRIX_COLSIZE 7

/*
 * Fastcomp algorithm
 */
static Py_ssize_t check_model (struct strbuf *sb1, struct strbuf *sb2, Py_ssize_t k, const char *model) {

    Py_ssize_t i = 0, j = 0, c = 0;

    while (i < sb1->len && j < sb2->len) {
        if (strbuf_char_at(sb1, i) != strbuf_char_at(sb2, j)) {
            if (c >= k)
                break;
            switch (model[c]) {
                case 'd':
                    i++;
                    break;
                case 'i':
                    j++;
                    break;
                case 'r':
                    i++;
                    j++;
                    break;
            }
            c++;
        } else {
            i++;
            j++;
        }
    }
    return c + (sb1->len - i) + (sb2->len - j);
}

static Py_ssize_t fastcomp (struct strbuf *sb1, struct strbuf *sb2, Py_ssize_t k)
{
    const char *model;
    int row, col;
    Py_ssize_t res = k + 1;
    Py_ssize_t dst;

    row = k * (k + 1) / 2  - 1 + (sb1->len - sb2->len);
    for (col = 0; col < MATRIX_COLSIZE; col++) {
        model = matrix[row * MATRIX_COLSIZE + col];
        if (model == NULL)
            break;
        dst = check_model(sb1, sb2, k, model);
        res = min(res, dst);
    }

    return res;
}

/*
 * Wagner-Fischer algorithm
 */
static Py_ssize_t wagner_fischer (struct strbuf *sb1, struct strbuf *sb2)
{
    Py_ssize_t i, j, dia, tmp;
    Py_ssize_t *arr;
    Py_UCS4 chr;

    arr = malloc((sb2->len + 1) * sizeof(Py_ssize_t));
    if (arr == NULL)
        return -1;

    for (j = 0; j <= sb2->len; j++)
        arr[j] = j;

    for (i = 1; i <= sb1->len; i++) {
        chr = strbuf_char_at(sb1, i - 1);
        dia = i - 1;
        arr[0] = i;

        for (j = 1; j <= sb2->len; j++) {
            tmp = arr[j];

            if (chr != strbuf_char_at(sb2, j - 1)) {
                arr[j] = min(arr[j], arr[j - 1]);
                arr[j] = min(arr[j], dia) + 1;
            } else {
                arr[j] = dia;
            }
            dia = tmp;
        }
    }
    return arr[j - 1];
}

/*
 * Interface function
 */
static PyObject* polyleven_levenshtein(PyObject *self, PyObject *args)
{
    PyObject *u1, *u2;
    struct strbuf sb1, sb2, tmp;
    Py_ssize_t k = -1;
    Py_ssize_t res;

    if (!PyArg_ParseTuple(args, "UU|n", &u1, &u2, &k))
        return NULL;

    get_strbuf(u1, &sb1);
    get_strbuf(u2, &sb2);

    if (sb1.len - sb2.len < 0) {
        tmp = sb1;
        sb1 = sb2;
        sb2 = tmp;
    }

    if (0 <= k && k < sb1.len - sb2.len)
        return PyLong_FromSsize_t(k + 1);

    if (k == 0) {
        res = PyUnicode_Compare(u1, u2) ? 1 : 0;
    } else if (0 < k && k <= 3) {
        res = fastcomp(&sb1, &sb2, k);
    } else {
        res = wagner_fischer(&sb1, &sb2);
    }

    if (res == -1)
        return NULL;
    if (0 < k && k < res)
        res = k + 1;

    return PyLong_FromSsize_t(res);
}

/*
 * Define an entry point for importing this module
 */
static PyMethodDef polyleven_methods[] = {
    {"levenshtein", polyleven_levenshtein, METH_VARARGS, NULL}
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
