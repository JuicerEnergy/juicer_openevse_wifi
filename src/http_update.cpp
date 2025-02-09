#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_HTTP_UPATE)
#undef ENABLE_DEBUG
#endif

#include "http_update.h"
#include "lcd.h"
#include "debug.h"
#include "emonesp.h"
#include "web_server.h"
#include <MongooseHttpClient.h>
#include <Update.h>

MongooseHttpClient client;
static int lastPercent = -1;
static size_t update_total_size = 0;
static size_t update_position = 0;

long totalDownload = 0;
bool http_update_from_url_minimal(String url,
                                  std::function<void(size_t complete, size_t total)> progress,
                                  std::function<void(int)> success,
                                  std::function<void(int)> error)
{
  DBUGF("Update from URL: %s", url.c_str());
  totalDownload = 0;
  MongooseHttpClientRequest *request = client.beginRequest(url.c_str());
  if (!request){
    return false ;
  }
  request->setMethod(HTTP_GET);
  //  request->addHeader("X-hello", "world");
  request->onBody([url, progress](MongooseHttpClientResponse *response)
                  {
                    size_t total = response->contentLength();
                    long len = response->body().length();
                    totalDownload += response->body().length();
                    DBUGF("Downloaded %ld of %ld, response %d", totalDownload, total, response->respCode());
                    if (response->respCode() == 200 && len > 0){
                      if(Update.isRunning() || http_update_start(url.c_str(), total)){
                        uint8_t *data = (uint8_t *)response->body().c_str();
                        if(http_update_write(data, len)){
                          DBUGF("OTA Updated\n");
                          progress(len, total);
                        }
                      }
                    } 
                    });
  request->onResponse([](MongooseHttpClientResponse *response)
                      { DBUGF("On response %d\n", response->respCode()); });

  request->onClose([error, success]()
                   {
                      DBUGF("onClose !");
                      if(Update.isRunning())
                        {
                          DBUGF("Ending Update");
                          if(http_update_end())
                          {
                            DBUGF("Update succesful");
                            success(HTTP_UPDATE_OK);
                          } else {
                            DBUGF("Ending Update Failed!");
                            error(HTTP_UPDATE_ERROR_WRITE_FAILED);
                          }
                        } });
  client.send(request);
  return true ;
}

bool http_update_from_url(String url,
                          std::function<void(size_t complete, size_t total)> progress,
                          std::function<void(int)> success,
                          std::function<void(int)> error)
{
  DBUGF("Update from URL: %s", url.c_str());

  MongooseHttpClientRequest *request = client.beginRequest(url.c_str());
  if (request)
  {
    request->setMethod(HTTP_GET);
    request->addHeader("Connection", "keep-alive");
    DBUGF("Trying to fetch firmware from %s", url.c_str());

    request->onBody([url, progress, error, request](MongooseHttpClientResponse *response)
                    {
      DBUGF("Update onBody %d", response->respCode());
      if(response->respCode() == 200 || response->respCode() == 206)
      {
        size_t len = response->body().length();
        if (len <= 0) return ;
        size_t total = response->contentLength();
        DBUGVAR(total);
        if(Update.isRunning() || http_update_start(url, total))
        {
          uint8_t *data = (uint8_t *)response->body().c_str();
          if(http_update_write(data, len))
          {
            progress(len, total);
            return;
          } else {
            error(HTTP_UPDATE_ERROR_WRITE_FAILED);
          }
        } else {
          error(HTTP_UPDATE_ERROR_FAILED_TO_START_UPDATE);
        }
      } else if (300 <= response->respCode() && response->respCode() < 400) {
        // handle 3xx redirects (later)
        return;
      } else {
        error(response->respCode());
      }
      request->abort(); });

    request->onResponse([](MongooseHttpClientResponse *response)
                        {
                          DBUGF("Update onResponse %d", response->respCode());
                          // if(301 == response->respCode() ||
                          //    302 == response->respCode())
                          // {
                          //   MongooseString location = response->headers("Location");
                          //   DBUGVAR(location.toString());
                          //   http_update_from_url(location.toString(), progress, success, error);
                          // }
                        });

    request->onClose([success, error]()
                     {
      DBUGLN("Update onClose");
      if(Update.isRunning())
      {
        if(http_update_end())
        {
          success(HTTP_UPDATE_OK);
          restart_system();
        } else {
          error(HTTP_UPDATE_ERROR_FAILED_TO_END_UPDATE);
        }
      }
      error(HTTP_UPDATE_ERROR_FAILED_TO_END_UPDATE); });
    client.send(request);

    return true;
  }

  return false;
}

bool http_update_start(String source, size_t total)
{
  update_position = 0;
  update_total_size = total;
  if (Update.begin())
  {
    DEBUG_PORT.printf("Update Start: %s %zu\n", source.c_str(), total);

    lcd.display(F("Updating WiFi"), 0, 0, 10 * 1000, LCD_CLEAR_LINE | LCD_DISPLAY_NOW);
    lcd.display(F(""), 0, 1, 10 * 1000, LCD_CLEAR_LINE | LCD_DISPLAY_NOW);
    StaticJsonDocument<128> event;
    event["ota"] = "started";
    web_server_event(event);
    return true;
  }

  return false;
}

bool http_update_write(uint8_t *data, size_t len)
{
  // Issue #187 "HTTP update fails on ESP32 ethernet gateway" was caused by the
  // loop watchdog timing out, presumably because updating wasn't bottlenecked
  // by network and execution stayed in the Mongoose poll() method.
  feedLoopWDT();

  DBUGF("Update Writing %u, %u", update_position, len);
  size_t written = Update.write(data, len);
  DBUGVAR(written);
  if (written == len)
  {
    update_position += len;
    if (update_total_size > 0)
    {
      int percent = update_position / (update_total_size / 100);
      DBUGVAR(percent);
      DBUGVAR(lastPercent);
      if (percent != lastPercent)
      {
        String text = String(percent) + F("%");
        lcd.display(text, 0, 1, 10 * 1000, LCD_DISPLAY_NOW);

        DEBUG_PORT.printf("Update: %d%%\n", percent);

        StaticJsonDocument<128> event;
        event["ota_progress"] = percent;
        web_server_event(event);
        yield();
        lastPercent = percent;
      }
    }

    return true;
  }

  return false;
}

bool http_update_end()
{
  DBUGLN("Upload finished");
  if (Update.end(true))
  {
    DBUGF("Update Success: %u", update_position);
    lcd.display(F("Complete"), 0, 1, 10 * 1000, LCD_CLEAR_LINE | LCD_DISPLAY_NOW);
    StaticJsonDocument<128> event;
    event["ota"] = "completed";
    web_server_event(event);
    yield();
    return true;
  }
  else
  {
    DBUGF("Update failed: %d", Update.getError());
    StaticJsonDocument<128> event;
    event["ota"] = "failed";
    web_server_event(event);
    yield();
    lcd.display(F("Error"), 0, 1, 10 * 1000, LCD_CLEAR_LINE | LCD_DISPLAY_NOW);
  }

  return false;
}
