#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "../noise.h"
#include "../encounter.h"
#include "../monster_data.h"
#include "../_monster_data.h"


PyDoc_STRVAR(module_doc, "NotImplemented");
PyDoc_STRVAR(generate_doc, "NotImplemented");

PyObject *EncounterError;

static PyObject*
generate(PyObject* self, PyObject* args)
{
    EncounterInputs inputs = {0};
    PyObject* player_levels;
    uint32_t seed;
    if (!PyArg_ParseTuple(args, "OiI", &player_levels, &inputs.difficulty, &seed)) return NULL;

    inputs.number_of_characters = PySequence_Length(player_levels);
    if (inputs.number_of_characters == -1) return NULL;

    for (size_t i=0; i < inputs.number_of_characters; i++) {
        PyObject* py_long = PySequence_GetItem(player_levels, i);
        if (!py_long) return NULL;
        inputs.character_levels[i] = PyLong_AsLong(py_long);
        Py_XDECREF(py_long);
        if (inputs.character_levels[i] == -1) return NULL;
    }

    RNG rng = {.seed=seed};
    Encounter encounter = {0};
    if (generate_encounter(&inputs, &encounter, &rng)) {
        PyErr_SetString(EncounterError, "encounter generation failed");
        return NULL;
    }

    PyObject* monster_ids = PyTuple_New(encounter.number_of_enemies);
    if (!monster_ids) return NULL;
    for (size_t i=0; i < encounter.number_of_enemies; i++) {
        PyObject* pynum = PyLong_FromSize_t(encounter.monster_ids[i]);
        if (!pynum) {
            Py_DECREF(monster_ids);
            return NULL;
        }
        PyTuple_SET_ITEM(monster_ids, i, pynum);
    }
    return monster_ids;
}

static PyObject*
init(PyObject* self, PyObject* args)
{
    char* filepath;
    if (!PyArg_ParseTuple(args, "s", &filepath)) return NULL;
    init_weights_table(filepath);
    Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
    {"generate", (PyCFunction)generate, METH_VARARGS, NULL},
    {"init", (PyCFunction)init, METH_VARARGS, NULL},
    {NULL}
};

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_encounters",
    .m_doc = module_doc,
    .m_size = -1,
    .m_methods = module_methods,
};

PyMODINIT_FUNC
PyInit__encounters(void)
{
    PyObject* module = NULL;
    module = PyModule_Create(&module_def);

    EncounterError = PyErr_NewException("_encounters.EncounterError", NULL, NULL);
    Py_XINCREF(EncounterError);

    if (!module) goto error;
    if (PyModule_AddObject(module, "EncounterError", EncounterError) < 0) goto error;

    return module;

error:
    Py_XDECREF(EncounterError);
    Py_XDECREF(module);
    return NULL;
}
