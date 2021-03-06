#include <Python.h>
#include <SDL.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "unarchive.h"

void initgraphics();
void initandroid();
void initfreecivclient();
void initjnius();

static char* storage;

int SDL_main(int argc, char** argv) {
  __android_log_write(ANDROID_LOG_INFO, "freeciv", "starting SDL_main");
  char* ipath = storage = SDL_AndroidGetInternalStoragePath();
  storage = malloc(strlen(ipath) + 100);
  sprintf(storage, "%s/package", ipath);
  setenv("PYTHONHOME", storage, 1);
  char* pythonpath = malloc(strlen(storage) + 100);
  strcpy(pythonpath, storage);
  strcat(pythonpath, "/lib/python2.7");
  setenv("PYTHONPATH", pythonpath, 1);

  SDL_RWops* ops = SDL_RWFromFile("code.archive", "rb");
  if(ops == NULL) {
    __android_log_write(ANDROID_LOG_ERROR, "freeciv", "opening code assets failed");
  }
  unarchive(ops, storage);
  SDL_RWclose(ops);

  Py_Initialize();
  PyRun_SimpleString("print 'Python running'");
  initgraphics();
  initjnius();
  initandroid();
  initfreecivclient();
  PyRun_SimpleString("import android; android.main()");
  Py_Finalize();

  __android_log_write(ANDROID_LOG_INFO, "freeciv", "closing app");

  exit(0);
}

void* SDL_ANDROID_GetJNIEnv() {
  /* make pyjnius happy */
  return SDL_AndroidGetJNIEnv();
}

void unpack_res() {
  __android_log_write(ANDROID_LOG_INFO, "freeciv", "unpacking resources...");
  SDL_RWops* ops = SDL_RWFromFile("res.archive", "rb");
  if(ops == NULL) {
    __android_log_write(ANDROID_LOG_ERROR, "freeciv", "opening resource assets failed");
  }
  char* res_storage = malloc(strlen(storage) + 20);
  sprintf(res_storage, "%s/data", storage);
  unarchive(ops, res_storage);
  free(res_storage);
  SDL_RWclose(ops);
  __android_log_write(ANDROID_LOG_INFO, "freeciv", "unpacking resources done");
}
