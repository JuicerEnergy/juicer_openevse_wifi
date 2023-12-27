#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_JUICER)
#undef ENABLE_DEBUG
#endif

#include <Arduino.h>
#include "debug.h"

int currentLogLevel = 10;

/**
 * set the current logging level
 */
void setLogLevel(int level)
{
  currentLogLevel = level;
}

// /**
//  * Log the given line depending on the current log level
//  */
// void logLineLevel(const char *line, int level = 10)
// {
//   if (level > currentLogLevel)
//     return;
//   printf(line);
//   printf("\n");
// }

/**
 * Log the given line depending on the current log level
 */
char logbuffer[256];
void logLine(const char *line, ...)
{
  va_list args;
  va_start(args, line);
  vsnprintf(logbuffer, 256, line, args);
  va_end(args);

  // printf(logbuffer);
  // printf("\n");
  DBUGLN(logbuffer);
  // DEBUG_PORT.println(logbuffer);

}

/**
 * Log the given line depending on the current log level
 */
void logLineLevel(int level, const char *line, ...)
{
  if (level > currentLogLevel){
    return ;
  }  
  va_list args;
  va_start(args, line);
  vsnprintf(logbuffer, 256, line, args);
  va_end(args);

  DBUGLN(logbuffer);
  // printf(logbuffer);
  // printf("\n");
  // DEBUG_PORT.println(logbuffer);
}
