#include <Arduino.h>
#include <updatemanager.h>
#include <juicer_constants.h>
#include <logging.h>
#include <http_update.h>

UpdateManager *UpdateManager::mManager = NULL ;

bool UpdateManager::startUpdate(const char *url)
{
    mUpdateTotalSize = 0 ;
    mUpdateDone = 0;
    mUpdateStatus = UpdateStatus::InProgress;
    return http_update_from_url(
        url, [](size_t complete, size_t total)
        { 
                logLine("Updating firmware %ld of %ld", complete, total);
                UpdateManager::getInstance()->setUpdateProgress(UpdateStatus::InProgress, total, complete); },
        [](int)
        {
            logLine("Updating firmware Success called");
            UpdateManager::getInstance()->setUpdateStatus(UpdateStatus::Success);
        },
        [](int)
        {
                logLine("Updating firmware Failure called");
            UpdateManager::getInstance()->setUpdateStatus(UpdateStatus::Failed);
        });
}
