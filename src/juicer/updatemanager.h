#pragma once

class UpdateManager
{ // The class
public:
  enum UpdateStatus
  {
    NotStarted = 0,
    InProgress,
    Failed,
    Success,
  };

protected:
  static UpdateManager *mManager;
  enum UpdateStatus mUpdateStatus = UpdateStatus::NotStarted;
  long mUpdateTotalSize = 0;
  long mUpdateDone = 0;

public:
  ~UpdateManager();
  static UpdateManager *getInstance()
  {
    if (!mManager)
    {
      mManager = new UpdateManager();
    }
    return mManager ;
  };
  bool canUpdate() { return mUpdateStatus != UpdateStatus::InProgress; };
  bool startUpdate(const char *url);
  long getUpdateProgress() {return (mUpdateTotalSize <= 0 || mUpdateDone > mUpdateTotalSize) ? 0 : (int)((mUpdateDone / (double)mUpdateTotalSize) * 100.0);};
  void setUpdateProgress(UpdateStatus stat, long total, long complete){
    mUpdateStatus  = stat;
    mUpdateTotalSize = total ;
    mUpdateDone += complete;
  }
  void setUpdateStatus(UpdateStatus stat){
    mUpdateStatus  = stat;
  }

  enum UpdateStatus getUpdateStatus() { return mUpdateStatus; };
};