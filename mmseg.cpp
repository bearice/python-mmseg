#include<set>
#include<string>
#include<iostream>
#include<SegmenterManager.h>
#include<Segmenter.h>
#include<Python.h>
#include<errno.h>

using namespace std;

const char* skipWords_arr[] = {
    " ","\r","\n","\t",
    ",",".","!","@","#","$","%","^","&","*","(",")","-","=","_","+",
    "\"","'","~",";",":","<",">","/","?","[","]","{","}","\\","|",
    "！","￥","⋯","（","）","—","【","】","。","，","、","？","「","」","“","”",
    NULL,
};

uint32_t decodeUTF8(const char* cp){
    unsigned char c = *cp;
    if(c<0x80) return c;
    if(c<0xE0) return ((c & 0x1F) << 6) |  (cp[1] & 0x3F);
    if(c<0xF0) return ((c & 0x0F) <<12) | ((cp[1] & 0x3F) <<  6) |  (cp[2] & 0x3F);
    if(c<0xF8) return ((c & 0x07) <<18) | ((cp[1] & 0x3F) << 12) | ((cp[2] & 0x3F) <<  6) |  (cp[3] & 0x3F);
    if(c<0xFC) return ((c & 0x03) <<24) | ((cp[1] & 0x3F) << 18) | ((cp[2] & 0x3F) << 12) | ((cp[3] & 0x3F) <<  6) |  (cp[4] & 0x3F);
    return ((c & 0x01) <<30) | ((cp[1] & 0x3F) << 24) | ((cp[2] & 0x3F) << 18) | ((cp[3] & 0x3F) << 12) | ((cp[4] & 0x3F) << 6) | (cp[5] & 0x3F);
}

set<uint32_t> skipWords;
css::SegmenterManager g_mgr;


int init(const char *path){
	if(g_mgr.init(path) != 0) {
		perror(path);
		return 1;
	}
    const char** sw = skipWords_arr;
    while(*sw){
        skipWords.insert(decodeUTF8(*sw));
        //fprintf(stderr,"\r\n%s>0x%08x\r\n",*sw,getUTF8(*sw));    
        sw++;
    }
	return 0;
}

bool isSkipWord(const char* cp){
    //fprintf(stderr,"\r\n0x%08x\r\n",getUTF8(cp));    
    return skipWords.find(decodeUTF8(cp)) != skipWords.end();
}


static PyObject* Error;

static PyObject*
mmseg_init(PyObject* self, PyObject* args)
{
    const char *path = NULL;
    if (!PyArg_ParseTuple(args, "|z", &path)){
        return NULL;
    }
    if (!path)
        path = "/opt/local/etc/mmseg";
    
    if(init(path)){
        PyErr_SetString(Error, strerror(errno));
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject*
mmseg_tokenlize(PyObject* self,PyObject* args)
{
    PyObject *list = PyList_New(0);
    const char *str;
    size_t slen;
    if (!PyArg_ParseTuple(args, "s#", &str,&slen))
        return NULL;
    
    css::Segmenter* seg = g_mgr.getSegmenter();
    seg->setBuffer((u1*)str,slen);
    u2 len, symlen;
    while(1)
    { 
        len = symlen = 0;
        char* tok = (char*)seg->peekToken(len,symlen);
        if(!tok || !*tok || !len){
            break;
        }
        if(isSkipWord(tok)){
            seg->popToken(len);
            continue;
        }
        PyList_Append(list, Py_BuildValue("(i,i,s#)", tok-str,len,tok,len));
        seg->popToken(len);
    }
    return list;
}

static PyMethodDef Methods[] = {
    {"init",       mmseg_init,      METH_VARARGS, "Initilaze library."},
    {"tokenlize",  mmseg_tokenlize, METH_VARARGS, "Tokenlize string."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initmmseg(void)
{
    PyObject *m;
    
    m = Py_InitModule("mmseg", Methods);
    if (m == NULL)
        return;
    
    Error = PyErr_NewException("mmseg.error", NULL, NULL);
    Py_INCREF(Error);
    PyModule_AddObject(m, "error", Error);
}
