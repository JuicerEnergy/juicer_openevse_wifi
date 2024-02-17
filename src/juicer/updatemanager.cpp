#include <Arduino.h>
#include <updatemanager.h>
#include <juicer_constants.h>
#include <logging.h>
#include <http_update.h>
#include <websocket.h>
#include <lcd.h>

UpdateManager *UpdateManager::mManager = NULL ;

bool UpdateManager::startUpdate(const char *url)
{
    mUpdateTotalSize = 0 ;
    mUpdateDone = 0;
    mUpdateStatus = UpdateStatus::InProgress;
    
    // before starting the update, pause the Juicer Web Socket, so as to free some memory
    JuicerWebSocketTask::getInstance()->pauseProcessing();
    lcd.display("Updating firmware", 0, 0, 1000, LCD_DISPLAY_NOW);

    boolean bresult =  http_update_from_url_minimal(
        url, [](size_t complete, size_t total)
        { 
                // lcd.display("Updating firmware", 0, 0, 1000, LCD_DISPLAY_NOW);
                // logLine("Updating firmware %ld of %ld", complete, total);
                UpdateManager::getInstance()->setUpdateProgress(UpdateStatus::InProgress, total, complete); },
        [](int)
        {
            // lcd.display("Firmware Updated", 0, 0, 1000, LCD_DISPLAY_NOW);
            logLine("Updating firmware Success called");
            UpdateManager::getInstance()->setUpdateStatus(UpdateStatus::Success);
        },
        [](int err)
        {
            logLine("Updating firmware Failure called, error %d", err);
            UpdateManager::getInstance()->setUpdateStatus(UpdateStatus::Failed);
            JuicerWebSocketTask::getInstance()->resumeProcessing(); // if failed we can resume processing websockets.
        });

    if (bresult){
        logLine("http update command returned true");
    }else{
        logLine("http update command returned FALSE !");
    }
    return bresult ;
}
