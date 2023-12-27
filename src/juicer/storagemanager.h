#pragma once

class StorageManager
{ // The class
protected:
  static StorageManager *mManager;
  char* mBuffer = NULL ;
public:    
  ~StorageManager();
  static void setupStorage(); // Method/function declaration
  static StorageManager *getInstance();
  const char* readText(const char* path);
  String readTextOld(const char* path);
  bool writeText(const char* path, const char* s);
  void allocateBuffer(size_t size);
};